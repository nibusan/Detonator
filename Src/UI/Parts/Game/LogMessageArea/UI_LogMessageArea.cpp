#include "../../../../Manager/SceneManager.h"
#include "../../../../Manager/InputManager.h"
#include "../../../../Manager/GameManager.h"
#include "../../../../Manager/ResourceManager.h"
#include "UI_LogMessageArea.h"

const std::string UI_LogMessageArea::EMPTY_MESSAGE = "empty";

UI_LogMessageArea::UI_LogMessageArea(
	const std::string& fontName, 
	int maxLineCount, 
	float enterTime,
	float exitTime, 
	float waitTime, 
	const std::function<std::shared_ptr<MESSAGE_DATA_BASE>(void)>& getLogDataFunction) :
maxLineCount_(maxLineCount),
enterTime_(enterTime),
exitTime_(exitTime),
waitTime_(waitTime),
getLogDataFunction_(getLogDataFunction) {
	usingFont_ = std::make_shared<Font>(fontName, 24);
	usingFont_->SetIsAutoDeleteHandle(true);
}

void UI_LogMessageArea::Init_UI(void) {
	auto& resourceManager = ResourceManager::GetInstance();
	icon_KillLog_ = std::dynamic_pointer_cast<Graphic>(resourceManager.GetResourceFile("Image_Icon_Game_Kill_Log"));

	messageList_.resize(maxLineCount_, std::make_shared<MESSAGE_DATA_BASE>());
}

bool UI_LogMessageArea::Update_UI(void) {
	auto& sceneManager = SceneManager::GetInstance();
	auto& inputManager = InputManager::GetInstance();

	// ���O�\���e�X�g�p
	/*if (inputManager.IsTrgDown(KEY_INPUT_0)) {
		std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE> messageDataBase
			= std::make_shared<UI_LogMessageArea::MESSAGE_DATA_NORMAL>("AIUEOAIUEO", 0x00FF00);
		AddMessage(messageDataBase);
	}*/

	// �V�K�̃��O���擾����
	auto logMessage = getLogDataFunction_();

	// ����nullptr����Ȃ��������ނɂ���ď�����ς���
	if (logMessage != nullptr) {
		if (logMessage->type != LOG_MESSAGE_TYPE::NONE) {
			switch (logMessage->type) {
			case UI_LogMessageArea::LOG_MESSAGE_TYPE::NORMAL: {
				auto normalLog = std::dynamic_pointer_cast<MESSAGE_DATA_NORMAL>(logMessage);
				if (normalLog->data.message_ != EMPTY_MESSAGE) {
					AddMessage(logMessage);
				}
			}
				break;

			case UI_LogMessageArea::LOG_MESSAGE_TYPE::KILL: {
				auto killLog = std::dynamic_pointer_cast<MESSAGE_DATA_KILL>(logMessage);
				if (killLog->by_.message_ != EMPTY_MESSAGE && killLog->killed_.message_ != EMPTY_MESSAGE) {
					AddMessage(logMessage);
				}
			}
				break;

			case UI_LogMessageArea::LOG_MESSAGE_TYPE::NONE:
			default:
				break;
			}
		}
	}

	for (auto& messageData : messageList_) {
		if (messageData == nullptr) continue;
		if (messageData->type == LOG_MESSAGE_TYPE::NONE) continue;
		switch (messageData->type) {
		case UI_LogMessageArea::LOG_MESSAGE_TYPE::NORMAL: {
			auto normalLog = std::dynamic_pointer_cast<MESSAGE_DATA_NORMAL>(messageData);
			if (normalLog->data.message_ == EMPTY_MESSAGE) continue;

		}
			break;

		case UI_LogMessageArea::LOG_MESSAGE_TYPE::KILL: {
			auto killLog = std::dynamic_pointer_cast<MESSAGE_DATA_KILL>(messageData);
			//if (killLog->by_.message_ != EMPTY_MESSAGE && killLog->killed_.message_ != EMPTY_MESSAGE) continue;
		}
			break;

		case UI_LogMessageArea::LOG_MESSAGE_TYPE::NONE:
		default:
			break;
		}

		// �\�����Ԃ̍X�V
		if (messageData->enterTime <= enterTime_) {
			messageData->enterTime += sceneManager.GetDeltaTime();
			continue;
		}
		else if (messageData->waitTime <= waitTime_) {
			messageData->waitTime += sceneManager.GetDeltaTime();
			continue;
		}
		else if (messageData->exitTime <= exitTime_) {
			messageData->exitTime += sceneManager.GetDeltaTime();
			continue;
		}
		messageData->Init();
	}

	// �\�����Ԃ��Z�����Ƀ\�[�g����
	std::sort(
		messageList_.begin(),
		messageList_.end(), 
		[](std::shared_ptr<MESSAGE_DATA_BASE>& a1, std::shared_ptr<MESSAGE_DATA_BASE>& a2)->bool {
			return (a1->enterTime + a1->waitTime + a1->exitTime) < (a2->enterTime + a2->waitTime + a2->exitTime);
		}
	);

	// ������������UI�̓N���b�N�ł��Ȃ��̂�false��Ԃ�
    return false;
}

