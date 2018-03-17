// �]���֐�

/* -----------------------------------------------------------------------------

	�\�ߒ�߂Ă���ǖʂ̓����ʂ���_����t����(���L�Q��)
	�����̓_���͈�`�I�A���S���Y���ɂ���Ċw�K����

	�����Ƃ��āA�e�ǖʂ̕]���l�Ƃ��̕]���l�������Ă���(�����v�Z�Ȃǂ̂���)
	�����������ʌ^�ł͍����v�Z���s��Ȃ��̂ŁA�����ł͍Ō�܂Ŏg�p���Ȃ�

	# ------------------------
	#   �����ʂ̈ꗗ
	# ------------------------

	0~3   : 1�i�ڂ̂��ꂼ��̋�̌���
	4~7   : 2 �V
	8~11  : 3 �V
	12    : ���݂��̓G�w�i3�i�ڂ̉�����2��ڂ܂Łj�ɂ����̐��̍�
	13    : �Ǘ������i�ׂ̃}�X�ɖ����̋�Ȃ��j��̌���
	14    : �ړ��͈͂̃}�X�̌���
	15    : �˒��̃}�X�̌���
	16    : �˒����̑���̋�̌���
	17~19 : �i�̐�L��(�i�̎����̋�̐� / �S��̐�)
	20    : �˒����ɑ���̋�ꂪ���݂��邩�ۂ�
	21    : ��̓_���ɂ�����W��

	���ۂ̒l(2018.3.01)
	0~20 : [4, 2, 1, 4, 1, 5, 4, 8, 8, 8, 1, 9, 4, 4, 8, 2, 2, 1, 0, 7, 8]
	21 : 8

-------------------------------------------------------------------------------- */

#include "position.h"
#include "table.h"
#include <array>

// ------------------------------
//	�p�����[�^ (2018.3.01�X�V)
// ------------------------------

// GA�Ŋw�K��������
std::array<__int8, 21>PARAMETERS{ 32, 16, 8, 32, 8, 40, 32, 64, 64, 64, 8, 72, 4, 4, 8, 2, 2, 1, 0, 7, 8 };

// ------------------------------
//	�]���֐�
// ------------------------------

static const Move NULL_MOVE = move(-1, -1, TRAVEL); // move������̂Ƃ��A���[�g�m�[�h

int evaluate(Position parent_pos, int parent_value = INT_MAX, Move move = NULL_MOVE) {

	// ���݂̋ǖʂ����߁Anow_pos�Ƃ���
	Position now_pos;
	if (parent_value == INT_MAX) now_pos = parent_pos;
	else now_pos = parent_pos.moved(move);
	
	// PARAMETER�ƑΉ��t���Ēl���i�[���Ă���
	// �Ō��PARAMETER�Ƃ̓��ς����A�����]���l�Ƃ��ĕԂ�
	std::array<__int8, 21> values;
	values.fill(0);

	//�y0~11�z1�`3�i�ڂ̂��ꂼ��̋�̌���
	for (int i : std::array<__int8, PIECE_KIND>{0, 1, 2, 3}) {
		for (int j; j < 4; j++) {
			values[i + j * 4] += bb_and(now_pos.pieces[i],STAGES[j]).count();
			values[i + j * 4] -= bb_and(now_pos.pieces[PIECE_MIRROR[i]], STAGES[j]).count();
		}
	}
	//�y12�z���݂��̓G�w�i3�i�ڂ̉�����2��ڂ܂Łj�ɂ����̐��̍�
	values[12] += bb_and(now_pos.occupied[FRIEND], bb_or(ROWS[0], ROWS[1])).count();
	values[12] -= bb_and(now_pos.occupied[ENEMY], bb_or(ROWS[3], ROWS[4])).count();

	//�y13�z�Ǘ������i�ׂ̃}�X�ɖ����̋�Ȃ��j��̌���
	values[13] += now_pos.alone(FRIEND) - now_pos.alone(ENEMY);

	//�y14,15,16�z�ړ��͈�/�˒��̃}�X�̌����A�˒����̑���̋�̌���

	// tmp = (�����ړ��\bitboard, �����˒�bitboard)
	std::array<Bitboard, 2> tmp = now_pos.travel_shoot_able(FRIEND);

	values[14] += tmp[0].count();
	values[15] += tmp[1].count();
	values[16] += bb_and(tmp[1], now_pos.occupied[ENEMY]).count();

	// tmp = (�G�ړ��\bitboard,�G�˒�bitboard)
	tmp = now_pos.travel_shoot_able(ENEMY);

	values[14] -= tmp[0].count();
	values[15] -= tmp[1].count();
	values[16] -= bb_and(tmp[1], now_pos.occupied[FRIEND]).count();

	//�y17~19�z�i�̐�L��(�i�̎����̋�̐� / �S��̐�)TODO
	for (__int8 i = 0; i < 3; i++) {
		values[17 + i] += bb_and(now_pos.occupied[FRIEND], STAGES[i]).count();
	}

	//�y20�z�˒����ɑ���̋�ꂪ���݂��邩�ۂ�
	values[20] = now_pos.check();

	// PARAMETERS��values�̓���
	int dot;
	for (int i = 0; i < 20; i++) {
		dot += PARAMETERS[i] * values[i];
	}
	return dot;
}