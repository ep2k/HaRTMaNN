// �T����

/* -------------------------------------------------------------------

	�T���ɂ͔����[���@�𗘗p���Ă���
	�����2��ǂ݁�4��ǂ݁��c�c�Ə��X�ɐ[�����Ă����̂��A
	���߂�ꂽ���Ԃ܂ŌJ��Ԃ����@�ł���B

	���ۂ̒T��������MTD(f)�@�𗘗p���Ă���B
	����͕]���l���ߎ����Ă������ƂŁA
	Null Window Search�������Ȃ�Ƃ������@�ł���B

	�܂������̐��\���ő���Ɉ����o�����߁A
	�u���\���g�p���Ă���B

	�]���֐��������v�Z���s����悤�ɁA
	�e�ǖʂ̕]���l�ƁA���ǖʂɎ������Ō�̎w����������ɓ���Ă���B

------------------------------------------------------------------------ */

#include "position.h"
#include "search.h"
#include <iostream>
#include <thread>
#include <algorithm>
#include <string>
#include <time.h>

// ------------------------------------
//		 ���C���v�l��(�����[��)
// ------------------------------------

// pos			: ���ǖ�(�G���W��������̎��_)
// thinking_time: �v�l����[�~���b]
SearchResult search(Position pos, __int16 thinking_time) {

	int time_start = clock();

	// ���݂̐ÓI�]���l���v�Z���Ă���
	// �����MTD(f)��f�Ƃ��Ďg������A�����v�Z�ŗ��p�����肷��
	int root_static_value = evaluate(pos); // CHECK

	__int8 depth = 2;			// mtdf������Ƃ��̒T���[��
	int f = root_static_value;	// mtdf�ł̕]���l�ߎ��l 
	SearchResult mtdf_result;	// mtdf�̌��ʂ��i�[

	// �����[���T�� (���Ԑ����܂ŒT����[����������)
	while ((clock() - time_start) < thinking_time) {

		mtdf_result = mtdf(pos, root_static_value, f, depth);

		f = mtdf_result.value; // �����f���~�j�}�b�N�X�l�ɂ���
		depth += 2;
	}
	std::cout << "depth: " << depth << std::endl;
	std::cout << "thinking_time: " << clock() - time_start << "ms" << std::endl;
	return mtdf_result;
}

// ------------------------------------
//			  MTD(f)�֐�
// ------------------------------------

// node			: �T���J�n�m�[�h
// static_value : �T���J�n�m�[�h�̐ÓI�]���l
// f			: �~�j�}�b�N�X�l�����ς������l
// depth		: �T���̐[��
SearchResult mtdf(Position node, int static_value, int f, int depth) {

	SearchResult g;		// NullWindowSearch�̌��ʂ��i�[����
	int upper = +INF;	// g.value�̍ő�l
	int lower = -INF;	// g.value�̍ŏ��l
	int alpha;			// �� = �� = alpha �Ƃ��ă��������s����(NullWindowSearch)

	g.value = f;		// f�͉��炩�̕��@�Ń~�j�}�b�N�X�l�����ς������l

	while (lower < upper) {
		if (g.value == lower) alpha = g.value + 1; // �O��T���J�n�m�[�h�ŃJ�b�g����Ȃ�����
		else alpha = g.value;

		// �u���\�t�������@�� NullWindowSearch ���s��
		g = null_window(node, static_value, alpha, 1, depth);

		if (g.value == alpha) lower = g.value;		// �Ő�m�[�h�ŃJ�b�g����Ă��Ȃ�
		else if (g.value > alpha) upper = g.value;	// �J�b�g���ꂽ
	}
	return g;
}

// ------------------------------------
//	  Null-Window_Search(�u���\�g�p)
// ------------------------------------

// nega�����̒T�����͈̔͂�0�ɂ���
// remain_depth������0������ƃG���[

// �őP�肪�m�肵���Ƃ��́A
// "/0017T/2117T/0327S"�̂悤�ɁA��̎肩��L�q���Ă���

// node			: ���݂̃m�[�h
// static_value : ���m�[�h�̐ÓI�]���l�i�]���֐������̂܂܎g�p�����]���l�j
// alpha		: �T����(beta�����˂�)
// depth		: �T���J�n�m�[�h����̐[��
// remain_depth	: �c��̒T���̐[��

