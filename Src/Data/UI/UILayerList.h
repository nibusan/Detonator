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
	// UI���C���[�̎��
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

	// UI�̃f�[�^
	struct DEFAULT_UI_DATA {
		// UI��ID
		int id;

		// UI�̎��
		UI::TYPE uiType;

		// ���W
		Vector2<float> pos;

		// �T�C�Y
		Vector2<int> size;

		// �Œ肳�ꂽ�T�C�Y��
		bool isFixedSize;

		// �X�P�[���l
		float scale;

		// �p�x
		float angle;

		// �����x
		float alpha;

		// ������Ԃ��A�N�e�B�u�ɂ��邩
		bool isActive;

		// �N���b�N�ł��邩
		bool isClickable;

		// �ǂ�����ɕ`�悷�邩
		UIBase::DRAW_POSITION_TYPE positionType;

		// �N���b�N���ꂽ���ɌĂяo�����֐���ID
		int onClickFuncID;

		// �qUI�̃f�[�^
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
		// �g�p����O���t�B�b�N�̃��\�[�X�L�[
		std::string image_ResourceKey;

		// �g�p����s�N�Z���V�F�[�_�[�̃��\�[�X�L�[
		std::string pixelShader_ResourceKey;

		IMAGE_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader") {};

		~IMAGE_UI_DATA(void) override = default;
	};

	struct TEXT_UI_DATA : public DEFAULT_UI_DATA {
		// �g�p���郁�b�Z�[�W
		std::string message;

		// �g�p����t�H���g
		std::string font_ResourceKey;

		// �t�H���g�̃T�C�Y
		int fontSize;

		// �t�H���g�̐F
		unsigned int fontColor;

		TEXT_UI_DATA(void) : message("empty_Message") {};

		~TEXT_UI_DATA(void) override = default;
	};
	
	struct BUTTON_UI_DATA : public DEFAULT_UI_DATA {
		// �g�p����O���t�B�b�N�̃��\�[�X�L�[
		std::string image_ResourceKey;

		// �g�p����s�N�Z���V�F�[�_�[�̃��\�[�X�L�[
		std::string pixelShader_ResourceKey;

		// �g�p���郁�b�Z�[�W
		std::string message;

		// �g�p����t�H���g
		std::string font_ResourceKey;

		// �t�H���g�̃T�C�Y
		int fontSize;

		// �t�H���g�̐F
		unsigned int fontColor;

		// �g�p���郁�b�Z�[�W
		std::string message2;

		// �g�p����t�H���g
		std::string font_ResourceKey2;

		// �t�H���g�̃T�C�Y
		int fontSize2;

		// �t�H���g�̐F
		unsigned int fontColor2;

		BUTTON_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader"),
			message("empty_Message") {};

		~BUTTON_UI_DATA(void) override = default;
	};

	struct WINDOW_UI_DATA : public DEFAULT_UI_DATA {
		// �g�p����O���t�B�b�N�̃��\�[�X�L�[
		std::string image_ResourceKey;

		// �g�p����s�N�Z���V�F�[�_�[�̃��\�[�X�L�[
		std::string pixelShader_ResourceKey;

		// �g�p���郁�b�Z�[�W
		std::string message;

		// �g�p����t�H���g
		std::string font_ResourceKey;

		// �t�H���g�̃T�C�Y
		int fontSize;

		// �t�H���g�̐F
		unsigned int fontColor;

		// �g�p���郁�b�Z�[�W
		std::string message2;

		// �g�p����t�H���g
		std::string font_ResourceKey2;

		// �t�H���g�̃T�C�Y
		int fontSize2;

		// �t�H���g�̐F
		unsigned int fontColor2;

		// �t�����Ă���{�^��
		std::vector<std::shared_ptr<DEFAULT_UI_DATA>> buttons;

		WINDOW_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader"),
			message("empty_Message") {};

		~WINDOW_UI_DATA(void) override = default;
	};

	struct INPUT_TEXT_AREA_UI_DATA : public DEFAULT_UI_DATA {
		// �g�p����O���t�B�b�N�̃��\�[�X�L�[
		std::string image_ResourceKey;

		// �g�p����s�N�Z���V�F�[�_�[�̃��\�[�X�L�[
		std::string pixelShader_ResourceKey;

		// �g�p����t�H���g
		std::string font_ResourceKey;

		// �t�H���g�̃T�C�Y
		int fontSize;

		// �t�H���g�̐F
		unsigned int fontColor;

		// ���͂����e�L�X�g�̎��
		UI_InputTextArea::INPUT_TYPE inputType;

		// ���͂��󂯕t����e�L�X�g�̎��
		UI_InputTextArea::INPUT_TEXT_TYPE inputTextType;

		// �󂯕t����e�L�X�g�̍ő啶����
		int maxLength;

		// ���͂��I������ۂɌĂяo�����֐���ID
		int endInputFuncID;

		INPUT_TEXT_AREA_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader")
			{};

		~INPUT_TEXT_AREA_UI_DATA(void) override = default;
	};

	struct COLOR_BOX_UI_DATA : public DEFAULT_UI_DATA {
		//	�{�b�N�X�̐F
		unsigned int color;

		COLOR_BOX_UI_DATA(void){};

		~COLOR_BOX_UI_DATA(void) override = default;
	};

	struct MODEL_PREVIEW_UI_DATA : public DEFAULT_UI_DATA {
		// �g�p����O���t�B�b�N�̃��\�[�X�L�[
		std::string image_ResourceKey;

		// �g�p����}�X�N�̃��\�[�X�L�[
		std::string imageMask_ResourceKey;

		// �g�p����s�N�Z���V�F�[�_�[�̃��\�[�X�L�[
		std::string pixelShader_ResourceKey;

		// �\������I�u�W�F�N�g��ID
		std::vector<UI_ModelPreview::OBJECT_TYPE> objectIDs;

		MODEL_PREVIEW_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			imageMask_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader")
		{};

		~MODEL_PREVIEW_UI_DATA(void) override = default;
	};

	struct CURSOR_UI_DATA : public DEFAULT_UI_DATA {
		// �g�p����O���t�B�b�N�̃��\�[�X�L�[
		std::string image_ResourceKey;

		// �g�p����s�N�Z���V�F�[�_�[�̃��\�[�X�L�[
		std::string pixelShader_ResourceKey;

		// �J�[�\���̕���
		UI_Cursor::DIR dir;

		CURSOR_UI_DATA(void) : 
			image_ResourceKey("empty_Image"),
			pixelShader_ResourceKey("empty_PixelShader")
			{};

		~CURSOR_UI_DATA(void) override = default;
	};

	struct MESSAGE_BOX_UI_DATA : public DEFAULT_UI_DATA {
		// �g�p����O���t�B�b�N�̃��\�[�X�L�[
		std::string image_ResourceKey;

		// �g�p����f�t�H���g�̃��b�Z�[�W
		std::string defMessage;

		// �g�p����t�H���g
		std::string font_ResourceKey;

		// �t�H���g�̃T�C�Y
		int fontSize;

		// �t�H���g�̐F
		unsigned int fontColor;

		// �ǉ�����e�L�X�g���擾����֐���ID
		int getAddTextFuncID;

		MESSAGE_BOX_UI_DATA(void) : 
			image_ResourceKey("empty_Image") {};

		~MESSAGE_BOX_UI_DATA(void) override = default;
	};

	struct WEAPON_INFO_UI_DATA : public DEFAULT_UI_DATA {
		// �g�p����O���t�B�b�N�̃��\�[�X�L�[
		std::string image_ResourceKey;

		// �g�p����O���l�[�h�̃A�C�R���̃��\�[�X�L�[
		std::vector<std::string> image_Grenade_ResourceKeys;

		// �g�p����e�̃A�C�R���̃��\�[�X�L�[
		std::vector<std::string> image_Gun_ResourceKeys;

		// �e�̐���`�悷��t�H���g�̃��\�[�X�L�[
		std::string font_Magazine_ResourceKey;

		// �e�̖��O��`�悷��t�H���g�̃��\�[�X�L�[
		std::string font_GunName_ResourceKey;

		WEAPON_INFO_UI_DATA(void) : 
			image_ResourceKey("empty_Image") {};

		~WEAPON_INFO_UI_DATA(void) override = default;
	};

	struct LOG_MESSAGE_AREA_UI_DATA : public DEFAULT_UI_DATA {
		// �g�p����t�H���g
		std::string font_ResourceKey;

		// �\���ł���s�̍ő吔
		int maxLineCount;

		// ���b�Z�[�W���o�ꂷ��܂ł̎���
		float enterTime;

		// ���b�Z�[�W���ޏꂷ��܂ł̎���
		float exitTime;

		// ���b�Z�[�W��\�����鎞��
		float waitTime;

		// ���O���b�Z�[�W���擾����֐���ID
		int getTextFuncID;

		LOG_MESSAGE_AREA_UI_DATA(void) {};

		~LOG_MESSAGE_AREA_UI_DATA(void) override = default;
	};

	struct RESULT_TEXT_UI_DATA : public DEFAULT_UI_DATA {
		// �g�p����f�t�H���g�̃��b�Z�[�W
		std::string defMessage;

		// �g�p����t�H���g
		std::string font_ResourceKey;

		// �t�H���g�̃T�C�Y
		int fontSize;

		// �t�H���g�̐F
		unsigned int fontColor;

		// �ǉ�����e�L�X�g���擾����֐���ID
		int getAddTextFuncID;

		RESULT_TEXT_UI_DATA(void) {};

		~RESULT_TEXT_UI_DATA(void) override = default;
	};

	struct TIME_UI_DATA : public DEFAULT_UI_DATA {
		// �g�p����t�H���g
		std::string font_ResourceKey;

		// �t�H���g�̃T�C�Y
		int fontSize;

		// �t�H���g�̐F
		unsigned int fontColor;

		TIME_UI_DATA(void) {};

		~TIME_UI_DATA(void) override = default;
	};

private:

	// UI���C���[�̃��X�g
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

