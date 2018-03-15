#include "position.h"
#include "table.h"
#include <array>
#include <intrin.h>
#include <string>
#include <algorithm>

// ----------------------------
//          �����ǖ�
// ----------------------------

// �����ǖʔz��
const std::array<__int8, SQ_NUM> _INITIAL_POS_ARRAY = {
	GC,EMP,EMP,EMP,SF,GH,EMP,EMP,EMP,SE,GE,EMP,EMP,EMP,SH,GF,EMP,EMP,EMP,SC,
	GF,EMP,EMP,EMP,SC,GH,EMP,EMP,EMP,SE,GE,EMP,EMP,EMP,SH,GC,EMP,EMP,EMP,SF,
	GC,EMP,EMP,EMP,SF,GK,EMP,EMP,EMP,SK,GK,EMP,EMP,EMP,SK,GF,EMP,EMP,EMP,SC
};
// �����ǖ�PosArray
const PosArray INITIAL_POS_ARRAY(_INITIAL_POS_ARRAY);

// ----------------------------
//      Position �N���X
// ----------------------------

Position::Position() { // �����ǖʂƂ��Đݒ肷��

	std::array<Bitboard, PIECE_KIND * 2> pieces(INITIAL_PIECES);

	occupied[FRIEND] = bb_xor(ROWS[4], CARRIER[FRIEND]);
	occupied[ENEMY] = bb_xor(ROWS[0], CARRIER[ENEMY]);
	occupied[BOTH] = bb_or(occupied[FRIEND], occupied[ENEMY]);

	parray = INITIAL_POS_ARRAY;

	hash = 0xd28f75d1fdec47B8; // TODO

	is_first = true;
}

