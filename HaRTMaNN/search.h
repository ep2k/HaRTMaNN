// �T����

#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "position.h"
#include <vector>
#include <unordered_map>

// �u���\�̓O���[�o���ϐ��Ƃ��Ďg��
std::unordered_map<__int64, HashEntry> hash_table;

// INT_MAX(2,147,483,647)�𖳌���Ƃ��Ĉ���
static const float INF = INT_MAX;

// �]���֐� (evaluate.cpp�Ŏ���)

// parent_pos	: �e�ǖ�
// value		: �e�ÓI�]���l�i���[�g�m�[�h���͏ȗ��j
// move			: �w����i���[�g�m�[�h���͏ȗ��j
int evaluate(Position parent_pos, int parent_value, Move move);

// �T����(�����[��)
SearchResult search(Position pos, __int16 thinking_time);

// MTD(f)�@
SearchResult mtdf(Position node, int static_value, int f, int depth);

// Null-Window-Search(�u���\�g�p)
SearchResult null_window(Position node, int static_value, int alpha, unsigned __int8 depth, unsigned __int8 remain_depth);

// SearchResult�^����
SearchResult search_result(int value, std::string move_follow = "");

// �ǖʂƎw����̔z����󂯎��A�����̕]���l����񏈗��ł����߁A�z��ŕԂ�
std::vector<int> parallel_eval(Position pos, std::vector<Move> moves);

// int vector���󂯎��A�v�f�̏����ɃC���f�b�N�X����ׁAvector�ɂ���
std::vector<unsigned __int8> index_sort(std::vector<int> v);

// --------------------------------------
//		�T���֐� �߂�l�\����
// --------------------------------------

struct SearchResult {

	int value;
	std::string move_follow;
};

// --------------------------------------
//				�u���\
// --------------------------------------

// �u���\�G���g���\����
struct HashEntry {
	int value;				// MTD(f)�œ����ǖʂ̕]���l
	std::string move_follow;// ����ȍ~�ɑ����w����(""�Ȃ�΁Avalue�͉����l��\��)
	__int8 remain_depth;	// �����T�����s�����ۂ̎c��[��
};

// �u���\�ǉ��֐�
void table_new(unsigned __int64,__int8, int, std::string);

#endif // _SEARCH_H_