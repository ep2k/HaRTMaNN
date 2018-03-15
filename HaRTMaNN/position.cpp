#include "position.h"
#include "table.h"
#include <array>
#include <intrin.h>
#include <string>
#include <algorithm>

// ----------------------------
//          初期局面
// ----------------------------

// 初期局面配列
const std::array<__int8, SQ_NUM> _INITIAL_POS_ARRAY = {
	GC,EMP,EMP,EMP,SF,GH,EMP,EMP,EMP,SE,GE,EMP,EMP,EMP,SH,GF,EMP,EMP,EMP,SC,
	GF,EMP,EMP,EMP,SC,GH,EMP,EMP,EMP,SE,GE,EMP,EMP,EMP,SH,GC,EMP,EMP,EMP,SF,
	GC,EMP,EMP,EMP,SF,GK,EMP,EMP,EMP,SK,GK,EMP,EMP,EMP,SK,GF,EMP,EMP,EMP,SC
};
// 初期局面PosArray
const PosArray INITIAL_POS_ARRAY(_INITIAL_POS_ARRAY);

// ----------------------------
//      Position クラス
// ----------------------------

Position::Position() { // 初期局面として設定する

	std::array<Bitboard, PIECE_KIND * 2> pieces(INITIAL_PIECES);

	occupied[FRIEND] = bb_xor(ROWS[4], CARRIER[FRIEND]);
	occupied[ENEMY] = bb_xor(ROWS[0], CARRIER[ENEMY]);
	occupied[BOTH] = bb_or(occupied[FRIEND], occupied[ENEMY]);

	parray = INITIAL_POS_ARRAY;

	hash = 0xd28f75d1fdec47B8; // TODO

	is_first = true;
}

// 指し手を受け取り、自身の値を変える
// ここで盤面の反転も行う
void Position::move(Move m) {

	__int8 p_from = parray[m.from]; // 元の駒
	__int8 p_to = parray[m.to];

	switch (m.kind) {

	case TRAVEL:

		// -- 駒bitboard の更新 ------
		pieces[p_from].v ^= (1 << m.from) | (1 << m.to);

		// -- occupied bitboard の更新 -------
		occupied[FRIEND].v ^= (1 << m.from) | (1 << m.to);
		occupied[BOTH].v ^= (1 << m.from) | (1 << m.to);

		// -- ハッシュ値の更新 -------
		if (is_first) {
			hash -= RAND_P_SQ[p_from][m.from];
			hash += RAND_P_SQ[p_from][m.to];

			hash += TURN_HASH;
		}
		else { // 実際の盤面の向きに戻す
			hash -= RAND_P_SQ[p_from + PIECE_KIND][SQ_NUM - m.from];
			hash += RAND_P_SQ[p_to + PIECE_KIND][SQ_NUM - m.to];

			hash -= TURN_HASH;
		}
		

		break;

	case SUICIDE:

		// -- 駒bitboard の更新 ------
		pieces[p_from].v ^= 1 << m.from;
		pieces[p_to].v ^= 1 << m.to;

		// -- occupied bitboard の更新 ------
		occupied[FRIEND].v ^= 1 << m.from;
		occupied[ENEMY].v ^= 1 << m.to;
		occupied[BOTH].v ^= (1 << m.from) | (1 << m.to);

		// -- ハッシュ値の更新 -------
		if (is_first) {
			hash -= RAND_P_SQ[p_from][m.from];
			hash -= RAND_P_SQ[p_to][m.to];
			hash -= RAND_P[p_from] + RAND_P[p_to]; // 駒の数が減った

			hash += TURN_HASH;
		}
		else { // 実際の盤面の向きに戻す
			hash -= RAND_P_SQ[p_from + PIECE_KIND][SQ_NUM - m.from];
			hash -= RAND_P_SQ[p_to - PIECE_KIND][SQ_NUM - m.to];
			hash -= RAND_P[p_from + PIECE_KIND] + RAND_P[p_to - PIECE_KIND]; // 駒の数が減った

			hash -= TURN_HASH;
		}

		break;

	case SHOOT:

		// -- 駒bitboard の更新 ------
		pieces[parray[m.to]].v ^= 1 << m.to;

		// -- occupied bitboard の更新 ------
		occupied[ENEMY].v ^= 1 << m.to;
		occupied[BOTH].v ^= 1 << m.to;

		// -- ハッシュ値の更新 -------
		if (is_first) {
			hash -= RAND_P_SQ[p_to][m.to];
			hash -= RAND_P[p_to];

			hash += TURN_HASH;
		}
		else {
			hash -= RAND_P_SQ[p_to - PIECE_KIND][SQ_NUM - m.to];
			hash -= RAND_P[p_to - PIECE_KIND];

			hash -= TURN_HASH;
		}

		break;
	}
	reverse();

}

