#pragma once
#include <vector>
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"

class UI_LogMessageArea : public UIBase {
public:

	// ログの種類
	enum class LOG_MESSAGE_TYPE {
		NONE = -1,
		NORMAL,		// ノーマルログ
		KILL		// キルログ
	};

	// ログメッセージデータのベース
	struct MESSAGE_DATA_BASE {
		// 入場にかかる時間
		float enterTime;

		// 退場にかかる時間
		float exitTime;

		// 止まって表示する時間
		float waitTime;

		// ログの種類
		LOG_MESSAGE_TYPE type;

		MESSAGE_DATA_BASE(void) :
		enterTime(0.0f),
		exitTime(0.0f),
		waitTime(0.0f),
		type(LOG_MESSAGE_TYPE::NONE) {};

		virtual ~MESSAGE_DATA_BASE(void) = default;

		void Init(void) {
			enterTime = 0.0f;
			exitTime = 0.0f;
			waitTime = 0.0f;
			type = LOG_MESSAGE_TYPE::NONE;
		}
	};

	// メッセージのデータ
	struct LOG_MESSAGE_DATA {
		// ログのメッセージ
		std::string message_;

		// ログの色
		unsigned int color_;

		LOG_MESSAGE_DATA(const std::string& message, unsigned int color) :
		message_(message),
		color_(color) {};
	};

	// ノーマルログ
	struct MESSAGE_DATA_NORMAL : public MESSAGE_DATA_BASE {
		LOG_MESSAGE_DATA data;

		MESSAGE_DATA_NORMAL(const std::string& message, unsigned int color) :
		MESSAGE_DATA_BASE(),
		data{ message, color } {
			type = LOG_MESSAGE_TYPE::NORMAL;
		};

		~MESSAGE_DATA_NORMAL(void) override = default;

		void Init(void) {
			MESSAGE_DATA_BASE::Init();
			data = { EMPTY_MESSAGE, 0x000000 };
		}
	};

	// キルログ
	struct MESSAGE_DATA_KILL : public MESSAGE_DATA_BASE {
		// キルした・された相手のログデータ
		LOG_MESSAGE_DATA by_;
		LOG_MESSAGE_DATA killed_;

		MESSAGE_DATA_KILL(const std::string& by, unsigned int by_color, const std::string& killed, unsigned int killed_color) :
			MESSAGE_DATA_BASE(),
			by_{ by, by_color },
			killed_{ killed, killed_color } {
			type = LOG_MESSAGE_TYPE::KILL;
		};

		~MESSAGE_DATA_KILL(void) override = default;

		void Init(void) {
			MESSAGE_DATA_BASE::Init();
			by_ = { EMPTY_MESSAGE, 0x000000 };
			killed_ = { EMPTY_MESSAGE, 0x000000 };
		}
	};

	UI_LogMessageArea(void) = default;
	UI_LogMessageArea(
		const std::string& fontName, 
		int maxLineCount, 
		float enterTime, 
		float exitTime, 
		float waitTime, 
		const std::function<std::shared_ptr<MESSAGE_DATA_BASE>(void)>& getLogDataFunction
	);
	~UI_LogMessageArea(void) = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;
private:
	// ログメッセージのX軸のスタート地点
	static constexpr float START_POS_X = 30.0f;

	// ログメッセージが入ってないときに格納する文字列
	static const std::string EMPTY_MESSAGE;

	// 使用するフォント
	Shared_Font usingFont_;

	// キルログで使用するアイコン
	Weak_Graphic icon_KillLog_;

	// 表示できる行の最大数
	const int maxLineCount_;

	// メッセージが登場するまでの時間
	const float enterTime_;

	// メッセージが退場するまでの時間
	const float exitTime_;

	// メッセージを表示する時間
	const float waitTime_;

	// メッセージリスト
	std::vector<std::shared_ptr<MESSAGE_DATA_BASE>> messageList_;

	// ログメッセージを取得する関数
	std::function<std::shared_ptr<MESSAGE_DATA_BASE>(void)> getLogDataFunction_;

	void AddMessage(const std::shared_ptr<MESSAGE_DATA_BASE>& logData);
};
