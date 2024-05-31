#include <Dxlib.h>
#include "../../../../Manager/Camera/CameraBase.h"
#include "../../../../Manager/SceneManager.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Object/Actor/Player/PlayerMenu.h"
#include "../../../../Object/Gun/GunMenu.h"
#include "../../../../Object/Grenade/GrenadeMenu.h"
#include "UI_ModelPreview.h"

UI_ModelPreview::UI_ModelPreview(Weak_Graphic graphic, Weak_Graphic graphic_Mask, Weak_PixelShader pixelShader, std::vector<OBJECT_TYPE> objectIDs) :
graphic_(graphic), 
graphic_Mask_(graphic_Mask) {
	auto& resourceManager = ResourceManager::GetInstance();
	usingPixelShader_ = pixelShader;

	for (auto& objID : objectIDs) {
		std::shared_ptr<ObjectBase> obj;
		switch (objID) {
		case UI_ModelPreview::OBJECT_TYPE::PLAYER:
			obj = std::make_shared<PlayerMenu>(ID::PLAYER_1);
			obj->Init();
			object_.emplace_back(obj);
			break;

		case UI_ModelPreview::OBJECT_TYPE::GUN_1:
			obj = std::make_shared<GunMenu>(ID::PLAYER_1, VECTOR());
			obj->Init();
			object_.emplace_back(obj);
			break;

		case UI_ModelPreview::OBJECT_TYPE::GUN_2:
			obj = std::make_shared<GunMenu>(ID::PLAYER_1, VECTOR());
			obj->Init();
			object_.emplace_back(obj);
			break;

		case UI_ModelPreview::OBJECT_TYPE::GUN_3:
			obj = std::make_shared<GunMenu>(ID::PLAYER_1, VECTOR());
			obj->Init();
			object_.emplace_back(obj);
			break;

		case UI_ModelPreview::OBJECT_TYPE::GUN_4:
			obj = std::make_shared<GunMenu>(ID::PLAYER_1, VECTOR());
			obj->Init();
			object_.emplace_back(obj);
			break;

		case UI_ModelPreview::OBJECT_TYPE::GRENADE_1:
			obj = std::make_shared<GrenadeMenu>(ID::PLAYER_1, 0, VGet( 0.0f,-60.0f,400.0f ));
			obj->Init();
			object_.emplace_back(obj);
			break;

		case UI_ModelPreview::OBJECT_TYPE::GRENADE_2:
			obj = std::make_shared<GrenadeMenu>(ID::PLAYER_1, 1, VGet( 0.0f,-60.0f,400.0f ));
			obj->Init();
			object_.emplace_back(obj);
			break;

		default:
			break;
		}
	}
}

void UI_ModelPreview::Init_UI(void) {
	
}

bool UI_ModelPreview::Update_UI(void) {
	for (auto& obj : object_) {
		obj->Update();
	}
	return false;
}

void UI_ModelPreview::Draw_UI(void) {
	// ƒJƒƒ‰Ý’è
	SceneManager::GetInstance().GetCamera()->SetBeforeDraw();

	graphic_.lock()->Draw({ 0.0f,0.0f }, false, nullptr);

	for (auto& obj : object_) {
		obj->Draw();
	}

	for (auto& model : model_) {
		model.lock()->Draw(
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f},
			{1.0f,1.0f,1.0f},
			nullptr,
			nullptr
		);
	}
}

void UI_ModelPreview::Release_UI(void) {
	for (auto& obj : object_) {
		obj->Release();
	}
}