// 指し手を受け取り、それを適用した局面を返す
// 自分自身は破壊しない
Position Position::moved(Move m) {
	Position tmp;
	tmp.move(m);
	return tmp;
}

// 相手側から見た局面にする
// (駒なども全て反転する)
void Position::reverse() {

	for (int i = 0; i < PIECE_KIND * 2; i++) {
		pieces[i].reverse();
	}
	for (int i = 0; i < PIECE_KIND; i++) {
		std::swap(pieces[i], pieces[i + PIECE_KIND]);
	}
	for (int i = 0; i < 3; i++) {
		occupied[i].reverse();
	}
	std::swap(occupied[FRIEND], occupied[ENEMY]);

	parray.reverse();

}

// 相手側から見た局面を返す
// (駒なども全て反転する)
Position Position::reversed() {
	Position tmp(*this);
	tmp.reverse();
	return tmp;
}

// 王手時(相手の空母が自分の射程内に入ってるとき)、Trueを返す
// 毎ターン最初にチェックしておく
bool Position::check() {
	if (shootable().v & CARRIER[ENEMY].v) return true;
}

// 被王手時(自分の空母が相手の射程内に入ってるとき)、Trueを返す
// 毎ターン最初にチェックしておく
bool Position::checked() {
	if (shootable().v & CARRIER[FRIEND].v) return true;
}

// 移動合法手の配列を返す
std::vector<Move> Position::gen_travel() {

	

}

// 射撃合法手の配列を返す
std::vector<Move> Position::gen_travel() {

}

// 引数の指し手が合法手であるか確認する
bool Position::legal(Move m) {
	
	if (m.kind == SHOOT) {

		Bitboard tmp = travelable();

		// m.toが相手の駒でない
		if (parray[m.to] < SQ_NUM | parray[m.to] == SK) return false;

		// 射程が存在しない
		if (!tmp.v) return false;

		// move.fromが射程内に入っていない
		if (!(1 << m.from & tmp.v)) return false;

		// TODO

	}
	else if (m.kind == SUICIDE) {

		Bitboard tmp = travelable();

		// m.toが相手の駒でない
		if (parray[m.to] < SQ_NUM | parray[m.to] == SK) return false;

		// 移動範囲内に存在しない
		if (!(1 << m.from & tmp.v)) return false;

		// 

	}

	return true;
	
}

// traverable() と shootable() を同時に行う
std::array<Bitboard, 2> Position::travel_shoot_able(enum turn turn = FRIEND) {

	Bitboard travelable, shootable;
	std::array<__int8, 4> piece_array; // 確認する駒の配列

	if (turn == FRIEND) piece_array = { SE, SH, SF, SC };
	else piece_array = { GE,GH,GF,GC };

	for (int i: piece_array) {
		for (int j : pieces[i].locate()) {
			travelable.v |= TRAVEL_TABLE[i][j].v;
			shootable.v |= SHOOT_TABLE[i][j].v;
		}
	}
	return std::array<Bitboard, 2>{travelable, shootable};
}

// 指定した手番の駒が移動できる範囲をbitboardで返す
// (移動先の駒の有無は考慮しない)
// shootable()も同時に使う場合は、travel_shoot_able()を使うべき
Bitboard Position::travelable(enum turn turn = FRIEND) {

	Bitboard travelable;
	std::array<__int8, 4> piece_array; // 確認する駒の配列

	if (turn == FRIEND) piece_array = { SE, SH, SF, SC };
	else piece_array = { GE,GH,GF,GC };

	for (int i : piece_array) {
		for (int j : pieces[i].locate()) {
			travelable.v |= TRAVEL_TABLE[i][j].v;
		}
	}
	return travelable;
}

