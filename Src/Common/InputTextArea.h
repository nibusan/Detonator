#pragma once
#include <string>
#include "Vector2.h"
#include "../Manager/InputTextManager.h"

// マウスで矩形を左クリックしたら入力を受け付けるクラス
// 汎用性なし。仮作成。
class InputTextArea
{

public:

	const unsigned int COLOR_BACK_DEFAULT = 0x000000;
	const unsigned int COLOR_BACK_ACTIVE = 0x2f4f4f;
	const unsigned int COLOR_CHAR = 0xffffff;
	const unsigned int COLOR_FRAME = 0xffffff;

	InputTextArea(Vector2<int> pos, Vector2<int> size, int length);
	~InputTextArea(void);

	// 更新ステップ
	void Update(void);

	// 描画
	void Draw(void);

	// 解放処理
	void Release(void);

	// キー入力ハンドルを取得する
	int GetKeyHandle(void);

	// テキストをセット
	std::string GetText(void);
	void SetText(std::string text);

	// 入力内容をセット
	void SetKeyInputStringBuffer(void);

private:

	InputTextManager& manager_;

	// キー入力ハンドル
	int keyInputHandle_;

	int mouseInputOld_;
	int mouseInputNew_;

	// 矩形の左上座標
	Vector2<int> pos_;

	// 矩形の大きさ
	Vector2<int> size_;

	// 文字数
	int length_;

	// 入力バッファ
	char buffer_[256];

	// 入力テキスト
	std::string text_;

	// 描画
	void DrawDefault(void);
	void DrawActive(void);

};
