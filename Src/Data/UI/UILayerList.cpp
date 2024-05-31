#include <fstream>
#include <sstream>
#include <istream>
#include "../../Common/nlohmann-json/json.hpp"
#include "../Message/MessageTable.h"
#include "../../UI/UIFunctionList.h"
#include "../../Utility/Utility.h"
#include "UILayerList.h"

void UILayerList::Load(void) {
	nlohmann::json json_UILayersData;
	std::ifstream ifs("Data/Data/UI/UILayers.json");
	ifs >> json_UILayersData;
	
	for (int i = 0; i < json_UILayersData["UILayerDatas"].size(); i++) {
		for (int j = 0; j < json_UILayersData["UILayerDatas"][i]["UIs"].size(); j++) {
			auto uiData = json_UILayersData["UILayerDatas"][i]["UIs"][j];
	
			//UIのデータを生成する
			auto ui = CreateUIData(uiData);

			uiLayerList_[static_cast<UILayerList::TYPE>(json_UILayersData["UILayerDatas"][i]["LayerType"])][ui->id] = ui;
		}
	}
}

std::shared_ptr<UILayerList::DEFAULT_UI_DATA> UILayerList::CreateUIData(nlohmann::json jsonUIData) {
	std::shared_ptr<DEFAULT_UI_DATA> data;
	
	switch (static_cast<UI::TYPE>(jsonUIData["Type"])) {
	case UI::TYPE::IMAGE : {
		data = std::make_shared<IMAGE_UI_DATA>();
		auto image = std::dynamic_pointer_cast<IMAGE_UI_DATA>(data);
		image->image_ResourceKey = jsonUIData["Attribute"]["Image_ResourceKey"];
		if (jsonUIData["Attribute"]["PixelShader_ResourceKey"] != nullptr) {
			image->pixelShader_ResourceKey = jsonUIData["Attribute"]["PixelShader_ResourceKey"];
		}
	}
		break;
	case UI::TYPE::TEXT: {
		data = std::make_shared<TEXT_UI_DATA>();
		auto text = std::dynamic_pointer_cast<TEXT_UI_DATA>(data);
		text->message = Utility::WStringToString(MessageTable::GetInstance().GetMessage_(jsonUIData["Attribute"]["MessageID"]));
		text->font_ResourceKey = jsonUIData["Attribute"]["Font_ResourceKey"];
		text->fontSize = jsonUIData["Attribute"]["FontSize"];
		text->fontColor = std::stoi(std::string(jsonUIData["Attribute"]["FontColor"]), nullptr, 16);
	}
		break;

	case UI::TYPE::BUTTON: {
		data = std::make_shared<BUTTON_UI_DATA>();
		auto button = std::dynamic_pointer_cast<BUTTON_UI_DATA>(data);
		button->image_ResourceKey = jsonUIData["Attribute"]["Image_ResourceKey"];
		button->message = Utility::WStringToString(MessageTable::GetInstance().GetMessage_(jsonUIData["Attribute"]["MessageID"]));
		button->font_ResourceKey = jsonUIData["Attribute"]["Font_ResourceKey"];
		button->fontSize = jsonUIData["Attribute"]["FontSize"];
		button->fontColor = std::stoi(std::string(jsonUIData["Attribute"]["FontColor"]), nullptr, 16);
		button->message2 = Utility::WStringToString(MessageTable::GetInstance().GetMessage_(jsonUIData["Attribute"]["MessageID2"]));
		button->font_ResourceKey2 = jsonUIData["Attribute"]["Font_ResourceKey2"];
		button->fontSize2 = jsonUIData["Attribute"]["FontSize2"];
		button->fontColor2 = std::stoi(std::string(jsonUIData["Attribute"]["FontColor2"]), nullptr, 16);
	}
		break;

	case UI::TYPE::WINDOW: {
		data = std::make_shared<WINDOW_UI_DATA>();
		auto window = std::dynamic_pointer_cast<WINDOW_UI_DATA>(data);
		window->image_ResourceKey = jsonUIData["Attribute"]["Image_ResourceKey"];
		window->message = Utility::WStringToString(MessageTable::GetInstance().GetMessage_(jsonUIData["Attribute"]["MessageID"]));
		window->font_ResourceKey = jsonUIData["Attribute"]["Font_ResourceKey"];
		window->fontSize = jsonUIData["Attribute"]["FontSize"];
		window->fontColor = std::stoi(std::string(jsonUIData["Attribute"]["FontColor"]), nullptr, 16);
		window->message2 = Utility::WStringToString(MessageTable::GetInstance().GetMessage_(jsonUIData["Attribute"]["MessageID2"]));
		window->font_ResourceKey2 = jsonUIData["Attribute"]["Font_ResourceKey2"];
		window->fontSize2 = jsonUIData["Attribute"]["FontSize2"];
		window->fontColor2 = std::stoi(std::string(jsonUIData["Attribute"]["FontColor2"]), nullptr, 16);
		for (int i = 0; i < jsonUIData["Attribute"]["Buttons"].size(); i++) {
			window->buttons.emplace_back(CreateUIData(jsonUIData["Attribute"]["Buttons"][i]));
		}
	}
		break;

	case UI::TYPE::INPUT_TEXT_AREA: {
		data = std::make_shared<INPUT_TEXT_AREA_UI_DATA>();
		auto inputTextArea = std::dynamic_pointer_cast<INPUT_TEXT_AREA_UI_DATA>(data);
		inputTextArea->image_ResourceKey = jsonUIData["Attribute"]["Image_ResourceKey"];
		inputTextArea->font_ResourceKey = jsonUIData["Attribute"]["Font_ResourceKey"];
		inputTextArea->fontSize = jsonUIData["Attribute"]["FontSize"];
		inputTextArea->fontColor = std::stoi(std::string(jsonUIData["Attribute"]["FontColor"]), nullptr, 16);
		inputTextArea->inputType = jsonUIData["Attribute"]["InputType"];
		inputTextArea->inputTextType = jsonUIData["Attribute"]["InputTextType"];
		inputTextArea->maxLength = jsonUIData["Attribute"]["MaxLength"];
		inputTextArea->endInputFuncID = jsonUIData["Attribute"]["EndInputFuncID"];
	}
		break;

	case UI::TYPE::COLOR_BOX: {
		data = std::make_shared<COLOR_BOX_UI_DATA>();
		auto colorBox = std::dynamic_pointer_cast<COLOR_BOX_UI_DATA>(data);
		colorBox->color = std::stoi(std::string(jsonUIData["Attribute"]["Color"]), nullptr, 16);
	}
		break;

	case UI::TYPE::MODEL_PREVIEW: {
		data = std::make_shared<MODEL_PREVIEW_UI_DATA>();
		auto modelPreview = std::dynamic_pointer_cast<MODEL_PREVIEW_UI_DATA>(data);
		modelPreview->image_ResourceKey = jsonUIData["Attribute"]["Image_ResourceKey"];
		modelPreview->imageMask_ResourceKey = jsonUIData["Attribute"]["Image_Mask_ResourceKey"];
		modelPreview->pixelShader_ResourceKey = jsonUIData["Attribute"]["PixelShader_ResourceKey"];
		for (int i = 0; i < jsonUIData["Attribute"]["PixelShader_ResourceKey"].size(); i++) {
			modelPreview->objectIDs.emplace_back(jsonUIData["Attribute"]["PreviewObjectIDs"][i]);
		}
	}
		break;

	case UI::TYPE::CURSOR: {
		data = std::make_shared<CURSOR_UI_DATA>();
		auto cursor = std::dynamic_pointer_cast<CURSOR_UI_DATA>(data);
		cursor->image_ResourceKey = jsonUIData["Attribute"]["Image_ResourceKey"];
		cursor->dir = jsonUIData["Attribute"]["Direction"];
	}
		break;

	case UI::TYPE::MESSAGE_BOX: {
		data = std::make_shared<MESSAGE_BOX_UI_DATA>();
		auto messageBox = std::dynamic_pointer_cast<MESSAGE_BOX_UI_DATA>(data);
		messageBox->image_ResourceKey = jsonUIData["Attribute"]["Image_ResourceKey"];
		messageBox->defMessage = Utility::WStringToString(MessageTable::GetInstance().GetMessage_(jsonUIData["Attribute"]["DefMessageID"]));
		messageBox->font_ResourceKey = jsonUIData["Attribute"]["Font_ResourceKey"];
		messageBox->fontSize = jsonUIData["Attribute"]["FontSize"];
		messageBox->fontColor = std::stoi(std::string(jsonUIData["Attribute"]["FontColor"]), nullptr, 16);
		messageBox->getAddTextFuncID = jsonUIData["Attribute"]["GetAddTextFuncID"];
	}
		break;

	case UI::TYPE::WEAPON_INFO: {
		data = std::make_shared<WEAPON_INFO_UI_DATA>();
		auto weaponInfo = std::dynamic_pointer_cast<WEAPON_INFO_UI_DATA>(data);
		weaponInfo->image_ResourceKey = jsonUIData["Attribute"]["Image_ResourceKey"];
		for (int i = 0; i < jsonUIData["Attribute"]["Image_Grenade_ResourceKeys"].size(); i++) {
			weaponInfo->image_Grenade_ResourceKeys.push_back(jsonUIData["Attribute"]["Image_Grenade_ResourceKeys"][i]);
		}
		for (int i = 0; i < jsonUIData["Attribute"]["Image_Gun_ResourceKeys"].size(); i++) {
			weaponInfo->image_Gun_ResourceKeys.push_back(jsonUIData["Attribute"]["Image_Gun_ResourceKeys"][i]);
		}
		weaponInfo->font_Magazine_ResourceKey = jsonUIData["Attribute"]["Font_Magazine_ResourceKey"];
		weaponInfo->font_GunName_ResourceKey = jsonUIData["Attribute"]["Font_GunName_ResourceKey"];
	}
		break;

	case UI::TYPE::LOG_MESSAGE_AREA: {
		data = std::make_shared<LOG_MESSAGE_AREA_UI_DATA>();
		auto logMessageArea = std::dynamic_pointer_cast<LOG_MESSAGE_AREA_UI_DATA>(data);
		logMessageArea->font_ResourceKey = jsonUIData["Attribute"]["Font_ResourceKey"];
		logMessageArea->maxLineCount = jsonUIData["Attribute"]["MaxLineCount"];
		logMessageArea->enterTime = jsonUIData["Attribute"]["EnterTime"];
		logMessageArea->exitTime = jsonUIData["Attribute"]["ExitTime"];
		logMessageArea->waitTime = jsonUIData["Attribute"]["WaitTime"];
		logMessageArea->getTextFuncID = jsonUIData["Attribute"]["GetTextFuncID"];
	}
		break;

	case UI::TYPE::RESULT_TEXT: {
		data = std::make_shared<RESULT_TEXT_UI_DATA>();
		auto resultText = std::dynamic_pointer_cast<RESULT_TEXT_UI_DATA>(data);
		resultText->defMessage = Utility::WStringToString(MessageTable::GetInstance().GetMessage_(jsonUIData["Attribute"]["DefMessageID"]));
		resultText->font_ResourceKey = jsonUIData["Attribute"]["Font_ResourceKey"];
		resultText->fontSize = jsonUIData["Attribute"]["FontSize"];
		resultText->fontColor = std::stoi(std::string(jsonUIData["Attribute"]["FontColor"]), nullptr, 16);
		resultText->getAddTextFuncID = jsonUIData["Attribute"]["GetAddTextFuncID"];
	}
		break;

	case UI::TYPE::GAME_TIME: {
		data = std::make_shared<TIME_UI_DATA>();
		auto time = std::dynamic_pointer_cast<TIME_UI_DATA>(data);
		time->font_ResourceKey = jsonUIData["Attribute"]["Font_ResourceKey"];
		time->fontSize = jsonUIData["Attribute"]["FontSize"];
		time->fontColor = std::stoi(std::string(jsonUIData["Attribute"]["FontColor"]), nullptr, 16);
	}
		break;

	default:
		data = std::make_shared<DEFAULT_UI_DATA>();
		break;
	}
	data->id = jsonUIData["ID"];
	data->uiType = static_cast<UI::TYPE>(jsonUIData["Type"]);
	data->pos = { jsonUIData["Pos"]["X"],jsonUIData["Pos"]["Y"] };
	data->isFixedSize = jsonUIData["FixedSize"] != nullptr;
	if (data->isFixedSize) {
		data->size = { jsonUIData["FixedSize"]["Width"],jsonUIData["FixedSize"]["Height"] };
	}
	data->scale = jsonUIData["Scale"];
	data->angle = jsonUIData["Angle"];
	data->alpha = jsonUIData["Alpha"];
	data->isActive = jsonUIData["IsActive"];
	data->isClickable = jsonUIData["IsClickable"];
	data->positionType = static_cast<UIBase::DRAW_POSITION_TYPE>(jsonUIData["PositionType"]);
	data->onClickFuncID = jsonUIData["OnClickFuncID"];
	
	if (jsonUIData["Childs"] != nullptr) {
		// 子UIが存在する場合、再起呼び出しを行い
		// 子UIのデータを生成して配列に格納する
		auto count = jsonUIData["Childs"].size();
		for (int i = 0; i < count; i++) {
			data->childs.push_back(CreateUIData(jsonUIData["Childs"][i]));
		}
	}
	
	return data;
}

void UILayerList::Init(void) {
	Load();
}

void UILayerList::Release(void) {
	uiLayerList_.clear();
}

const std::unordered_map<int, std::shared_ptr<UILayerList::DEFAULT_UI_DATA>>& UILayerList::GetUILayerData(UILayerList::TYPE type) const {
	return uiLayerList_.at(type);
}
