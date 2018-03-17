// uwi(Universal WINGS Interface)������
// ���C�����ł�����

#include "position.h"
#include "search.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>

using namespace std;

bool cui_mode;

// --------------------
//		������
// --------------------

/*
UWI�̏����𒆓r���[�ɏ����Ă���܂����A�܂��������Ă��܂���B
CUI���[�h���N�������悤�ɂȂ��Ă��܂��B
*/

/*
// GUI����̖���
enum GUI_ORDER {

	STOP // �T�����f

};

// ���߂��i�[���Ă���
// �m�F������A���̗v�f��delete����
vector<GUI_ORDER> orders;

// �T�����A���߂��󂯎��orders�Ɋi�[����
void usi(){

	string input; // ���͂��ꂽ������

	while (true) {

		cin >> input;

		// TODO

	}

}
*/
// �T�����𑗂�
void info(unsigned int nps,unsigned __int16 depth){

	if (cui_mode == true) return;
	
	// TODO
}

// �ǖʂ��R���\�[���ɕ`�悷��(CUI�p)
void draw(Position pos) {
	// TODO
	
}

// ������ō����CUI���[�h�ł�
// �R�[�h�������Ă��݂܂���
// UWI�֘A����������폜���܂��B
void cui() {	

	int integer;
	string str;
	bool first_is_com, second_is_com;
	unsigned __int16 thinking_time;
	Position pos;
	Move move;

	cout << "�΋ǂ̐ݒ����͂��Ă�������" << endl;
	cout << "0:���Ȃ������ 1:���Ȃ������ 2:�G���W�����m" << endl;
	cin >> integer;

	switch (integer) {
	case 0:
		first_is_com = false;
		second_is_com = true;
		break;
	case 1:
		first_is_com = true;
		second_is_com = false;
		break;
	case 2:
		first_is_com = true;
		second_is_com = true;
		break;
	}

	cout << "1��ɂ�����v�l���Ԃ���͂��Ă�������(ms)" << endl;
	cin >> thinking_time;

	while (true) {

		draw(pos);

		if (first_is_com) {

			Result result = search(pos, thinking_time);
			move = string_to_Move(result.move_follow);
			cout << "�]���l:" << result.value << endl;
		} 
		else {
			cout << "����1�����͂��Ă�������" << endl;
			cout << "��: 1Ad-2Ab  �ړ��Ȃ�u-�v�A�ˌ��Ȃ�u=�v���g���Ă�������" << endl;
			bool flag = false;
			do {
				if (flag) cout << "�w���肪�񍇖@��ł��B���͂������Ă�������" << endl;
				cin >> str;
				move = string_to_Move(str);
				flag = true;
			} while (!pos.legal(move));
		}

		if (move.to == 45 | move.to == 50) {
			cout << "���̏����ł��B\n\n" << endl;
			break;
		}

		pos.move(move);

		if (second_is_com) {

			Result result = search(pos.reversed(), thinking_time);
			move = reversed_move(string_to_Move(result.move_follow));
			cout << "�]���l:" << -result.value << endl;
		}
		else {
			cout << "����1�����͂��Ă�������" << endl;
			cout << "��: 1Ad-2Ab  �ړ��Ȃ�u-�v�A�ˌ��Ȃ�u=�v���g���Ă�������" << endl;
			bool flag = false;
			do {
				if (flag) cout << "�w���肪�񍇖@��ł��B���͂������Ă�������" << endl;
				cin >> str;
				move = string_to_Move(str);
				flag = true;

			} while (!pos.reversed.legal(reversed_move(move)));
		}
		if (move.to == 49 | move.to == 54) {
			cout << "���̏����ł��B\n\n" << endl;
			break;
		}

		pos.move(move);
	}
}

int main() {

	string input;

	//cin >> input;
	input = "cui";

	if (input == "uwi") {

		cout << "id name HaRTMaNN" << endl;
		cout << "option name hash_memory_size[MB] type spin default 100" << endl;
		cout << "uwiok" << endl;
	}
	else if (input == "cui") {
	
		cout << "CUI���[�h�����s���܂��B";

		cui();
	}

	else {
		cout << "GUI����̓��͂��Ԉ���Ă��܂�" << endl;
	}

	return 0;
}