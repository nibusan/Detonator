#include "../../Utility/Utility.h"
#include "../../Manager/ResourceManager.h"
#include "GrenadeMenu.h"

GrenadeMenu::GrenadeMenu(ID id,int slotNo, VECTOR pos):
gameManager_(GameManager::GetInstance())
{
	ID_ = id;
	slotNo_ = slotNo;
	ChangeType(GameManager::GetInstance().GetLoadOut((int)ID_).greType_[slotNo_]);

	models_[(int)GRENADE_TYPE::HAND] = std::make_unique<Model>(ResourceManager::GetInstance().GetResourceFile("Model_Grenade_HandGrenade1")->GetHandle());
	models_[(int)GRENADE_TYPE::HAND]->SetIsAutoDeleteHandle(true);
	greTransform_[(int)GRENADE_TYPE::HAND].SetModel(models_[(int)GRENADE_TYPE::HAND]->GetHandle());

	models_[(int)GRENADE_TYPE::SMOKE] = std::make_unique<Model>(ResourceManager::GetInstance().GetResourceFile("Model_Grenade_SmokeGrenade")->GetHandle());
	models_[(int)GRENADE_TYPE::SMOKE]->SetIsAutoDeleteHandle(true);
	greTransform_[(int)GRENADE_TYPE::SMOKE].SetModel(models_[(int)GRENADE_TYPE::SMOKE]->GetHandle());

	models_[(int)GRENADE_TYPE::STICKY] = std::make_unique<Model>(ResourceManager::GetInstance().GetResourceFile("Model_Grenade_StickyGrenade")->GetHandle());
	models_[(int)GRENADE_TYPE::STICKY]->SetIsAutoDeleteHandle(true);
	greTransform_[(int)GRENADE_TYPE::STICKY].SetModel(models_[(int)GRENADE_TYPE::STICKY]->GetHandle());

	//// ÉÇÉfÉãâºì«Ç›çûÇ›
	////greTransform_[(int)GRENADE_TYPE::HAND].SetModel(MV1LoadModel("Data/Model/Grenade/HandGranade1.mv1"));
	//greTransform_[(int)GRENADE_TYPE::SMOKE].SetModel(MV1LoadModel("Data/Model/Grenade/SmokeGranade.mv1"));
	//greTransform_[(int)GRENADE_TYPE::STICKY].SetModel(MV1LoadModel("Data/Model/Grenade/StickyGrenade.mv1"));

	greTransform_[(int)GRENADE_TYPE::HAND].pos = pos;
	greTransform_[(int)GRENADE_TYPE::HAND].pos.y -= 5.0f;
	greTransform_[(int)GRENADE_TYPE::SMOKE].pos = pos;
	greTransform_[(int)GRENADE_TYPE::SMOKE].pos.y += 10.0f;
	greTransform_[(int)GRENADE_TYPE::STICKY].pos = pos;
	greTransform_[(int)GRENADE_TYPE::STICKY].pos.y += 2.0f;

	greTransform_[(int)GRENADE_TYPE::HAND].scl = { 0.95f,0.95f ,0.95f };
	greTransform_[(int)GRENADE_TYPE::SMOKE].scl = { 0.15f,0.15f ,0.15f };
	greTransform_[(int)GRENADE_TYPE::STICKY].scl = { 1.2f,1.2f ,1.2f };

	greTransform_[(int)GRENADE_TYPE::HAND].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(90.0f),0.0f });
	greTransform_[(int)GRENADE_TYPE::SMOKE].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(0.0f),0.0f });
	greTransform_[(int)GRENADE_TYPE::SMOKE].quaRot = Quaternion::Euler({ 0.0f,0.0f,Utility::Deg2RadF(-20.0f) });
	greTransform_[(int)GRENADE_TYPE::STICKY].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(-90.0f),0.0f });

	for (auto& transform : greTransform_) {
		transform.Update();
	}
}

GrenadeMenu::~GrenadeMenu(void)
{
	for (auto& transform : greTransform_) {
		transform.Release();
	}
}

void GrenadeMenu::Update(void)
{
	ChangeType(GameManager::GetInstance().GetLoadOut((int)ID_).greType_[slotNo_]);
	greTransform_[(int)type_].Update();
}

void GrenadeMenu::Draw(void)
{
	// ÉÇÉfÉãï`âÊ
	greTransform_[(int)type_].Draw();
}

int GrenadeMenu::GetSlotNo(void) const
{
	return slotNo_;
}

void GrenadeMenu::ChangeType(GRENADE_TYPE type)
{
	if (type_ == type) return;
	type_ = type;
}

void GrenadeMenu::SetID(ID id)
{
	ID_ = id;
}

void GrenadeMenu::Init_Object(void) {

}

void GrenadeMenu::Update_Object(void) {
	ChangeType(GameManager::GetInstance().GetLoadOut((int)ID_).greType_[slotNo_]);
	greTransform_[(int)type_].Update();
}

void GrenadeMenu::Draw_Object(void) {
	// ÉÇÉfÉãï`âÊ
	greTransform_[(int)type_].Draw();
}

void GrenadeMenu::Release_Object(void) {
	for (auto& transform : greTransform_) {
		transform.Release();
	}
	transform_.Release();
}
