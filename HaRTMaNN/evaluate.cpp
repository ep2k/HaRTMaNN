// 評価関数

/* -----------------------------------------------------------------------------

	予め定めてある局面の特徴量から点数を付ける(下記参照)
	これらの点数は遺伝的アルゴリズムによって学習した

	引数として、親局面の評価値とその評価値も送っている(差分計算などのため)
	しかし特徴量型では差分計算を行わないので、ここでは最後まで使用しない

	# ------------------------
	#   特徴量の一覧
	# ------------------------

	0~3   : 1段目のそれぞれの駒の個数差
	4~7   : 2 〃
	8~11  : 3 〃
	12    : お互いの敵陣（3段目の奥から2列目まで）にいる駒の数の差
	13    : 孤立した（隣のマスに味方の駒がない）駒の個数差
	14    : 移動範囲のマスの個数差
	15    : 射程のマスの個数差
	16    : 射程内の相手の駒の個数差
	17~19 : 段の占有率(段の自分の駒の数 / 全駒の数)
	20    : 射程内に相手の空母が存在するか否か
	21    : 駒の点数にかける係数

	実際の値(2018.3.01)
	0~20 : [4, 2, 1, 4, 1, 5, 4, 8, 8, 8, 1, 9, 4, 4, 8, 2, 2, 1, 0, 7, 8]
	21 : 8

-------------------------------------------------------------------------------- */

#include "position.h"
#include "table.h"
#include <array>

// ------------------------------
//	パラメータ (2018.3.01更新)
// ------------------------------

// GAで学習したもの
std::array<__int8, 21>PARAMETERS{ 32, 16, 8, 32, 8, 40, 32, 64, 64, 64, 8, 72, 4, 4, 8, 2, 2, 1, 0, 7, 8 };

// ------------------------------
//	評価関数
// ------------------------------

static const Move NULL_MOVE = move(-1, -1, TRAVEL); // moveがこれのとき、ルートノード

int evaluate(Position parent_pos, int parent_value = INT_MAX, Move move = NULL_MOVE) {

	// 現在の局面を求め、now_posとする
	Position now_pos;
	if (parent_value == INT_MAX) now_pos = parent_pos;
	else now_pos = parent_pos.moved(move);
	
	// PARAMETERと対応付けて値を格納しておく
	// 最後にPARAMETERとの内積を取り、それを評価値として返す
	std::array<__int8, 21> values;
	values.fill(0);

	//【0~11】1～3段目のそれぞれの駒の個数差
	for (int i : std::array<__int8, PIECE_KIND>{0, 1, 2, 3}) {
		for (int j; j < 4; j++) {
			values[i + j * 4] += bb_and(now_pos.pieces[i],STAGES[j]).count();
			values[i + j * 4] -= bb_and(now_pos.pieces[PIECE_MIRROR[i]], STAGES[j]).count();
		}
	}
	//【12】お互いの敵陣（3段目の奥から2列目まで）にいる駒の数の差
	values[12] += bb_and(now_pos.occupied[FRIEND], bb_or(ROWS[0], ROWS[1])).count();
	values[12] -= bb_and(now_pos.occupied[ENEMY], bb_or(ROWS[3], ROWS[4])).count();

	//【13】孤立した（隣のマスに味方の駒がない）駒の個数差
	values[13] += now_pos.alone(FRIEND) - now_pos.alone(ENEMY);

	//【14,15,16】移動範囲/射程のマスの個数差、射程内の相手の駒の個数差

	// tmp = (味方移動可能bitboard, 味方射程bitboard)
	std::array<Bitboard, 2> tmp = now_pos.travel_shoot_able(FRIEND);

	values[14] += tmp[0].count();
	values[15] += tmp[1].count();
	values[16] += bb_and(tmp[1], now_pos.occupied[ENEMY]).count();

	// tmp = (敵移動可能bitboard,敵射程bitboard)
	tmp = now_pos.travel_shoot_able(ENEMY);

	values[14] -= tmp[0].count();
	values[15] -= tmp[1].count();
	values[16] -= bb_and(tmp[1], now_pos.occupied[FRIEND]).count();

	//【17~19】段の占有率(段の自分の駒の数 / 全駒の数)TODO
	for (__int8 i = 0; i < 3; i++) {
		values[17 + i] += bb_and(now_pos.occupied[FRIEND], STAGES[i]).count();
	}

	//【20】射程内に相手の空母が存在するか否か
	values[20] = now_pos.check();

	// PARAMETERSとvaluesの内積
	int dot;
	for (int i = 0; i < 20; i++) {
		dot += PARAMETERS[i] * values[i];
	}
	return dot;
}