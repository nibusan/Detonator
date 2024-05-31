#pragma once
#include <string>
#include <utility>
#include "../../Common/Vector2.h"
#include "../../Common/StaticSingleton.h"
#include "../../Common/nlohmann-json/json.hpp"
#include "../../UI/UIBase.h"
#include "../../UI/Parts/Common/InputTextArea/UI_InputTextArea.h"
#include "../../UI/Parts/Common/ModelPreview/UI_ModelPreview.h"
#include "../../UI/Parts/Common/Cursor/UI_Cursor.h"

class UILayerList : public StaticSingleton<UILayerList>
{
public:
	// UIレイヤーの種類
	enum class TYPE {
		TITLE = 0,
		SELECT_MENU,
		INPUT_IP_ADDRESS,
		SLOBBY,
		SGAME,
		SRESULT,
		MLOBBY,
		MGAME,
		MRESULT,
		OPTION,
		SHUTDOWN,
		SGAME_TIME_START,
		SGAME_SCORE_START,
		MGAME_ART,
		PAUSE_MENU,
		INPUT_NAME,
		MGAME_START,
		MAX,
	};

	// UIのデータ
	struct DEFAULT_UI_DATA {
		// UIのID
		int id;

		// UIの種類
		UI::TYPE uiType;

		// 座標
		Vector2<float> pos;

		// サイズ
		Vector2<int> size;

		// 固定されたサイズか
		bool isFixedSize;

		// スケール値
		float scale;

		// 角度
		float angle;

		// 透明度
		float alpha;

		// 初期状態をアクティブにするか
		bool isActive;

		// クリックできるか
		bool isClickable;

		// どこを基準に描画するか
		UIBase::DRAW_POSITION_TYPE positionType;

		// クリックされた時に呼び出される関数のID
		int onClickFuncID;

		// 子UIのデータ
		std::vector<std::shared_ptr<DEFAULT_UI_DATA>> childs;

		DEFAULT_UI_DATA(void) : 
		uiType(UI::TYPE::MAX), 
		pos({}), 
		positionType(UIBase::DRAW_POSITION_TYPE::UP_LEFT), 
		alpha(1.0f),
		scale(1.0f),
		angle(0.0f),
		onClickFuncID(-1) {};

		virtual ~DEFAULT_UI_DATA(void) = default;
	};

	struct IMAGE_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するグラフィックのリソースキー
		std::string image_ResourceKey;

		// 使用するピクセルシェーダーのリソースキー
		std::string pixelShader_ResourceKey;

		IMAGE_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader") {};

