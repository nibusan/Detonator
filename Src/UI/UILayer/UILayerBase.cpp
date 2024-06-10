#include "../../Application.h"
#include "../../Manager/ResourceManager.h"
#include "../UIBase.h"
#include "../UIFunctionList.h"
#include "../Parts/Image/UI_Image.h"
#include "../Parts/Text/UI_Text.h"
#include "../Parts/Common/Button/UI_Button.h"
#include "../Parts/Common/Window/UI_Window.h"
#include "../Parts/Common/InputTextArea/UI_InputTextArea.h"
#include "../Parts/Common/ColorBox/UI_ColorBox.h"
#include "../Parts/Common/ModelPreview/UI_ModelPreview.h"
#include "../Parts/Common/Cursor/UI_Cursor.h"
#include "../Parts/Game/MessageBox/UI_MessageBox.h"
#include "../Parts/Game/WeaponInfo/UI_WeaponInfo.h"
#include "../Parts/Game/LogMessageArea/UI_LogMessageArea.h"
#include "../Parts/Result/ResultText/UI_ResultText.h"
#include "../Parts/Game/Time/UI_Time.h"
#include "UILayerBase.h"

UILayerBase::UILayerBase(void) : 
isActive_(true) {

}

void UILayerBase::Init(void) {
	drawScreen_ = std::make_unique<Graphic>(MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true));
	SetDefaultUI();
}

void UILayerBase::Update(void) {
	if (!IsActive()) return;
	for (const auto& ui : uis_) {
		UpdateUI(ui);
	}
}

void UILayerBase::Draw(void) {
	auto defScreen = GetDrawScreen();

	SetDrawScreen(drawScreen_->GetHandle());
	ClearDrawScreen();
	for (const auto& ui : uis_) {
		DrawUI(ui);
	}

	SetDrawScreen(defScreen);
	drawScreen_->Draw(Vector2<float>(0.0f, 0.0f), false, nullptr);
}

void UILayerBase::Release(void) {
	for (const auto& ui : uis_) {
		ReleaseUI(ui);
	}
}

void UILayerBase::AddUI(std::shared_ptr<UIBase> ui) {
	uis_.emplace_back(ui);
}

bool UILayerBase::IsActive(void) const {
	return isActive_;
}

void UILayerBase::SetIsActive(bool flag) {
	isActive_ = flag;
}

void UILayerBase::UpdateUI(std::shared_ptr<UIBase> ui) {
	// UIが呼び出したイベント関数のIDを格納
	std::vector<int> calledFuncIDList;

	ui->Update();

	// 第一引数のUIの子の数
	auto childs = ui->GetChilds();
	auto childCount = childs.size();
	if (childCount <= 0) return;
	for (int i = 0; i < childCount; i++) {
		UpdateUI(childs[i]);
	}
}

void UILayerBase::DrawUI(std::shared_ptr<UIBase> ui) {
	ui->Draw();

	// 第一引数のUIの子の数
	auto childs = ui->GetChilds();
	auto childCount = childs.size();
	if (childCount <= 0) return;
	for (int i = 0; i < childCount; i++) {
		DrawUI(childs[i]);
	}
}

void UILayerBase::ReleaseUI(std::shared_ptr<UIBase> ui) {
	ui->Release();

	// 第一引数のUIの子の数
	auto childs = ui->GetChilds();
	auto childCount = childs.size();
	if (childCount <= 0) return;
	for (int i = 0; i < childCount; i++) {
		ReleaseUI(childs[i]);
	}
}

