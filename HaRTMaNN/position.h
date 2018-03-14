// Position クラス
// Bitboard クラス
// PosArray クラス
// Move 構造体

#ifndef _POSITION_H_
#define _POSITION_H_

#include <vector>
#include <array>

// --------------------------
//           定数
// --------------------------

// 本来値の定義はソースファイルでするべきだろうが、
// std::arrayの要素数として使いたいし、
// コンパイラが最適化してくれるかもしれないので、
// ここで定義も同時に行う

//【駒】
extern enum piece {

	EMP = -1, // 駒が存在しない

	SE, // 味方E
	SH, // 味方H
	SF, // 味方F
	SC, // 味方C

	GE, // 相手E
	GH, // 相手H
	GF, // 相手F
	GC, // 相手C

	SK, // 先手空母
	GK, // 後手空母

};
//extern const std::array<std::array<__int8, PIECE_KIND>, 2> PIECE_ARRAY;
extern const __int8 PIECE_MIRROR[] = { GE,GH,GF,GC,SE,SH,SF,SC };

//【指し手の種類】
extern enum move_kind {
	TRAVEL,
	SUICIDE,
	SHOOT,
};

//【手番】
extern enum turn {
	FRIEND,
	ENEMY,
	BOTH,
};

//【各種値】
extern const __int8 PIECE_KIND = 4; // 駒の種類の数(先後の区別なし)
extern const __int8 SQ_NUM = 60; // 座標の数

extern const std::array<__int8, SQ_NUM> _INITIAL_POS_ARRAY; // 初期局面配列
extern const PosArray INITIAL_POS_ARRAY; // 初期局面PosArray


// ---------------------------------
//       Position(局面)クラス
// ---------------------------------

/*----------

	局面はオブジェクトとして扱う
	局面は様々なbitboardを持つ
	リストとしても、局面を保持しておく
	手番変数は持たず、常にこちら側を先手にしておく
	(手が進むごとに、局面が反転する)

										----------*/

class Position {

public: // コードが冗長になるので、全部Publicにしておく

// メンバ変数
	std::array<Bitboard,PIECE_KIND*2> pieces;
	Bitboard occupied[3];
	PosArray parray;
	unsigned __int64 hash; // ハッシュ値
	bool is_first; // ハッシュ値算出時のみ使う

	// メソッド
	Position(); // コンストラクタ
	//~Position(); // デストラクタ

	// 指し手を受け取り、自身の値を変える
	// ここで盤面の反転も行う
	void move(Move m);

	// 指し手を受け取り、それを適用した局面を返す
	// 自分自身は破壊しない
	Position moved(Move m);

	// 相手側から見た局面にする
	// (駒などもすべて反転する)
	void reverse();

	// 相手側から見た局面を返す
	// (駒なども全て反転する)
	Position reversed();

	// 王手時(相手の空母が自分の射程内に入ってるとき)、Trueを返す
	// 毎ターン最初にチェックしておく
	bool check();

	// 被王手時(自分の空母が相手の射程内に入ってるとき)、Trueを返す
	// 毎ターン最初にチェックしておく
	bool checked();

	// 移動合法手の配列を返す
	std::vector<Move> gen_travel();

	// 射撃合法手の配列を返す
	std::vector<Move> gen_shoot();

	// 引数の指し手が合法手であるか確認する
	bool legal(Move);

	// traverable() と shootable() を同時に行う
	std::array<Bitboard,2> travel_shoot_able(enum turn turn = FRIEND);

	// 指定した手番の駒が移動できる範囲をbitboardで返す
	// (移動先の駒の有無は考慮しない)
	// shootable()も同時に使う場合は、travel_shoot_able()を使うべき
	Bitboard travelable(enum turn turn = FRIEND);

	// 指定した手番の駒の全射程をbitboardで返す
	// (射撃先の駒の有無は考慮しない)
	// travelable()も同時に使う場合は、travel_shoot_able()を使うべき
	Bitboard shootable(enum turn turn = FRIEND);

	// 指定した手番の孤立した駒(隣8マスに味方の駒がない)の"個数"を返す
	__int8 alone(enum turn turn = FRIEND);

};


// ---------------------------------
//		bitboardクラス
// ---------------------------------

/*----------

	bitboardとは局面を2進数で表す方法である

	1ビットを1マスと対応付けし、
	「そのマスに駒があるとき、そのビットを立てる」などのような使い方をする

	HaRTMaNNは縦型bitboardを採用しているので、
	「1Aa→1Ab→1Ac→1Ad→1Ae→1Ba→……→1De→2Aa→……→3De」
	の順で低い桁から対応付けしている

	WINGSボードは空母も含めて60マスなので、64bitに収めることができる。
	上4桁は余るので、全て0のままにしておく

																----------*/

class Bitboard {

public:
// メンバ変数

	// 実際のbitbaord値
	// vとしては「&=」「|=」「^=」のときのみ使う
	unsigned __int64 v;

// メソッド

	Bitboard(unsigned __int64); // コンストラクタ

	// 1の数をカウントする
	__int8 count();

	// 最右ビット(ntz)を求める
	unsigned __int64 ntz();

	// 1の位置を列挙し、動的配列で返す
	std::vector<__int8> locate();

	// ビットの順序を反転する
	void reverse();

	// ビットの順序を反転したものを返す(自身は破壊しない)
	Bitboard reversed();

};

// bitboard 作成関数
// 指定した整数を持つBitboardを返す
Bitboard bb(unsigned __int64);

// bitboard ビット演算関数

Bitboard bb_not(Bitboard); // ~
Bitboard bb_and(Bitboard, Bitboard); // &
Bitboard bb_or(Bitboard, Bitboard); // |
Bitboard bb_xor(Bitboard, Bitboard); // ^
Bitboard bb_lshift(Bitboard, __int8); // <<
Bitboard bb_rshift(Bitboard, __int8); // >>

// ---------------------------------
//		PosArray(局面配列)クラス
// ---------------------------------

/*----------

	局面クラスはbitboardの他にこれを持つ

	bitboardと同じ順番でマスを見て、
	それぞれのマスの駒を格納している

	例) 初期局面:[GC,EMPTY,EMPTY,EMPTY,SF,……,SC]

	その他幾つかメソッドを追加している

										----------*/

class PosArray{

public:

// メンバ変数
	std::array<__int8, SQ_NUM> v;

// メソッド
	PosArray(std::array<__int8,SQ_NUM>); // コンストラクタ
	//~PosArray(); // デストラクタ

	// 相手側から見た局面にする
	void reverse();

	// 相手側から見た局面を返す(自身は破壊しない)
	PosArray reversed();

	// 指し手を受け取り、自身の値を変える(戻り値はなし)
	// ※reversedは行わない
	void move(Move m);

	// 指し手を受け取り、それを適用した局面を返す(自身は破壊しない)
	// ※reversedは行わない
	PosArray moved(Move m);

};

// ---------------------------------
//		Move(指し手)構造体
// ---------------------------------

/*----------

	指し手を扱う型

	以下のメンバ変数を持つ
	・移動/自爆/射撃 前の座標 (0~59)
	・　　  〃  　　 後の座標 (0~59)
	・手の種類 (TRAVEL / SUICIDE / SHOOT)

									----------*/

struct Move {
	__int8 from;
	__int8 to;
	enum move_kind kind;
	void reversed(); // 指し手を相手側から見たものにする
};

// 指し手型作成用関数
// 引数に入れた値から指し手型のオブジェクトを作り、返す
Move move(__int8 from, __int8 to, enum move_kind kind);

#endif //_POSITION_H_