// 探索部

#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "position.h"
#include <vector>
#include <unordered_map>

// 置換表はグローバル変数として使う
std::unordered_map<__int64, HashEntry> hash_table;

// INT_MAX(2,147,483,647)を無限大として扱う
static const float INF = INT_MAX;

// 評価関数 (evaluate.cppで実装)

// parent_pos	: 親局面
// value		: 親静的評価値（ルートノード時は省略）
// move			: 指し手（ルートノード時は省略）
int evaluate(Position parent_pos, int parent_value, Move move);

// 探索部(反復深化)
SearchResult search(Position pos, __int16 thinking_time);

// MTD(f)法
SearchResult mtdf(Position node, int static_value, int f, int depth);

// Null-Window-Search(置換表使用)
SearchResult null_window(Position node, int static_value, int alpha, unsigned __int8 depth, unsigned __int8 remain_depth);

// SearchResult型生成
SearchResult search_result(int value, std::string move_follow = "");

// 局面と指し手の配列を受け取り、それらの評価値を並列処理でも求め、配列で返す
std::vector<int> parallel_eval(Position pos, std::vector<Move> moves);

// int vectorを受け取り、要素の昇順にインデックスを並べ、vectorにする
std::vector<unsigned __int8> index_sort(std::vector<int> v);

// --------------------------------------
//		探索関数 戻り値構造体
// --------------------------------------

struct SearchResult {

	int value;
	std::string move_follow;
};

// --------------------------------------
//				置換表
// --------------------------------------

// 置換表エントリ構造体
struct HashEntry {
	int value;				// MTD(f)で得た局面の評価値
	std::string move_follow;// それ以降に続く指し手(""ならば、valueは下限値を表す)
	__int8 remain_depth;	// αβ探索を行った際の残り深さ
};

// 置換表追加関数
void table_new(unsigned __int64,__int8, int, std::string);

#endif // _SEARCH_H_