// �w������󂯎��A���g�̒l��ς���
// �����ŔՖʂ̔��]���s��
void Position::move(Move m) {

	__int8 p_from = parray[m.from]; // ���̋�
	__int8 p_to = parray[m.to];

	switch (m.kind) {

	case TRAVEL:

		// -- ��bitboard �̍X�V ------
		pieces[p_from].v ^= (1 << m.from) | (1 << m.to);

		// -- occupied bitboard �̍X�V -------
		occupied[FRIEND].v ^= (1 << m.from) | (1 << m.to);
		occupied[BOTH].v ^= (1 << m.from) | (1 << m.to);

		// -- �n�b�V���l�̍X�V -------
		if (is_first) {
			hash -= RAND_P_SQ[p_from][m.from];
			hash += RAND_P_SQ[p_from][m.to];

			hash += TURN_HASH;
		}
		else { // ���ۂ̔Ֆʂ̌����ɖ߂�
			hash -= RAND_P_SQ[p_from + PIECE_KIND][SQ_NUM - m.from];
			hash += RAND_P_SQ[p_to + PIECE_KIND][SQ_NUM - m.to];

			hash -= TURN_HASH;
		}
		

		break;

	case SUICIDE:

		// -- ��bitboard �̍X�V ------
		pieces[p_from].v ^= 1 << m.from;
		pieces[p_to].v ^= 1 << m.to;

		// -- occupied bitboard �̍X�V ------
		occupied[FRIEND].v ^= 1 << m.from;
		occupied[ENEMY].v ^= 1 << m.to;
		occupied[BOTH].v ^= (1 << m.from) | (1 << m.to);

		// -- �n�b�V���l�̍X�V -------
		if (is_first) {
			hash -= RAND_P_SQ[p_from][m.from];
			hash -= RAND_P_SQ[p_to][m.to];
			hash -= RAND_P[p_from] + RAND_P[p_to]; // ��̐���������

			hash += TURN_HASH;
		}
		else { // ���ۂ̔Ֆʂ̌����ɖ߂�
			hash -= RAND_P_SQ[p_from + PIECE_KIND][SQ_NUM - m.from];
			hash -= RAND_P_SQ[p_to - PIECE_KIND][SQ_NUM - m.to];
			hash -= RAND_P[p_from + PIECE_KIND] + RAND_P[p_to - PIECE_KIND]; // ��̐���������

			hash -= TURN_HASH;
		}

		break;

	case SHOOT:

		// -- ��bitboard �̍X�V ------
		pieces[parray[m.to]].v ^= 1 << m.to;

		// -- occupied bitboard �̍X�V ------
		occupied[ENEMY].v ^= 1 << m.to;
		occupied[BOTH].v ^= 1 << m.to;

		// -- �n�b�V���l�̍X�V -------
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

// �w������󂯎��A�����K�p�����ǖʂ�Ԃ�
// �������g�͔j�󂵂Ȃ�
Position Position::moved(Move m) {
	Position tmp;
	tmp.move(m);
	return tmp;
}

// ���葤���猩���ǖʂɂ���
// (��Ȃǂ��S�Ĕ��]����)
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

// ���葤���猩���ǖʂ�Ԃ�
// (��Ȃǂ��S�Ĕ��]����)
Position Position::reversed() {
	Position tmp(*this);
	tmp.reverse();
	return tmp;
}

// ���莞(����̋�ꂪ�����̎˒����ɓ����Ă�Ƃ�)�ATrue��Ԃ�
// ���^�[���ŏ��Ƀ`�F�b�N���Ă���
bool Position::check() {
	if (shootable().v & CARRIER[ENEMY].v) return true;
}

// �퉤�莞(�����̋�ꂪ����̎˒����ɓ����Ă�Ƃ�)�ATrue��Ԃ�
// ���^�[���ŏ��Ƀ`�F�b�N���Ă���
bool Position::checked() {
	if (shootable().v & CARRIER[FRIEND].v) return true;
}

// �ړ����@��̔z���Ԃ�
std::vector<Move> Position::gen_travel() {

	

}

// �ˌ����@��̔z���Ԃ�
std::vector<Move> Position::gen_travel() {

}

// �����̎w���肪���@��ł��邩�m�F����
bool Position::legal(Move m) {
	
	if (m.kind == SHOOT) {

		Bitboard tmp = travelable();

		// m.to������̋�łȂ�
		if (parray[m.to] < SQ_NUM | parray[m.to] == SK) return false;

		// �˒������݂��Ȃ�
		if (!tmp.v) return false;

		// move.from���˒����ɓ����Ă��Ȃ�
		if (!(1 << m.from & tmp.v)) return false;

		// TODO

	}
	else if (m.kind == SUICIDE) {

		Bitboard tmp = travelable();

		// m.to������̋�łȂ�
		if (parray[m.to] < SQ_NUM | parray[m.to] == SK) return false;

		// �ړ��͈͓��ɑ��݂��Ȃ�
		if (!(1 << m.from & tmp.v)) return false;

		// 

	}

	return true;
	
}

// traverable() �� shootable() �𓯎��ɍs��
std::array<Bitboard, 2> Position::travel_shoot_able(enum turn turn = FRIEND) {

	Bitboard travelable, shootable;
	std::array<__int8, 4> piece_array; // �m�F�����̔z��

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

// �w�肵����Ԃ̋�ړ��ł���͈͂�bitboard�ŕԂ�
// (�ړ���̋�̗L���͍l�����Ȃ�)
// shootable()�������Ɏg���ꍇ�́Atravel_shoot_able()���g���ׂ�
Bitboard Position::travelable(enum turn turn = FRIEND) {

	Bitboard travelable;
	std::array<__int8, 4> piece_array; // �m�F�����̔z��

	if (turn == FRIEND) piece_array = { SE, SH, SF, SC };
	else piece_array = { GE,GH,GF,GC };

	for (int i : piece_array) {
		for (int j : pieces[i].locate()) {
			travelable.v |= TRAVEL_TABLE[i][j].v;
		}
	}
	return travelable;
}

// �w�肵����Ԃ̋�̎˒���bitboard�ŕԂ�
// (�ˌ���̋�̗L���͍l�����Ȃ�)
// shootable()�������Ɏg���ꍇ�́Atravel_shoot_able()���g���ׂ�
Bitboard Position::shootable(enum turn turn = FRIEND) {

	Bitboard shootable;
	std::array<__int8, 4> piece_array; // �m�F�����̔z��

	if (turn == FRIEND) piece_array = { SE, SH, SF, SC };
	else piece_array = { GE,GH,GF,GC };

	for (int i : piece_array) {
		for (int j : pieces[i].locate()) {
			shootable.v |= SHOOT_TABLE[i][j].v;
		}
	}
	return shootable;
}

// �w�肵����Ԃ̌Ǘ�������(��8�}�X�ɖ����̋�Ȃ�)��"��"��Ԃ�
__int8 Position::alone(enum turn turn = FRIEND) {
	//TODO
}

// ----------------------------
//      Bitboard �N���X
// ----------------------------

Bitboard::Bitboard(unsigned __int64 bitboard=0){
	v = bitboard;
}

// 1�̐����J�E���g����
__int8 Bitboard::count() {
	return __popcnt64(v);
}

// 1�̈ʒu��񋓂��A���I�z��ŕԂ�TODO
std::vector<__int8> Bitboard::locate() {

	unsigned __int64 value = v;

	std::vector<__int8> bit_array();

	std::copy(bit_array.begin(), v2.end(), std::back_inserter(v1));
}

// �r�b�g�̏����𔽓]�������̂�Ԃ�
Bitboard Bitboard::reversed() {
	// TODO
}

// ----------------------------
//      Bitboard�n�֐�
// ----------------------------

// �w�肵������������Bitboard��Ԃ�
Bitboard bb(unsigned __int64 bitboard) {
	Bitboard tmp(bitboard);
	return tmp;
}

// bitboard �r�b�g���Z�֐�

inline Bitboard bb_not(Bitboard a) { return bb((~a.v) & SQUARE); } // ~
inline Bitboard bb_and(Bitboard a, Bitboard b) { return bb(a.v & b.v); } // &
inline Bitboard bb_or(Bitboard a, Bitboard b) { return bb(a.v | b.v); } // |
inline Bitboard bb_xor(Bitboard a, Bitboard b) { return bb(a.v ^ b.v); } // ^
inline Bitboard bb_lshift(Bitboard a, __int8 s) { return bb(a.v << s); } // <<
inline Bitboard bb_rshift(Bitboard a, __int8 s) { return bb(a.v >> s); } // >>

// ----------------------------
//		PosArray �N���X
// ----------------------------

PosArray::PosArray(std::array<__int8, SQ_NUM> parray) {
	v = parray;
}

// ���葤���猩���ǖʂɂ���
void PosArray::reverse() {

	std::reverse(v.begin(), v.end());
	for (int i = 0; i < SQ_NUM; i++) {
		v[i] = PIECE_MIRROR[v[i]];
	}

}

// ���葤���猩���ǖʂ�Ԃ�(���g�͔j�󂵂Ȃ�)
PosArray PosArray::reversed() {
	
	PosArray tmp(*this);
	tmp.reverse();
	return tmp;
}

// �w������󂯎��A���g�̒l��ς���(�߂�l�͂Ȃ�)
// ��reversed�͍s��Ȃ�
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

// �w������󂯎��A�����K�p�����ǖʂ�Ԃ�(���g�͔j�󂵂Ȃ�)
// ��reversed�͍s��Ȃ�
PosArray PosArray::moved(Move m) {
	PosArray tmp(*this);
	tmp.move(m);
	return tmp;
}

// ----------------------------
//       Move �\����
// ----------------------------

// �w����^�쐬�p�֐�
// �����ɓ��ꂽ�l����w����^�̃I�u�W�F�N�g�����A�Ԃ�
Move move(__int8 from, __int8 to, enum move_kind kind) {
	Move obj;
	obj.from = from;
	obj.to = to;
	obj.kind = kind;
	return obj;
}

// �w����𑊎葤���猩�����̂ɂ���
void Move::reversed() {
	from = SQ_NUM - from;
	to = SQ_NUM - to;
}