void UI_LogMessageArea::Draw_UI(void) {
	auto& sceneManager = SceneManager::GetInstance();
	auto size = GetSize();
	for (int i = 0; i < messageList_.size(); i++) {
		if (messageList_[i]->type == UI_LogMessageArea::LOG_MESSAGE_TYPE::NONE) continue;
		// ���O��`�悷����W
		Vector2<int> messageDrawPos = {};

		// ���O�̓����x
		int messageAlpha = 0;

		// ���O�̕\�����@(����A�ҋ@�A�ޏ�)�ɂ���č��W�̌v�Z�̎d����ς���
		if (messageList_[i]->enterTime <= enterTime_) {
			messageDrawPos.x = cosf((1.0f - (messageList_[i]->enterTime / enterTime_)) * DX_PI_F) * START_POS_X;
			messageAlpha = static_cast<int>(messageList_[i]->enterTime / enterTime_ * 255);
		}
		else if (messageList_[i]->waitTime <= waitTime_) {
			messageDrawPos.x = START_POS_X;
			messageAlpha = 255;
		}
		else if (messageList_[i]->exitTime <= exitTime_) {
			messageDrawPos.x = cosf(((messageList_[i]->exitTime / exitTime_)) * DX_PI_F) * START_POS_X;
			messageAlpha = static_cast<int>((1.0f - messageList_[i]->exitTime / exitTime_) * 255);
		}

		//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 50);
		//DrawBox(0, 32 * (messageList_.size() - 1 - i), size.x, 32 * (messageList_.size() - 1 - i +1), 0x888888, true);
		//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, messageAlpha);
		if (messageList_[i]->type == LOG_MESSAGE_TYPE::NORMAL) {
			// �m�[�}�����O�̕`�揈��

			auto normalLog = std::dynamic_pointer_cast<MESSAGE_DATA_NORMAL>(messageList_[i]);
			DrawStringToHandle(
				messageDrawPos.x,
				32 * (messageList_.size() - 1 - i),
				normalLog->data.message_.c_str(),
				normalLog->data.color_,
				usingFont_->GetHandle()
			);
		}
		else if (messageList_[i]->type == UI_LogMessageArea::LOG_MESSAGE_TYPE::KILL) {
			// �L�����O�̕`�揈��

			auto killLog = std::dynamic_pointer_cast<MESSAGE_DATA_KILL>(messageList_[i]);
			DrawStringToHandle(
				messageDrawPos.x,
				32 * (messageList_.size() - 1 - i),
				killLog->killed_.message_.c_str(),
				killLog->killed_.color_,
				usingFont_->GetHandle()
			);

			auto width = GetDrawStringWidthToHandle(killLog->killed_.message_.c_str(), killLog->killed_.message_.length(), usingFont_->GetHandle());

			icon_KillLog_.lock()->Draw(
				{ static_cast<float>(width + 42), static_cast<float>(32 * (messageList_.size() - 1 - i)) },
				false,
				nullptr
			);

			// �L�����ꂽ���̃��b�Z�[�W��`�悷�邽�߂̃I�t�Z�b�g
			Vector2<int> offset = { 48, 32 };
			DrawStringToHandle(
				width + offset.x + messageDrawPos.x,
				offset.y * (messageList_.size() - 1 - i),
				killLog->by_.message_.c_str(),
				killLog->by_.color_,
				usingFont_->GetHandle()
			);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void UI_LogMessageArea::Release_UI(void) {

}

void UI_LogMessageArea::AddMessage(const std::shared_ptr<MESSAGE_DATA_BASE>& logData) {
	// ���O��ǉ�����
	for (int i = 0; i < messageList_.size(); i++) {
		if (messageList_[i]->type == LOG_MESSAGE_TYPE::NONE) {
			messageList_[i] = logData;
			return;
		}
	}
}