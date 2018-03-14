// Position �N���X
// Bitboard �N���X
// PosArray �N���X
// Move �\����

#ifndef _POSITION_H_
#define _POSITION_H_

#include <vector>
#include <array>

// --------------------------
//           �萔
// --------------------------

// �{���l�̒�`�̓\�[�X�t�@�C���ł���ׂ����낤���A
// std::array�̗v�f���Ƃ��Ďg���������A
// �R���p�C�����œK�����Ă���邩������Ȃ��̂ŁA
// �����Œ�`�������ɍs��

//�y��z
extern enum piece {

	EMP = -1, // ����݂��Ȃ�

	SE, // ����E
	SH, // ����H
	SF, // ����F
	SC, // ����C

	GE, // ����E
	GH, // ����H
	GF, // ����F
	GC, // ����C

	SK, // �����
	GK, // �����

};
//extern const std::array<std::array<__int8, PIECE_KIND>, 2> PIECE_ARRAY;
extern const __int8 PIECE_MIRROR[] = { GE,GH,GF,GC,SE,SH,SF,SC };

//�y�w����̎�ށz
extern enum move_kind {
	TRAVEL,
	SUICIDE,
	SHOOT,
};

//�y��ԁz
extern enum turn {
	FRIEND,
	ENEMY,
	BOTH,
};

//�y�e��l�z
extern const __int8 PIECE_KIND = 4; // ��̎�ނ̐�(���̋�ʂȂ�)
extern const __int8 SQ_NUM = 60; // ���W�̐�

extern const std::array<__int8, SQ_NUM> _INITIAL_POS_ARRAY; // �����ǖʔz��
extern const PosArray INITIAL_POS_ARRAY; // �����ǖ�PosArray


// ---------------------------------
//       Position(�ǖ�)�N���X
// ---------------------------------

/*----------

	�ǖʂ̓I�u�W�F�N�g�Ƃ��Ĉ���
	�ǖʂ͗l�X��bitboard������
	���X�g�Ƃ��Ă��A�ǖʂ�ێ����Ă���
	��ԕϐ��͎������A��ɂ����瑤����ɂ��Ă���
	(�肪�i�ނ��ƂɁA�ǖʂ����]����)

										----------*/

class Position {

public: // �R�[�h���璷�ɂȂ�̂ŁA�S��Public�ɂ��Ă���

// �����o�ϐ�
	std::array<Bitboard,PIECE_KIND*2> pieces;
	Bitboard occupied[3];
	PosArray parray;
	unsigned __int64 hash; // �n�b�V���l
	bool is_first; // �n�b�V���l�Z�o���̂ݎg��

	// ���\�b�h
	Position(); // �R���X�g���N�^
	//~Position(); // �f�X�g���N�^

	// �w������󂯎��A���g�̒l��ς���
	// �����ŔՖʂ̔��]���s��
	void move(Move m);

	// �w������󂯎��A�����K�p�����ǖʂ�Ԃ�
	// �������g�͔j�󂵂Ȃ�
	Position moved(Move m);

	// ���葤���猩���ǖʂɂ���
	// (��Ȃǂ����ׂĔ��]����)
	void reverse();

	// ���葤���猩���ǖʂ�Ԃ�
	// (��Ȃǂ��S�Ĕ��]����)
	Position reversed();

	// ���莞(����̋�ꂪ�����̎˒����ɓ����Ă�Ƃ�)�ATrue��Ԃ�
	// ���^�[���ŏ��Ƀ`�F�b�N���Ă���
	bool check();

	// �퉤�莞(�����̋�ꂪ����̎˒����ɓ����Ă�Ƃ�)�ATrue��Ԃ�
	// ���^�[���ŏ��Ƀ`�F�b�N���Ă���
	bool checked();

	// �ړ����@��̔z���Ԃ�
	std::vector<Move> gen_travel();

	// �ˌ����@��̔z���Ԃ�
	std::vector<Move> gen_shoot();

	// �����̎w���肪���@��ł��邩�m�F����
	bool legal(Move);

	// traverable() �� shootable() �𓯎��ɍs��
	std::array<Bitboard,2> travel_shoot_able(enum turn turn = FRIEND);

	// �w�肵����Ԃ̋�ړ��ł���͈͂�bitboard�ŕԂ�
	// (�ړ���̋�̗L���͍l�����Ȃ�)
	// shootable()�������Ɏg���ꍇ�́Atravel_shoot_able()���g���ׂ�
	Bitboard travelable(enum turn turn = FRIEND);

	// �w�肵����Ԃ̋�̑S�˒���bitboard�ŕԂ�
	// (�ˌ���̋�̗L���͍l�����Ȃ�)
	// travelable()�������Ɏg���ꍇ�́Atravel_shoot_able()���g���ׂ�
	Bitboard shootable(enum turn turn = FRIEND);

