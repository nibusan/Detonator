#pragma once
#include <vector>
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"

class UI_LogMessageArea : public UIBase {
public:

	// ���O�̎��
	enum class LOG_MESSAGE_TYPE {
		NONE = -1,
		NORMAL,		// �m�[�}�����O
		KILL		// �L�����O
	};

	// ���O���b�Z�[�W�f�[�^�̃x�[�X
	struct MESSAGE_DATA_BASE {
		// ����ɂ����鎞��
		float enterTime;

		// �ޏ�ɂ����鎞��
		float exitTime;

		// �~�܂��ĕ\�����鎞��
		float waitTime;

		// ���O�̎��
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

	// ���b�Z�[�W�̃f�[�^
	struct LOG_MESSAGE_DATA {
		// ���O�̃��b�Z�[�W
		std::string message_;

		// ���O�̐F
		unsigned int color_;

		LOG_MESSAGE_DATA(const std::string& message, unsigned int color) :
		message_(message),
		color_(color) {};
	};

	// �m�[�}�����O
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

	// �L�����O
	struct MESSAGE_DATA_KILL : public MESSAGE_DATA_BASE {
		// �L�������E���ꂽ����̃��O�f�[�^
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
	// ���O���b�Z�[�W��X���̃X�^�[�g�n�_
	static constexpr float START_POS_X = 30.0f;

	// ���O���b�Z�[�W�������ĂȂ��Ƃ��Ɋi�[���镶����
	static const std::string EMPTY_MESSAGE;

	// �g�p����t�H���g
	Shared_Font usingFont_;

	// �L�����O�Ŏg�p����A�C�R��
	Weak_Graphic icon_KillLog_;

	// �\���ł���s�̍ő吔
	const int maxLineCount_;

	// ���b�Z�[�W���o�ꂷ��܂ł̎���
	const float enterTime_;

	// ���b�Z�[�W���ޏꂷ��܂ł̎���
	const float exitTime_;

	// ���b�Z�[�W��\�����鎞��
	const float waitTime_;

	// ���b�Z�[�W���X�g
	std::vector<std::shared_ptr<MESSAGE_DATA_BASE>> messageList_;

	// ���O���b�Z�[�W���擾����֐�
	std::function<std::shared_ptr<MESSAGE_DATA_BASE>(void)> getLogDataFunction_;

	void AddMessage(const std::shared_ptr<MESSAGE_DATA_BASE>& logData);
};
