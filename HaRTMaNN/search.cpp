// 探索部

/* -------------------------------------------------------------------

	探索には反復深化法を利用している
	これは2手読み→4手読み→……と徐々に深くしていくのを、
	決められた時間まで繰り返す方法である。

	実際の探索部分はMTD(f)法を利用している。
	これは評価値を近似しておくことで、
	Null Window Searchが速くなるという方法である。

	またこれらの性能を最大限に引き出すため、
	置換表を使用している。

	評価関数が差分計算を行えるように、
	親局面の評価値と、現局面に至った最後の指し手を引数に入れている。

------------------------------------------------------------------------ */

#include "position.h"
#include "search.h"
#include <iostream>
#include <thread>
#include <algorithm>
#include <string>
#include <time.h>

// ------------------------------------
//		 メイン思考部(反復深化)
// ------------------------------------

// pos			: 現局面(エンジン側からの視点)
// thinking_time: 思考時間[ミリ秒]
SearchResult search(Position pos, __int16 thinking_time) {

	int time_start = clock();

	// 現在の静的評価値を計算しておく
	// これはMTD(f)のfとして使ったり、差分計算で利用したりする
	int root_static_value = evaluate(pos); // CHECK

	__int8 depth = 2;			// mtdfをするときの探索深さ
	int f = root_static_value;	// mtdfでの評価値近似値 
	SearchResult mtdf_result;	// mtdfの結果を格納

	// 反復深化探索 (時間制限まで探索を深くし続ける)
	while ((clock() - time_start) < thinking_time) {

		mtdf_result = mtdf(pos, root_static_value, f, depth);

		f = mtdf_result.value; // 次回のfをミニマックス値にする
		depth += 2;
	}
	std::cout << "depth: " << depth << std::endl;
	std::cout << "thinking_time: " << clock() - time_start << "ms" << std::endl;
	return mtdf_result;
}

// ------------------------------------
//			  MTD(f)関数
// ------------------------------------

// node			: 探索開始ノード
// static_value : 探索開始ノードの静的評価値
// f			: ミニマックス値を見積もった値
// depth		: 探索の深さ
SearchResult mtdf(Position node, int static_value, int f, int depth) {

	SearchResult g;		// NullWindowSearchの結果を格納する
	int upper = +INF;	// g.valueの最大値
	int lower = -INF;	// g.valueの最小値
	int alpha;			// α = β = alpha としてαβを実行する(NullWindowSearch)

	g.value = f;		// fは何らかの方法でミニマックス値を見積もった値

	while (lower < upper) {
		if (g.value == lower) alpha = g.value + 1; // 前回探索開始ノードでカットされなかった
		else alpha = g.value;

		// 置換表付きαβ法で NullWindowSearch を行う
		g = null_window(node, static_value, alpha, 1, depth);

		if (g.value == alpha) lower = g.value;		// 最浅ノードでカットされていない
		else if (g.value > alpha) upper = g.value;	// カットされた
	}
	return g;
}

// ------------------------------------
//	  Null-Window_Search(置換表使用)
// ------------------------------------

// negaαβの探索窓の範囲を0にする
// remain_depth引数に0を入れるとエラー

// 最善手が確定したときは、
// "/0017T/2117T/0327S"のように、先の手から記述していく

// node			: 現在のノード
// static_value : 現ノードの静的評価値（評価関数をそのまま使用した評価値）
// alpha		: 探索窓(betaも兼ねる)
// depth		: 探索開始ノードからの深さ
// remain_depth	: 残りの探索の深さ

SearchResult null_window(Position node, int static_value, int alpha, unsigned __int8 depth, unsigned __int8 remain_depth) {

	SearchResult lower; // このノードの評価値の下限値+その時の指し手
	bool already_in_table = hash_table.find(node.hash) != hash_table.end(); // 置換表に含まれているか

	// 置換表にその局面のエントリが含まれていれば利用する
	if (already_in_table) {

		HashEntry reference = hash_table[node.hash]; // 置換表既存のデータ

		// 置換表データの残りdepthが少ないときは使わない
		if (reference.remain_depth >= remain_depth) {

			// 実際の評価値が記録されているときは、それを返す
			if (reference.move_follow != "") { return search_result(reference.value, reference.move_follow); }

			// 評価値の下限値が記録されているときは、そのまま利用する
			else { lower = search_result(reference.value); }
		}
	}

	std::vector<Move> moves = node.gen_shoot(); // 射撃手の生成

	if (moves.empty()) moves = node.gen_travel(); // 射撃手がなければ移動手の生成

	// moves全てに評価関数を適用した評価値配列valuesを作成
	std::vector<int> values = parallel_eval(node, moves);

	// valuesから小さい順に要素の添字を取得し、それを配列とする
	std::vector<unsigned __int8> order = index_sort(values);

	// 評価値最大のノードをそのまま返す
	if (remain_depth == 1) {

		SearchResult result = search_result(values[order[0]], Move_to_string(moves[order[0]]));

		// 置換表に記録しておく
		table_new(node.hash, 1, result.value, result.move_follow);

		return result;
	}

	// movesの中身をorder順に見ていく
	for (int i = 0; i != moves.size(); ++i) {

		SearchResult null_window_result = null_window(node.moved[moves[order[0]]], values[order[0]], -alpha, depth + 1, remain_depth - 1);

		if (-null_window_result.value >= lower.value) {
			lower = search_result(-null_window_result.value);

			if (lower.value >= alpha) {

				table_new(node.hash, remain_depth, lower.value); // lower.value を下限値として置換表に登録

				if (depth == 1) return search_result(lower.value); // 最も浅いノードの場合、alphaを超えた値を返す
				else return search_result(INF);					 // そうでなければ通常のカット
			}
		}
	}

	// カットされずに(lower <= alpha のまま)最後のノードまで終了

	// lowerを真の評価値として置換表に登録
	table_new(node.hash, remain_depth, lower.value, lower.move_follow);

	return lower;
}

// --------------------------------------
//			置換表追加/更新関数
// --------------------------------------

void table_new(unsigned __int64 hash, __int8 remain_depth, int value, std::string move_follow = "") {

	HashEntry new_hash_entry;
	new_hash_entry.value = value;
	new_hash_entry.move_follow = move_follow;
	new_hash_entry.remain_depth = remain_depth;

	hash_table[hash] = new_hash_entry; // 新しいハッシュエントリの追加/更新
}

// --------------------------------------
//		  評価関数並列処理関数
// --------------------------------------

// 局面と指し手の配列を受け取り、それらの評価値を並列処理でも求め、配列で返す
std::vector<int> parallel_eval(Position pos, std::vector<Move> moves) {
	// TODO
}

// --------------------------------------
//		配列インデックスソート関数
// --------------------------------------

// int vectorを受け取り、要素の昇順にインデックスを並べ、vectorにする TODO
std::vector<unsigned __int8> index_sort(std::vector<int> v) {

	// TODO

}

// --------------------------------------
//	NullWindowSearch結果構造体 生成関数
// --------------------------------------

SearchResult search_result(int value, std::string move_follow = "") {

	SearchResult tmp;
	tmp.value = value;
	tmp.move_follow = move_follow;

	return tmp;
}