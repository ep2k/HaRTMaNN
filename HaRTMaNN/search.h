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

// �Ӗ��̂Ȃ��w����^
Move NULL_MOVE;

// �]���֐� (evaluate.cpp�Ŏ���)

// parent_pos	: �e�ǖ�
// value		: �e�ÓI�]���l�i���[�g�m�[�h���͏ȗ��j
// move			: �w����i���[�g�m�[�h���͏ȗ��j
int evaluate(Position parent_pos, int parent_value, Move move);

// �T����(�����[��)
Result search(Position pos, __int16 thinking_time);

// MTD(f)�@
Result mtdf(Position node, int static_value, int f, int depth);

// Null-Window-Search(�u���\�g�p)
Result null_window(Position node, int static_value, int alpha, unsigned __int8 depth, unsigned __int8 remain_depth);

// �ǖʂƎw����̔z����󂯎��A�����̕]���l����񏈗��ł����߁A�z��ŕԂ�
std::vector<int> parallel_eval(Position pos, std::vector<Move> moves);

// int vector���󂯎��A�v�f�̏����ɃC���f�b�N�X����ׁAvector�ɂ���
std::vector<unsigned __int8> index_sort(std::vector<int> v);

// --------------------------------------
//           �T�����ʃN���X
// --------------------------------------

// ����͍Ő[�m�[�h�ŃC���X�^���X������
// ����ȏ��depth�ł́A�߂�l��move_anticipate�Ɏw�����ǉ��A
// �܂���exact_value��false�ɂ��ĕԂ�

class Result {

public:
	// �]���l or �]���l�̉����l
	int value;

	// value���u�]���l�v�ł���
	bool exact_value;

	// �ǂ݋�(exact_value��True���̂�)
	// �������玞�n�񏇂̎w����ƂȂ�
	// �Ֆʂ̌�����1�育�Ɣ��]���Ă���
	std::vector<Move>* move_anticipate;

	Result(unsigned __int8 v, Move bottom_move);
	~Result();
	void add_move(Move move); // �ǂ݋؂́u�����v�Ɏw�����ǉ�
	void not_exact(); // �}���莞�Amove_anticipate���폜
};

// --------------------------------------
//           �u���\�G���g��
// --------------------------------------

// �u���\�G���g���N���X
class HashEntry {
public:
	__int8 remain_depth;// �����T�����s�����ۂ̎c��[��
	int value;// MTD(f)�œ����ǖʂ̕]���l
	bool exact_value;// value �����ۂ̕]���l�ł���
	Move* bestmove; // �O��̍őP��(exact_value���̂�)

	~HashEntry();
	void set_bestmove(Move move);
	void not_exact();
};

// �u���\�ǉ��֐�
void table_new(unsigned __int64, __int8, int, std::string);

#endif // _SEARCH_H_