	// �w�肵����Ԃ̌Ǘ�������(��8�}�X�ɖ����̋�Ȃ�)��"��"��Ԃ�
	__int8 alone(enum turn turn = FRIEND);

};


// ---------------------------------
//		bitboard�N���X
// ---------------------------------

/*----------

	bitboard�Ƃ͋ǖʂ�2�i���ŕ\�����@�ł���

	1�r�b�g��1�}�X�ƑΉ��t�����A
	�u���̃}�X�ɋ����Ƃ��A���̃r�b�g�𗧂Ă�v�Ȃǂ̂悤�Ȏg����������

	HaRTMaNN�͏c�^bitboard���̗p���Ă���̂ŁA
	�u1Aa��1Ab��1Ac��1Ad��1Ae��1Ba���c�c��1De��2Aa���c�c��3De�v
	�̏��ŒႢ������Ή��t�����Ă���

	WINGS�{�[�h�͋����܂߂�60�}�X�Ȃ̂ŁA64bit�Ɏ��߂邱�Ƃ��ł���B
	��4���͗]��̂ŁA�S��0�̂܂܂ɂ��Ă���

																----------*/

class Bitboard {

public:
// �����o�ϐ�

	// ���ۂ�bitbaord�l
	// v�Ƃ��Ắu&=�v�u|=�v�u^=�v�̂Ƃ��̂ݎg��
	unsigned __int64 v;

// ���\�b�h

	Bitboard(unsigned __int64); // �R���X�g���N�^

	// 1�̐����J�E���g����
	__int8 count();

	// �ŉE�r�b�g(ntz)�����߂�
	unsigned __int64 ntz();

	// 1�̈ʒu��񋓂��A���I�z��ŕԂ�
	std::vector<__int8> locate();

	// �r�b�g�̏����𔽓]����
	void reverse();

	// �r�b�g�̏����𔽓]�������̂�Ԃ�(���g�͔j�󂵂Ȃ�)
	Bitboard reversed();

};

// bitboard �쐬�֐�
// �w�肵������������Bitboard��Ԃ�
Bitboard bb(unsigned __int64);

// bitboard �r�b�g���Z�֐�

Bitboard bb_not(Bitboard); // ~
Bitboard bb_and(Bitboard, Bitboard); // &
Bitboard bb_or(Bitboard, Bitboard); // |
Bitboard bb_xor(Bitboard, Bitboard); // ^
Bitboard bb_lshift(Bitboard, __int8); // <<
Bitboard bb_rshift(Bitboard, __int8); // >>

// ---------------------------------
//		PosArray(�ǖʔz��)�N���X
// ---------------------------------

/*----------

	�ǖʃN���X��bitboard�̑��ɂ��������

	bitboard�Ɠ������ԂŃ}�X�����āA
	���ꂼ��̃}�X�̋���i�[���Ă���

	��) �����ǖ�:[GC,EMPTY,EMPTY,EMPTY,SF,�c�c,SC]

	���̑�������\�b�h��ǉ����Ă���

										----------*/

class PosArray{

public:

// �����o�ϐ�
	std::array<__int8, SQ_NUM> v;

// ���\�b�h
	PosArray(std::array<__int8,SQ_NUM>); // �R���X�g���N�^
	//~PosArray(); // �f�X�g���N�^

	// ���葤���猩���ǖʂɂ���
	void reverse();

	// ���葤���猩���ǖʂ�Ԃ�(���g�͔j�󂵂Ȃ�)
	PosArray reversed();

	// �w������󂯎��A���g�̒l��ς���(�߂�l�͂Ȃ�)
	// ��reversed�͍s��Ȃ�
	void move(Move m);

	// �w������󂯎��A�����K�p�����ǖʂ�Ԃ�(���g�͔j�󂵂Ȃ�)
	// ��reversed�͍s��Ȃ�
	PosArray moved(Move m);

};

// ---------------------------------
//		Move(�w����)�\����
// ---------------------------------

/*----------

	�w����������^

	�ȉ��̃����o�ϐ�������
	�E�ړ�/����/�ˌ� �O�̍��W (0~59)
	�E�@�@  �V  �@�@ ��̍��W (0~59)
	�E��̎�� (TRAVEL / SUICIDE / SHOOT)

									----------*/

struct Move {
	__int8 from;
	__int8 to;
	enum move_kind kind;
	void reversed(); // �w����𑊎葤���猩�����̂ɂ���
};

// �w����^�쐬�p�֐�
// �����ɓ��ꂽ�l����w����^�̃I�u�W�F�N�g�����A�Ԃ�
Move move(__int8 from, __int8 to, enum move_kind kind);

#endif //_POSITION_H_