// 指定した手番の駒の射程をbitboardで返す
// (射撃先の駒の有無は考慮しない)
// shootable()も同時に使う場合は、travel_shoot_able()を使うべき
Bitboard Position::shootable(enum turn turn = FRIEND) {

	Bitboard shootable;
	std::array<__int8, 4> piece_array; // 確認する駒の配列

	if (turn == FRIEND) piece_array = { SE, SH, SF, SC };
	else piece_array = { GE,GH,GF,GC };

	for (int i : piece_array) {
		for (int j : pieces[i].locate()) {
			shootable.v |= SHOOT_TABLE[i][j].v;
		}
	}
	return shootable;
}

// 指定した手番の孤立した駒(隣8マスに味方の駒がない)の"個数"を返す
__int8 Position::alone(enum turn turn = FRIEND) {
	//TODO
}

// ----------------------------
//      Bitboard クラス
// ----------------------------

Bitboard::Bitboard(unsigned __int64 bitboard=0){
	v = bitboard;
}

// 1の数をカウントする
__int8 Bitboard::count() {
	return __popcnt64(v);
}

// 1の位置を列挙し、動的配列で返すTODO
std::vector<__int8> Bitboard::locate() {

	unsigned __int64 value = v;

	std::vector<__int8> bit_array();

	std::copy(bit_array.begin(), v2.end(), std::back_inserter(v1));
}

// ビットの順序を反転したものを返す
Bitboard Bitboard::reversed() {
	// TODO
}

// ----------------------------
//      Bitboard系関数
// ----------------------------

// 指定した整数を持つBitboardを返す
Bitboard bb(unsigned __int64 bitboard) {
	Bitboard tmp(bitboard);
	return tmp;
}

// bitboard ビット演算関数

inline Bitboard bb_not(Bitboard a) { return bb((~a.v) & SQUARE); } // ~
inline Bitboard bb_and(Bitboard a, Bitboard b) { return bb(a.v & b.v); } // &
inline Bitboard bb_or(Bitboard a, Bitboard b) { return bb(a.v | b.v); } // |
inline Bitboard bb_xor(Bitboard a, Bitboard b) { return bb(a.v ^ b.v); } // ^
inline Bitboard bb_lshift(Bitboard a, __int8 s) { return bb(a.v << s); } // <<
inline Bitboard bb_rshift(Bitboard a, __int8 s) { return bb(a.v >> s); } // >>

// ----------------------------
//		PosArray クラス
// ----------------------------

PosArray::PosArray(std::array<__int8, SQ_NUM> parray) {
	v = parray;
}

// 相手側から見た局面にする
void PosArray::reverse() {

	std::reverse(v.begin(), v.end());
	for (int i = 0; i < SQ_NUM; i++) {
		v[i] = PIECE_MIRROR[v[i]];
	}

}

// 相手側から見た局面を返す(自身は破壊しない)
PosArray PosArray::reversed() {
	
	PosArray tmp(*this);
	tmp.reverse();
	return tmp;
}

// 指し手を受け取り、自身の値を変える(戻り値はなし)
// ※reversedは行わない
void PosArray::move(Move m) {
	
	switch (m.kind) {

	case TRAVEL:
		v[m.to] = v[m.to];
		v[m.from] = EMP;
		break;

	case SUICIDE:
		v[m.from] = EMP;
		v[m.to] = EMP;
		break;

	case SHOOT:
		v[m.to] = EMP;
		break;
	}

}

// 指し手を受け取り、それを適用した局面を返す(自身は破壊しない)
// ※reversedは行わない
PosArray PosArray::moved(Move m) {
	PosArray tmp(*this);
	tmp.move(m);
	return tmp;
}

// ----------------------------
//       Move 構造体
// ----------------------------

// 指し手型作成用関数
// 引数に入れた値から指し手型のオブジェクトを作り、返す
Move move(__int8 from, __int8 to, enum move_kind kind) {
	Move obj;
	obj.from = from;
	obj.to = to;
	obj.kind = kind;
	return obj;
}

// 指し手を相手側から見たものにする
void Move::reversed() {
	from = SQ_NUM - from;
	to = SQ_NUM - to;
}