		~IMAGE_UI_DATA(void) override = default;
	};

	struct TEXT_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するメッセージ
		std::string message;

		// 使用するフォント
		std::string font_ResourceKey;

		// フォントのサイズ
		int fontSize;

		// フォントの色
		unsigned int fontColor;

		TEXT_UI_DATA(void) : message("empty_Message") {};

		~TEXT_UI_DATA(void) override = default;
	};
	
	struct BUTTON_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するグラフィックのリソースキー
		std::string image_ResourceKey;

		// 使用するピクセルシェーダーのリソースキー
		std::string pixelShader_ResourceKey;

		// 使用するメッセージ
		std::string message;

		// 使用するフォント
		std::string font_ResourceKey;

		// フォントのサイズ
		int fontSize;

		// フォントの色
		unsigned int fontColor;

		// 使用するメッセージ
		std::string message2;

		// 使用するフォント
		std::string font_ResourceKey2;

		// フォントのサイズ
		int fontSize2;

		// フォントの色
		unsigned int fontColor2;

		BUTTON_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader"),
			message("empty_Message") {};

		~BUTTON_UI_DATA(void) override = default;
	};

	struct WINDOW_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するグラフィックのリソースキー
		std::string image_ResourceKey;

		// 使用するピクセルシェーダーのリソースキー
		std::string pixelShader_ResourceKey;

		// 使用するメッセージ
		std::string message;

		// 使用するフォント
		std::string font_ResourceKey;

		// フォントのサイズ
		int fontSize;

		// フォントの色
		unsigned int fontColor;

		// 使用するメッセージ
		std::string message2;

		// 使用するフォント
		std::string font_ResourceKey2;

		// フォントのサイズ
		int fontSize2;

		// フォントの色
		unsigned int fontColor2;

		// 付属しているボタン
		std::vector<std::shared_ptr<DEFAULT_UI_DATA>> buttons;

		WINDOW_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader"),
			message("empty_Message") {};

		~WINDOW_UI_DATA(void) override = default;
	};

	struct INPUT_TEXT_AREA_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するグラフィックのリソースキー
		std::string image_ResourceKey;

		// 使用するピクセルシェーダーのリソースキー
		std::string pixelShader_ResourceKey;

		// 使用するフォント
		std::string font_ResourceKey;

		// フォントのサイズ
		int fontSize;

		// フォントの色
		unsigned int fontColor;

		// 入力されるテキストの種類
		UI_InputTextArea::INPUT_TYPE inputType;

		// 入力を受け付けるテキストの種類
		UI_InputTextArea::INPUT_TEXT_TYPE inputTextType;

		// 受け付けるテキストの最大文字数
		int maxLength;

		// 入力が終わった際に呼び出される関数のID
		int endInputFuncID;

		INPUT_TEXT_AREA_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader")
			{};

		~INPUT_TEXT_AREA_UI_DATA(void) override = default;
	};

	struct COLOR_BOX_UI_DATA : public DEFAULT_UI_DATA {
		//	ボックスの色
		unsigned int color;

		COLOR_BOX_UI_DATA(void){};

		~COLOR_BOX_UI_DATA(void) override = default;
	};

	struct MODEL_PREVIEW_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するグラフィックのリソースキー
		std::string image_ResourceKey;

		// 使用するマスクのリソースキー
		std::string imageMask_ResourceKey;

		// 使用するピクセルシェーダーのリソースキー
		std::string pixelShader_ResourceKey;

		// 表示するオブジェクトのID
		std::vector<UI_ModelPreview::OBJECT_TYPE> objectIDs;

		MODEL_PREVIEW_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			imageMask_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader")
		{};

		~MODEL_PREVIEW_UI_DATA(void) override = default;
	};

	struct CURSOR_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するグラフィックのリソースキー
		std::string image_ResourceKey;

		// 使用するピクセルシェーダーのリソースキー
		std::string pixelShader_ResourceKey;

		// カーソルの方向
		UI_Cursor::DIR dir;

		CURSOR_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader")
			{};

		~CURSOR_UI_DATA(void) override = default;
	};

	struct MESSAGE_BOX_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するグラフィックのリソースキー
		std::string image_ResourceKey;

		// 使用するデフォルトのメッセージ
		std::string defMessage;

		// 使用するフォント
		std::string font_ResourceKey;

		// フォントのサイズ
		int fontSize;

		// フォントの色
		unsigned int fontColor;

		// 追加するテキストを取得する関数のID
		int getAddTextFuncID;

		MESSAGE_BOX_UI_DATA(void) : 
			image_ResourceKey("empty_Image") {};

		~MESSAGE_BOX_UI_DATA(void) override = default;
	};

	struct WEAPON_INFO_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するグラフィックのリソースキー
		std::string image_ResourceKey;

		// 使用するグレネードのアイコンのリソースキー
		std::vector<std::string> image_Grenade_ResourceKeys;

		// 使用する銃のアイコンのリソースキー
		std::vector<std::string> image_Gun_ResourceKeys;

		// 弾の数を描画するフォントのリソースキー
		std::string font_Magazine_ResourceKey;

		// 銃の名前を描画するフォントのリソースキー
		std::string font_GunName_ResourceKey;

		WEAPON_INFO_UI_DATA(void) : 
			image_ResourceKey("empty_Image") {};

		~WEAPON_INFO_UI_DATA(void) override = default;
	};

	struct LOG_MESSAGE_AREA_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するフォント
		std::string font_ResourceKey;

		// 表示できる行の最大数
		int maxLineCount;

		// メッセージが登場するまでの時間
		float enterTime;

		// メッセージが退場するまでの時間
		float exitTime;

		// メッセージを表示する時間
		float waitTime;

		// ログメッセージを取得する関数のID
		int getTextFuncID;

		LOG_MESSAGE_AREA_UI_DATA(void) {};

		~LOG_MESSAGE_AREA_UI_DATA(void) override = default;
	};

	struct RESULT_TEXT_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するデフォルトのメッセージ
		std::string defMessage;

		// 使用するフォント
		std::string font_ResourceKey;

		// フォントのサイズ
		int fontSize;

		// フォントの色
		unsigned int fontColor;

		// 追加するテキストを取得する関数のID
		int getAddTextFuncID;

		RESULT_TEXT_UI_DATA(void) {};

		~RESULT_TEXT_UI_DATA(void) override = default;
	};

	struct TIME_UI_DATA : public DEFAULT_UI_DATA {
		// 使用するフォント
		std::string font_ResourceKey;

		// フォントのサイズ
		int fontSize;

		// フォントの色
		unsigned int fontColor;

		TIME_UI_DATA(void) {};

		~TIME_UI_DATA(void) override = default;
	};

private:

	// UIレイヤーのリスト
	std::unordered_map<UILayerList::TYPE, std::unordered_map<int, std::shared_ptr<DEFAULT_UI_DATA>>> uiLayerList_;

	UILayerList(void) = default;
	void Load(void);
	std::shared_ptr<DEFAULT_UI_DATA> CreateUIData(nlohmann::json jsonUIData);
public:
	~UILayerList(void) = default;
	THIS_CLASS_IS_STATIC_SINGLETON(UILayerList);

	void Init(void);
	void Release(void);

	const std::unordered_map<int, std::shared_ptr<DEFAULT_UI_DATA>>& GetUILayerData(UILayerList::TYPE type) const;
};