SearchResult null_window(Position node, int static_value, int alpha, unsigned __int8 depth, unsigned __int8 remain_depth) {

	SearchResult lower; // ���̃m�[�h�̕]���l�̉����l+���̎��̎w����
	bool already_in_table = hash_table.find(node.hash) != hash_table.end(); // �u���\�Ɋ܂܂�Ă��邩

	// �u���\�ɂ��̋ǖʂ̃G���g�����܂܂�Ă���Η��p����
	if (already_in_table) {

		HashEntry reference = hash_table[node.hash]; // �u���\�����̃f�[�^

		// �u���\�f�[�^�̎c��depth�����Ȃ��Ƃ��͎g��Ȃ�
		if (reference.remain_depth >= remain_depth) {

			// ���ۂ̕]���l���L�^����Ă���Ƃ��́A�����Ԃ�
			if (reference.move_follow != "") { return search_result(reference.value, reference.move_follow); }

			// �]���l�̉����l���L�^����Ă���Ƃ��́A���̂܂ܗ��p����
			else { lower = search_result(reference.value); }
		}
	}

	std::vector<Move> moves = node.gen_shoot(); // �ˌ���̐���

	if (moves.empty()) moves = node.gen_travel(); // �ˌ��肪�Ȃ���Έړ���̐���

	// moves�S�Ăɕ]���֐���K�p�����]���l�z��values���쐬
	std::vector<int> values = parallel_eval(node, moves);

	// values���珬�������ɗv�f�̓Y�����擾���A�����z��Ƃ���
	std::vector<unsigned __int8> order = index_sort(values);

	// �]���l�ő�̃m�[�h�����̂܂ܕԂ�
	if (remain_depth == 1) {

		SearchResult result = search_result(values[order[0]], Move_to_string(moves[order[0]]));

		// �u���\�ɋL�^���Ă���
		table_new(node.hash, 1, result.value, result.move_follow);

		return result;
	}

	// moves�̒��g��order���Ɍ��Ă���
	for (int i = 0; i != moves.size(); ++i) {

		SearchResult null_window_result = null_window(node.moved[moves[order[0]]], values[order[0]], -alpha, depth + 1, remain_depth - 1);

		if (-null_window_result.value >= lower.value) {
			lower = search_result(-null_window_result.value);

			if (lower.value >= alpha) {

				table_new(node.hash, remain_depth, lower.value); // lower.value �������l�Ƃ��Ēu���\�ɓo�^

				if (depth == 1) return search_result(lower.value); // �ł��󂢃m�[�h�̏ꍇ�Aalpha�𒴂����l��Ԃ�
				else return search_result(INF);					 // �����łȂ���Βʏ�̃J�b�g
			}
		}
	}

	// �J�b�g���ꂸ��(lower <= alpha �̂܂�)�Ō�̃m�[�h�܂ŏI��

	// lower��^�̕]���l�Ƃ��Ēu���\�ɓo�^
	table_new(node.hash, remain_depth, lower.value, lower.move_follow);

	return lower;
}

// --------------------------------------
//			�u���\�ǉ�/�X�V�֐�
// --------------------------------------

void table_new(unsigned __int64 hash, __int8 remain_depth, int value, std::string move_follow = "") {

	HashEntry new_hash_entry;
	new_hash_entry.value = value;
	new_hash_entry.move_follow = move_follow;
	new_hash_entry.remain_depth = remain_depth;

	hash_table[hash] = new_hash_entry; // �V�����n�b�V���G���g���̒ǉ�/�X�V
}

// --------------------------------------
//		  �]���֐����񏈗��֐�
// --------------------------------------

// �ǖʂƎw����̔z����󂯎��A�����̕]���l����񏈗��ł����߁A�z��ŕԂ�
std::vector<int> parallel_eval(Position pos, std::vector<Move> moves) {
	// TODO
}

// --------------------------------------
//		�z��C���f�b�N�X�\�[�g�֐�
// --------------------------------------

// int vector���󂯎��A�v�f�̏����ɃC���f�b�N�X����ׁAvector�ɂ��� TODO
std::vector<unsigned __int8> index_sort(std::vector<int> v) {

	// TODO

}

// --------------------------------------
//	NullWindowSearch���ʍ\���� �����֐�
// --------------------------------------

SearchResult search_result(int value, std::string move_follow = "") {

	SearchResult tmp;
	tmp.value = value;
	tmp.move_follow = move_follow;

	return tmp;
}