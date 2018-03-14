// uwi(Universal WINGS Interface)処理部
// メイン部でもある

#include "position.h"
#include "search.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>

using namespace std;

bool cui_mode;

// --------------------
//		未完成
// --------------------

/*
UWIの処理を中途半端に書いてありますが、まだ完成していません。
CUIモードが起動されるようになっています。
*/

/*
// GUIからの命令
enum GUI_ORDER {

	STOP // 探索中断

};

// 命令を格納しておく
// 確認したら、その要素をdeleteする
vector<GUI_ORDER> orders;

// 探索中、命令を受け取りordersに格納する
void usi(){

	string input; // 入力された文字列

	while (true) {

		cin >> input;

		// TODO

	}

}
*/
// 探索情報を送る
void info(unsigned int nps,unsigned __int16 depth){

	if (cui_mode == true) return;
	
	// TODO
}

// 局面をコンソールに描画する(CUI用)
void draw(Position pos) {
	// TODO
	
}

// やっつけで作ったCUIモードです
// コードが汚くてすみません
// UWI関連が整ったら削除します。
void cui() {	

	int integer;
	string str;
	bool first_is_com, second_is_com;
	unsigned __int16 thinking_time;
	Position pos;
	Move move;

	cout << "対局の設定を入力してください" << endl;
	cout << "0:あなたが先手 1:あなたが後手 2:エンジン同士" << endl;
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

	cout << "1手にかける思考時間を入力してください(ms)" << endl;
	cin >> thinking_time;

	while (true) {

		draw(pos);

		if (first_is_com) {

			Result result = search(pos, thinking_time);
			move = string_to_Move(result.move_follow);
			cout << "評価値:" << result.value << endl;
		} 
		else {
			cout << "次の1手を入力してください" << endl;
			cout << "例: 1Ad-2Ab  移動なら「-」、射撃なら「=」を使ってください" << endl;
			bool flag = false;
			do {
				if (flag) cout << "指し手が非合法手です。入力し直してください" << endl;
				cin >> str;
				move = string_to_Move(str);
				flag = true;
			} while (!pos.legal(move));
		}

		if (move.to == 45 | move.to == 50) {
			cout << "先手の勝利です。\n\n" << endl;
			break;
		}

		pos.move(move);

		if (second_is_com) {

			Result result = search(pos.reversed(), thinking_time);
			move = reversed_move(string_to_Move(result.move_follow));
			cout << "評価値:" << -result.value << endl;
		}
		else {
			cout << "次の1手を入力してください" << endl;
			cout << "例: 1Ad-2Ab  移動なら「-」、射撃なら「=」を使ってください" << endl;
			bool flag = false;
			do {
				if (flag) cout << "指し手が非合法手です。入力し直してください" << endl;
				cin >> str;
				move = string_to_Move(str);
				flag = true;

			} while (!pos.reversed.legal(reversed_move(move)));
		}
		if (move.to == 49 | move.to == 54) {
			cout << "後手の勝利です。\n\n" << endl;
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
	
		cout << "CUIモードを実行します。";

		cui();
	}

	else {
		cout << "GUIからの入力が間違っています" << endl;
	}

	return 0;
}