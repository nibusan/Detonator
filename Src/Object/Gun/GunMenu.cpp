#include "../../Manager/ResourceManager.h"
#include "../../Utility/Utility.h"
#include "GunMenu.h"

GunMenu::GunMenu(ID id, Transform* follow, int frameNo) :
gameManager_(GameManager::GetInstance()),
follow_(follow),
followFrameNo_(frameNo)
{
	ID_ = id;
	type_ = GUN_TYPE::AWM;

	models_[(int)GUN_TYPE::AK47] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Gun_AK47")->GetHandle());
	models_[(int)GUN_TYPE::AK47]->SetIsAutoDeleteHandle(true);
	gunTransform_[(int)GUN_TYPE::AK47].SetModel(models_[(int)GUN_TYPE::AK47]->GetHandle());

	models_[(int)GUN_TYPE::AWM] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Gun_AWM")->GetHandle());
	models_[(int)GUN_TYPE::AWM]->SetIsAutoDeleteHandle(true);
	gunTransform_[(int)GUN_TYPE::AWM].SetModel(models_[(int)GUN_TYPE::AWM]->GetHandle());

	models_[(int)GUN_TYPE::M4] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Gun_M4")->GetHandle());
	models_[(int)GUN_TYPE::M4]->SetIsAutoDeleteHandle(true);
	gunTransform_[(int)GUN_TYPE::M4].SetModel(models_[(int)GUN_TYPE::M4]->GetHandle());

	models_[(int)GUN_TYPE::M24R] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Gun_M24R")->GetHandle());
	models_[(int)GUN_TYPE::M24R]->SetIsAutoDeleteHandle(true);
	gunTransform_[(int)GUN_TYPE::M24R].SetModel(models_[(int)GUN_TYPE::M24R]->GetHandle());


	relativePos_[(int)GUN_TYPE::AK47] = { 0.0f,-2.0f,22.0f };
	relativePos_[(int)GUN_TYPE::AWM] = { 0.0f,-3.0f,5.0f };
	relativePos_[(int)GUN_TYPE::M4] = { 0.0f,4.0f,25.0f };
	relativePos_[(int)GUN_TYPE::M24R] = { 0.0f,8.0f,13.0f };

	gunTransform_[(int)GUN_TYPE::AK47].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(90.0f),0.0f });
	gunTransform_[(int)GUN_TYPE::AWM].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(-90.0f),0.0f });
	gunTransform_[(int)GUN_TYPE::M4].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(-90.0f),0.0f });
	gunTransform_[(int)GUN_TYPE::M24R].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(180.0f),0.0f });
	gunTransform_[(int)GUN_TYPE::M4].scl = { 1.5f,1.5f,1.5f };
	gunTransform_[(int)GUN_TYPE::M24R].scl = { 8.0f,8.0f,8.0f };

	ChangeType(gameManager_.GetLoadOut((int)ID_).gunType_);

	Update();
}

GunMenu::GunMenu(ID id, VECTOR pos) :
gameManager_(GameManager::GetInstance()),
follow_(nullptr)
{
	ID_ = id;
	type_ = GUN_TYPE::M24R;

	// モデル仮読み込み
	gunTransform_[(int)GUN_TYPE::AK47].SetModel(MV1LoadModel("Data/Model/Gun/AK47.mv1"));
	gunTransform_[(int)GUN_TYPE::AWM].SetModel(MV1LoadModel("Data/Model/Gun/AWM.mv1"));
	gunTransform_[(int)GUN_TYPE::M4].SetModel(MV1LoadModel("Data/Model/Gun/M4.mv1"));
	gunTransform_[(int)GUN_TYPE::M24R].SetModel(MV1LoadModel("Data/Model/Gun/M24R.mv1"));

	relativePos_[(int)GUN_TYPE::AK47] = { 0.0f,-2.0f,22.0f };
	relativePos_[(int)GUN_TYPE::AWM] = { 0.0f,-3.0f,5.0f };
	relativePos_[(int)GUN_TYPE::M4] = { 0.0f,8.0f,25.0f };
	relativePos_[(int)GUN_TYPE::M24R] = { 0.0f,8.0f,13.0f };

	gunTransform_[(int)GUN_TYPE::AK47].pos = pos;
	gunTransform_[(int)GUN_TYPE::AWM].pos = pos;
	gunTransform_[(int)GUN_TYPE::AWM].pos.x += 28.0f;
	gunTransform_[(int)GUN_TYPE::M4].pos = pos;
	gunTransform_[(int)GUN_TYPE::M4].pos.x += 2.0f;
	gunTransform_[(int)GUN_TYPE::M4].pos.y += 5.0f;
	gunTransform_[(int)GUN_TYPE::M24R].pos = pos;
	gunTransform_[(int)GUN_TYPE::M24R].pos.y += 10.0f;

	gunTransform_[(int)GUN_TYPE::AK47].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(90.0f),0.0f });
	gunTransform_[(int)GUN_TYPE::AWM].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(-90.0f),0.0f });
	gunTransform_[(int)GUN_TYPE::M4].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(-90.0f),0.0f });
	gunTransform_[(int)GUN_TYPE::M24R].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(180.0f),0.0f });
	gunTransform_[(int)GUN_TYPE::M4].scl = { 1.3f,1.3f,1.3f };
	gunTransform_[(int)GUN_TYPE::AWM].scl = { 0.8f,0.8f,0.8f };
	gunTransform_[(int)GUN_TYPE::M24R].scl = { 9.0f,9.0f,9.0f };

	gunTransform_[(int)GUN_TYPE::AK47].quaRot = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(-90.0f),0.0f });
	gunTransform_[(int)GUN_TYPE::AWM].quaRot = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(-90.0f),0.0f });
	gunTransform_[(int)GUN_TYPE::M4].quaRot = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(-90.0f),0.0f });
	gunTransform_[(int)GUN_TYPE::M24R].quaRot = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(-90.0f),0.0f });

	//scopeTransform_[(int)GUN_TYPE::AK47].SetModel(MV1LoadModel("Data/Model/Gun/Scope/Red+Dot+Sight.mv1"));
	//scopeTransform_[(int)GUN_TYPE::AWM].SetModel(MV1LoadModel("Data/Model/Gun/Scope/AWMScope.mv1"));
	//scopeTransform_[(int)GUN_TYPE::M4].SetModel(MV1LoadModel("Data/Model/Gun/Scope/Red+Dot+Sight.mv1"));
	//scopeTransform_[(int)GUN_TYPE::M24R].SetModel(MV1LoadModel("Data/Model/Gun/Scope/Red+Dot+Sight.mv1"));

	//scopeTransform_[(int)GUN_TYPE::AK47].scl = { 2.0f,2.0f,2.0f };
	//scopeTransform_[(int)GUN_TYPE::AWM].scl = { 2.0f,2.0f,2.0f };
	//scopeTransform_[(int)GUN_TYPE::M4].scl = { 2.0f,2.0f,2.0f };
	//scopeTransform_[(int)GUN_TYPE::M24R].scl = { 2.0f,2.0f,2.0f };

	//scopeRelativePos_[(int)GUN_TYPE::AK47] = { 0.0f,12.0f,-3.0f };
	//scopeRelativePos_[(int)GUN_TYPE::AWM] = { 0.0f,12.0f,-3.0f };
	//scopeRelativePos_[(int)GUN_TYPE::M4] = { 0.0f,5.0f,-8.0f };
	//scopeRelativePos_[(int)GUN_TYPE::M24R] = { 0.0f,5.0f,5.0f };

	//scopeTransform_[(int)GUN_TYPE::AK47].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(90.0f),0.0f });
	//scopeTransform_[(int)GUN_TYPE::AWM].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(90.0f),0.0f });
	//scopeTransform_[(int)GUN_TYPE::M4].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(90.0f),0.0f });
	//scopeTransform_[(int)GUN_TYPE::M24R].quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(90.0f),0.0f });

	ChangeType(gameManager_.GetLoadOut((int)ID_).gunType_);
	Update();
}

