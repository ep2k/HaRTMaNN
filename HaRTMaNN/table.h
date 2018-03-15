#ifndef _TABLE_H_
#define _TABLE_H_

/*

	各種テーブルをまとめています。
	(一部テーブル(配列)でないものもあります。)

*/

#include "position.h"
#include <array>
#include <vector>

// ---------------------------------
//      ビット(bitboard)でない
// ---------------------------------

// 純粋なビットに関するテーブル

// 立っているビットの数
// (インデックス: 0~0xff)
const std::array<unsigned __int8, 0xff> COUNT_BIT {
	0, 1,
};

// 立っているビットのインデックス配列
// (インデックス:0~0xff)
const std::array<std::vector<unsigned __int8>, 0xff> LOCATE_BIT{

};

// ---------------------------------
//            bitboard
// ---------------------------------

// 盤面であるビットを表すbitboard
const Bitboard SQUARE(0xfffffffffffffff);

// 各段(1~3)を表すbitboard
const std::array<Bitboard, 3> STAGES{
	bb(0xfffff << 0),
	bb(0xfffff << 20),
	bb(0xfffff << 40)
};

// 各列(A~D)を表すbitboard
const std::array<Bitboard, 4> COLUMNS{
	bb(0x1f0001f0001f << 0),
	bb(0x1f0001f0001f << 5),
	bb(0x1f0001f0001f << 10),
	bb(0x1f0001f0001f << 15)
};

// 各行(a~e)を表すbitboard
extern const std::array<Bitboard, 5> ROWS{
	bb(0x84210842108421 << 0),
	bb(0x84210842108421 << 1),
	bb(0x84210842108421 << 2),
	bb(0x84210842108421 << 3),
	bb(0x84210842108421 << 4)
};

// 空母の位置を表すbitboard
// 0:先手, 1:後手, 2:先後両方
const std::array<Bitboard, 3> CARRIER{
	bb(0x4200000000000),
	bb(0x46200000000000),
	bb(0x48000000000000)
};

// 各駒の初期位置(pieces)を表すbitboardの配列
extern const std::array<Bitboard, PIECE_KIND*2> INITIAL_PIECES{
	bb(0x20000200),
	bb(0x400004000),
	bb(0x108000000010),
	bb(0x800000001080000),
	bb(0x40000400),
	bb(0x2000020),
	bb(0x80000000108000),
	bb(0x10800000001)
};

// TRAVEL[piece(SE,SH,SF,SC)][square(0~59)]で
// 指定した駒/マスの移動範囲をbitboardで返す
const std::array<std::array<Bitboard, SQ_NUM>, PIECE_KIND> TRAVEL_TABLE{

	// イーグル
	{},

	// ホーク
	{},

	// ファルコン
	{},

	// クロウ
	{}
};

// SHOOT[piece(SE,SH,SF,SC)][square(0~59)]で
// 指定した駒/マスの射程をbitboardで返す
const std::array<std::array<Bitboard, SQ_NUM>, PIECE_KIND> SHOOT_TABLE{

	// イーグル
	{},

	// ホーク
	{},

	// ファルコン
	{},

	// クロウ
	{}
};

// ---------------------------------
//        ハッシュ値/乱数表
// ---------------------------------

// 初期局面
extern const unsigned __int64 INITIAL_HASH;

// 駒とマスに対応する乱数表
extern const std::array<std::array<__int64, SQ_NUM>, PIECE_KIND> RAND_P_SQ;

// 駒の種類に対応する乱数表
// 射撃で駒がなくなるときに、値を引く
extern const std::array<unsigned __int64, PIECE_KIND> RAND_P;

// 手番を相手番にする乱数
extern const unsigned __int64 TURN_HASH;


#endif // _BB_TABLE_H_