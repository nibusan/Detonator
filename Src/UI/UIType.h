#pragma once

namespace UI {
	enum class TYPE {
		IMAGE = 0,	// 画像
		TEXT,		// テキスト
		BUTTON,		// ボタン
		SLIDER,		// スライダー
		WINDOW,		// ウィンドウ
		INPUT_TEXT_AREA,	// テキスト入力用ボックス
		COLOR_BOX,	// 単色で塗りつぶされたボックス
		MODEL_PREVIEW,	// 3Dモデルを描画する枠
		CURSOR,		// カーソル
		MESSAGE_BOX,	// メッセージボックス
		WEAPON_INFO,	// 武器の情報
		LOG_MESSAGE_AREA,	// ログメッセージを表示するエリア
		RESULT_TEXT,	// リザルト画面でスコアなどを表示する用のテキスト
		GAME_TIME,	// ゲームの残り時間
		CHANGEABLE_TEXT,	// テキスト（変更可能）
		MAX
	};
}