std::shared_ptr<UIBase> UILayerBase::CreateUI(std::shared_ptr<UILayerList::DEFAULT_UI_DATA> uiData) {
	std::shared_ptr<UIBase> ui;
	
	auto& resouceManager = ResourceManager::GetInstance();
	auto& uiFunctionList = UIFunctionList::GetInstance();

	switch (uiData->uiType) {
	case UI::TYPE::IMAGE: {
		auto uiData_Image = std::dynamic_pointer_cast<UILayerList::IMAGE_UI_DATA>(uiData);
		Weak_Graphic graphic = std::dynamic_pointer_cast<Graphic>(resouceManager.GetResourceFile(uiData_Image->image_ResourceKey));
		ui = std::make_shared<UI_Image>(graphic);
		if (uiData->isFixedSize) {
			ui->CreateDrawScreen(uiData->size);
		}
		else {
			ui->CreateDrawScreen(graphic.lock()->GetSize());
		}
	}
		break;

	case UI::TYPE::TEXT: {
		auto uiData_Text = std::dynamic_pointer_cast<UILayerList::TEXT_UI_DATA>(uiData);
		Weak_Font font = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_Text->font_ResourceKey));
		ui = std::make_shared<UI_Text>(uiData_Text->message, font.lock()->GetFontName(), uiData_Text->fontSize, uiData_Text->fontColor);
		auto text = std::dynamic_pointer_cast<UI_Text>(ui);
		if (uiData->isFixedSize) {
			ui->CreateDrawScreen(uiData->size);
		}
		else {
			ui->CreateDrawScreen(
				{
					GetDrawStringWidthToHandle(uiData_Text->message.c_str(), uiData_Text->message.length(), text->GetFont().lock()->GetHandle()),
					GetFontSizeToHandle(text->GetFont().lock()->GetHandle())
				}
			);
		}
	}
		break;

	case UI::TYPE::BUTTON: {
		auto uiData_Button = std::dynamic_pointer_cast<UILayerList::BUTTON_UI_DATA>(uiData);
		Weak_Graphic graphic = std::dynamic_pointer_cast<Graphic>(resouceManager.GetResourceFile(uiData_Button->image_ResourceKey));
		Weak_Font font = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_Button->font_ResourceKey));
		Weak_Font font2 = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_Button->font_ResourceKey2));
		ui = std::make_shared<UI_Button>(
			graphic, 
			uiData_Button->message,
			font.lock()->GetFontName(),
			uiData_Button->fontSize, 
			uiData_Button->fontColor, 
			uiData_Button->message2,
			font2.lock()->GetFontName(),
			uiData_Button->fontSize2, 
			uiData_Button->fontColor2
			);
		if (uiData->isFixedSize) {
			ui->CreateDrawScreen(uiData->size);
		}
		else {
			ui->CreateDrawScreen(graphic.lock()->GetSize());
		}
	}
		break;

	case UI::TYPE::SLIDER:
		break;

	case UI::TYPE::WINDOW: {
		auto uiData_Window = std::dynamic_pointer_cast<UILayerList::WINDOW_UI_DATA>(uiData);
		Weak_Graphic graphic = std::dynamic_pointer_cast<Graphic>(resouceManager.GetResourceFile(uiData_Window->image_ResourceKey));
		Weak_Font font = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_Window->font_ResourceKey));
		Weak_Font font2 = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_Window->font_ResourceKey2));
		ui = std::make_shared<UI_Window>(
			graphic, 
			uiData_Window->message, 
			font.lock()->GetFontName(), 
			uiData_Window->fontSize, 
			uiData_Window->fontColor, 
			uiData_Window->message2, 
			font2.lock()->GetFontName(), 
			uiData_Window->fontSize2, 
			uiData_Window->fontColor2
			);
		if (uiData->isFixedSize) {
			ui->CreateDrawScreen(uiData->size);
		}
		else {
			ui->CreateDrawScreen(graphic.lock()->GetSize());
		}

		auto window = std::dynamic_pointer_cast<UI_Window>(ui);
		for (int i = 0; i < uiData_Window->buttons.size(); i++) {
			auto button = CreateUI(uiData_Window->buttons[i]);
			AddUI(button);
			window->AddButton(button);
		}
	}
		break;

	case UI::TYPE::INPUT_TEXT_AREA: {
		auto uiData_InputTextArea = std::dynamic_pointer_cast<UILayerList::INPUT_TEXT_AREA_UI_DATA>(uiData);
		Weak_Graphic graphic = std::dynamic_pointer_cast<Graphic>(resouceManager.GetResourceFile(uiData_InputTextArea->image_ResourceKey));
		Weak_Font font = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_InputTextArea->font_ResourceKey));
		ui = std::make_shared<UI_InputTextArea>(
			graphic,
			font.lock()->GetFontName(), 
			uiData_InputTextArea->fontSize,
			uiData_InputTextArea->fontColor,
			uiData_InputTextArea->inputType,
			uiData_InputTextArea->inputTextType,
			uiData_InputTextArea->maxLength,
			uiFunctionList.GetEndInputFunction(uiData_InputTextArea->endInputFuncID)
		);
		if (uiData->isFixedSize) {
			ui->CreateDrawScreen(uiData->size);
		}
		else {
			ui->CreateDrawScreen(graphic.lock()->GetSize());
		}
	}
		break;

	case UI::TYPE::COLOR_BOX: {
		auto uiData_ColorBox = std::dynamic_pointer_cast<UILayerList::COLOR_BOX_UI_DATA>(uiData);
		ui = std::make_shared<UI_ColorBox>(uiData_ColorBox->color);
		ui->CreateDrawScreen(uiData_ColorBox->size);
	}
		break;

	case UI::TYPE::MODEL_PREVIEW: {
		auto uiData_ModelPreview = std::dynamic_pointer_cast<UILayerList::MODEL_PREVIEW_UI_DATA>(uiData);
		Weak_Graphic graphic = std::dynamic_pointer_cast<Graphic>(resouceManager.GetResourceFile(uiData_ModelPreview->image_ResourceKey));
		Weak_Graphic graphic_Mask = std::dynamic_pointer_cast<Graphic>(resouceManager.GetResourceFile(uiData_ModelPreview->imageMask_ResourceKey));
		Weak_PixelShader pixelShader = std::dynamic_pointer_cast<PixelShader>(resouceManager.GetResourceFile(uiData_ModelPreview->pixelShader_ResourceKey));
		ui = std::make_shared<UI_ModelPreview>(graphic, graphic_Mask, pixelShader, uiData_ModelPreview->objectIDs);
		if (uiData->isFixedSize) {
			ui->CreateDrawScreen(uiData->size);
		}
		else {
			ui->CreateDrawScreen(graphic.lock()->GetSize());
		}
	}
		break;

	case UI::TYPE::CURSOR: {
		auto uiData_Cursor = std::dynamic_pointer_cast<UILayerList::CURSOR_UI_DATA>(uiData);
		Weak_Graphic graphic = std::dynamic_pointer_cast<Graphic>(resouceManager.GetResourceFile(uiData_Cursor->image_ResourceKey));
		ui = std::make_shared<UI_Cursor>(graphic, uiData_Cursor->dir);
		if (uiData->isFixedSize) {
			ui->CreateDrawScreen(uiData->size);
		}
		else {
			ui->CreateDrawScreen(graphic.lock()->GetSize());
		}
	}
		break;

	case UI::TYPE::MESSAGE_BOX: {
		auto uiData_MessageBox = std::dynamic_pointer_cast<UILayerList::MESSAGE_BOX_UI_DATA>(uiData);
		Weak_Graphic graphic = std::dynamic_pointer_cast<Graphic>(resouceManager.GetResourceFile(uiData_MessageBox->image_ResourceKey));
		Weak_Font font = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_MessageBox->font_ResourceKey));
		ui = std::make_shared<UI_MessageBox>(
			graphic,
			uiData_MessageBox->defMessage,
			font.lock()->GetFontName(),
			uiData_MessageBox->fontSize,
			uiData_MessageBox->fontColor,
			uiFunctionList.GetAddTextFunction(uiData_MessageBox->getAddTextFuncID)
		);
		if (uiData->isFixedSize) {
			ui->CreateDrawScreen(uiData->size);
		}
		else {
			ui->CreateDrawScreen(graphic.lock()->GetSize());
		}
	}
		break;

	case UI::TYPE::WEAPON_INFO: {
		auto uiData_WeaponInfo = std::dynamic_pointer_cast<UILayerList::WEAPON_INFO_UI_DATA>(uiData);
		Weak_Graphic graphic = std::dynamic_pointer_cast<Graphic>(resouceManager.GetResourceFile(uiData_WeaponInfo->image_ResourceKey));
		std::vector<Weak_Graphic> graphic_Grenades;
		for (int i = 0; i < uiData_WeaponInfo->image_Grenade_ResourceKeys.size(); i++) {
			graphic_Grenades.push_back(std::dynamic_pointer_cast<Graphic>(resouceManager.GetResourceFile(uiData_WeaponInfo->image_Grenade_ResourceKeys[i])));
		}
		std::vector<Weak_Graphic> graphic_Guns;
		for (int i = 0; i < uiData_WeaponInfo->image_Gun_ResourceKeys.size(); i++) {
			graphic_Guns.push_back(std::dynamic_pointer_cast<Graphic>(resouceManager.GetResourceFile(uiData_WeaponInfo->image_Gun_ResourceKeys[i])));
		}
		Weak_Font font_Magazine = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_WeaponInfo->font_Magazine_ResourceKey));
		Weak_Font font_GunName = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_WeaponInfo->font_GunName_ResourceKey));
		ui = std::make_shared<UI_WeaponInfo>(
			graphic,
			graphic_Grenades,
			graphic_Guns,
			GUN_TYPE::AK47,
			font_Magazine.lock()->GetFontName(),
			font_GunName.lock()->GetFontName()
		);
		if (uiData->isFixedSize) {
			ui->CreateDrawScreen(uiData->size);
		}
		else {
			ui->CreateDrawScreen(graphic.lock()->GetSize());
		}
	}
		break;

	case UI::TYPE::LOG_MESSAGE_AREA: {
		auto uiData_LogMessageArea = std::dynamic_pointer_cast<UILayerList::LOG_MESSAGE_AREA_UI_DATA>(uiData);
		Weak_Font font = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_LogMessageArea->font_ResourceKey));
		ui = std::make_shared<UI_LogMessageArea>(
			font.lock()->GetFontName(),
			uiData_LogMessageArea->maxLineCount,
			uiData_LogMessageArea->enterTime,
			uiData_LogMessageArea->exitTime,
			uiData_LogMessageArea->waitTime,
			uiFunctionList.GetLogDataFunction(uiData_LogMessageArea->getTextFuncID)
		);
		ui->CreateDrawScreen(uiData->size);
	}
		break;

	case UI::TYPE::RESULT_TEXT : {
		auto uiData_ResultText = std::dynamic_pointer_cast<UILayerList::RESULT_TEXT_UI_DATA>(uiData);
		Weak_Font font = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_ResultText->font_ResourceKey));
		ui = std::make_shared<UI_ResultText>(
			uiData_ResultText->defMessage,
			font.lock()->GetFontName(),
			uiData_ResultText->fontSize,
			uiData_ResultText->fontColor,
			uiFunctionList.GetAddTextFunction(uiData_ResultText->getAddTextFuncID)
		);
		if (uiData->isFixedSize) {
			ui->CreateDrawScreen(uiData->size);
		}
		else {
			ui->CreateDrawScreen(
				{
					1000,
					GetFontSizeToHandle(font.lock()->GetHandle())
				}
			);
		}
	}
		break;

	case UI::TYPE::GAME_TIME : {
		auto uiData_Time = std::dynamic_pointer_cast<UILayerList::TIME_UI_DATA>(uiData);
		Weak_Font font = std::dynamic_pointer_cast<Font>(resouceManager.GetResourceFile(uiData_Time->font_ResourceKey));
		ui = std::make_shared<UI_Time>(
			font.lock()->GetFontName(),
			uiData_Time->fontSize,
			uiData_Time->fontColor
		);
		auto ui_Time = std::dynamic_pointer_cast<UI_Time>(ui);
		if (uiData->isFixedSize) {
			ui->CreateDrawScreen(uiData->size);
		}
		else {
			ui->CreateDrawScreen(
				{
					GetDrawStringWidthToHandle("00:00", 5, ui_Time->GetFont().lock()->GetHandle()),
					GetFontSizeToHandle(ui_Time->GetFont().lock()->GetHandle())
				}
			);
		}
	}
		break;

	case UI::TYPE::MAX:
	default:
		break;
	}

	ui->Init();
	ui->SetPos(uiData->pos);
	ui->SetAlpha(uiData->alpha);
	ui->SetScale(uiData->scale);
	ui->SetAngle(uiData->angle);
	ui->SetIsActive(uiData->isActive);
	ui->SetIsClickable(uiData->isClickable);
	ui->SetDrawPositionType(uiData->positionType);
	ui->CreateCollisionData(uiData->positionType, std::make_shared<Collision2D_Rect>(uiData->pos, ui->GetSize().ToVector2f() / 2));
	if (uiData->onClickFuncID != -1) {
		ui->SetOnClickFunction(uiFunctionList.GetOnClickFunction(uiData->onClickFuncID));
	}

	return ui;
}