GunMenu::~GunMenu(void)
{
	for (auto& transform : gunTransform_) {
		transform.Release();
	}
	//for (auto& transform : scopeTransform_) {
	//	transform.Release();
	//}
}

void GunMenu::Update(void)
{
	//transform_[(int)type_].pos = { 0.0f,-10.0f,100.0f };
	//transform_[(int)type_].quaRot = Quaternion::Euler(0.0f, Utility::Deg2RadF(-90), 0.0f);

	Follow();

	ChangeType(gameManager_.GetLoadOut((int)ID_).gunType_);
	gunTransform_[(int)type_].Update();

	//// スコープ追従
	//scopeTransform_[(int)type_].quaRot = transform_[(int)type_].quaRot;
	//scopeTransform_[(int)type_].pos = VAdd(transform_[(int)type_].pos,
	//	scopeTransform_[(int)type_].quaRot.PosAxis(scopeRelativePos_[(int)type_]));
	//scopeTransform_[(int)type_].Update();
}

void GunMenu::Draw(void)
{
	// モデル描画
	gunTransform_[(int)type_].Draw();
	//scopeTransform_[(int)type_].Draw();
}

void GunMenu::ChangeType(GUN_TYPE type)
{
	if (type_ == type) return;
	type_ = type;
	Update();
}

void GunMenu::SetID(ID id)
{
	ID_ = id;
}

void GunMenu::Follow(void)
{
	if (follow_ == nullptr)return;

	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する(先)
	auto mat = MV1GetFrameLocalWorldMatrix(follow_->modelId, 19);
	auto fraPosA = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	//Target(fraPosA);

	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する(元)
	mat = MV1GetFrameLocalWorldMatrix(follow_->modelId, followFrameNo_);
	auto fraPosB = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	// 2点から角度を求める(BtoA)
	auto diff = VSub(fraPosA, fraPosB);
	
	// 方位角
	float azimuth = atan2f(diff.x, diff.z);

	// 仰角
	float elevation = atanf(-diff.y / Utility::MagnitudeF({ diff.x,0.0f,diff.z }));

	// Z軸は回転させない
	VECTOR rot = { elevation ,azimuth , 0.0f };
	gunTransform_[(int)type_].quaRot = Quaternion::Euler(rot);

	// 位置同期
	gunTransform_[(int)type_].pos = VAdd(fraPosB, gunTransform_[(int)type_].quaRot.PosAxis(relativePos_[(int)type_]));
}

void GunMenu::Init_Object(void) {

}

void GunMenu::Update_Object(void) {
	Follow();

	ChangeType(gameManager_.GetLoadOut((int)ID_).gunType_);
	gunTransform_[(int)type_].Update();
}

void GunMenu::Draw_Object(void) {
	// モデル描画
	gunTransform_[(int)type_].Draw();
}

void GunMenu::Release_Object(void) {
	for (auto& transform : gunTransform_) {
		transform.Release();
	}
	transform_.Release();
	follow_ = nullptr;
}
