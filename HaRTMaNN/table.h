#ifndef _TABLE_H_
#define _TABLE_H_

/*

	�e��e�[�u�����܂Ƃ߂Ă��܂��B
	(�ꕔ�e�[�u��(�z��)�łȂ����̂�����܂��B)

*/

#include "position.h"
#include <array>
#include <vector>

// ---------------------------------
//      �r�b�g(bitboard)�łȂ�
// ---------------------------------

// �����ȃr�b�g�Ɋւ���e�[�u��

// �����Ă���r�b�g�̐�
// (�C���f�b�N�X: 0~0xff)
const std::array<unsigned __int8, 0xff> COUNT_BIT {
	0, 1,
};

// �����Ă���r�b�g�̃C���f�b�N�X�z��
// (�C���f�b�N�X:0~0xff)
const std::array<std::vector<unsigned __int8>, 0xff> LOCATE_BIT{

};

// ---------------------------------
//            bitboard
// ---------------------------------

// �Ֆʂł���r�b�g��\��bitboard
const Bitboard SQUARE(0xfffffffffffffff);

// �e�i(1~3)��\��bitboard
const std::array<Bitboard, 3> STAGES{
	bb(0xfffff << 0),
	bb(0xfffff << 20),
	bb(0xfffff << 40)
};

// �e��(A~D)��\��bitboard
const std::array<Bitboard, 4> COLUMNS{
	bb(0x1f0001f0001f << 0),
	bb(0x1f0001f0001f << 5),
	bb(0x1f0001f0001f << 10),
	bb(0x1f0001f0001f << 15)
};

// �e�s(a~e)��\��bitboard
extern const std::array<Bitboard, 5> ROWS{
	bb(0x84210842108421 << 0),
	bb(0x84210842108421 << 1),
	bb(0x84210842108421 << 2),
	bb(0x84210842108421 << 3),
	bb(0x84210842108421 << 4)
};

// ���̈ʒu��\��bitboard
// 0:���, 1:���, 2:��㗼��
const std::array<Bitboard, 3> CARRIER{
	bb(0x4200000000000),
	bb(0x46200000000000),
	bb(0x48000000000000)
};

// �e��̏����ʒu(pieces)��\��bitboard�̔z��
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

// TRAVEL[piece(SE,SH,SF,SC)][square(0~59)]��
// �w�肵����/�}�X�̈ړ��͈͂�bitboard�ŕԂ�
const std::array<std::array<Bitboard, SQ_NUM>, PIECE_KIND> TRAVEL_TABLE{

	// �C�[�O��
	{},

	// �z�[�N
	{},

	// �t�@���R��
	{},

	// �N���E
	{}
};

// SHOOT[piece(SE,SH,SF,SC)][square(0~59)]��
// �w�肵����/�}�X�̎˒���bitboard�ŕԂ�
const std::array<std::array<Bitboard, SQ_NUM>, PIECE_KIND> SHOOT_TABLE{

	// �C�[�O��
	{},

	// �z�[�N
	{},

	// �t�@���R��
	{},

	// �N���E
	{}
};

// ---------------------------------
//        �n�b�V���l/�����\
// ---------------------------------

// �����ǖ�
extern const unsigned __int64 INITIAL_HASH;

// ��ƃ}�X�ɑΉ����闐���\
extern const std::array<std::array<__int64, SQ_NUM>, PIECE_KIND> RAND_P_SQ;

// ��̎�ނɑΉ����闐���\
// �ˌ��ŋ�Ȃ��Ȃ�Ƃ��ɁA�l������
extern const std::array<unsigned __int64, PIECE_KIND> RAND_P;

// ��Ԃ𑊎�Ԃɂ��闐��
extern const unsigned __int64 TURN_HASH;


#endif // _BB_TABLE_H_