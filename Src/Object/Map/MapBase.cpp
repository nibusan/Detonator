#include "string"
#include "../../Manager/CollisionManager.h"
#include "../../Manager/EffectManager.h"
#include "../../Manager/ResourceManager.h"
#include "../Common/ColliderModel.h"
#include "MapBase.h"

void MapBase::Init_Object(void)
{
	// 仮読み込み
	//transform_.SetModel(
	//	MV1LoadModel("Data/Model/Map/Collision.mv1")
	//);

	//// 移動回転大きさ等をモデルに反映
	//transform_.Update();



		// NORMAL
	//map_[(int)TYPE::NORMAL].
	//	transform_.SetModel(MV1LoadModel("Data/Model/Map/Hangar_Full_Obj.mv1"));
	map_[(int)TYPE::NORMAL].models_[0] = std::make_shared<Model>(ResourceManager::GetInstance().GetResourceFile("Model_Map_Hangar_Full_Obj")->GetHandle());
	map_[(int)TYPE::NORMAL].models_[1] = std::make_shared<Model>(ResourceManager::GetInstance().GetResourceFile("Model_Map_Hangar_Full_Collision")->GetHandle());
	map_[(int)TYPE::NORMAL].models_[0]->SetIsAutoDeleteHandle(true);
	map_[(int)TYPE::NORMAL].transform_.SetModel(map_[(int)TYPE::NORMAL].models_[0]->GetHandle());
	map_[(int)TYPE::NORMAL].
		collider_ = std::make_shared<ColliderModel>
		(GetPtr(), Collider::OBJECT_TYPE::MAP,
			map_[(int)TYPE::NORMAL].models_[1]->GetHandle());
	CollisionManager::GetInstance().
		SetCollider(map_[(int)TYPE::NORMAL].collider_);

	// UNBREAK
	//map_[(int)TYPE::UNBREAK].
	//	transform_.SetModel(MV1LoadModel("Data/Model/Map/Hangar_Full_Wall.mv1"));
	map_[(int)TYPE::UNBREAK].models_[0] = std::make_shared<Model>(ResourceManager::GetInstance().GetResourceFile("Model_Map_Hangar_Full_Wall")->GetHandle());
	map_[(int)TYPE::UNBREAK].models_[1] = std::make_shared<Model>(ResourceManager::GetInstance().GetResourceFile("Model_Map_Hangar_Full_CollisionUnBreak")->GetHandle());
	map_[(int)TYPE::UNBREAK].models_[0]->SetIsAutoDeleteHandle(true);
	map_[(int)TYPE::UNBREAK].transform_.SetModel(map_[(int)TYPE::UNBREAK].models_[0]->GetHandle());
	map_[(int)TYPE::UNBREAK].
		collider_ = std::make_shared<ColliderModel>
		(GetPtr(), Collider::OBJECT_TYPE::MAP_UNBREAK, 
			map_[(int)TYPE::UNBREAK].models_[1]->GetHandle());
	CollisionManager::GetInstance().
		SetCollider(map_[(int)TYPE::UNBREAK].collider_);

	Update_Object();
}

void MapBase::Update_Object(void)
{
	//// 移動回転大きさ等をモデルに反映
	//transform_.Update();

	// コライダー更新
	for (auto m : map_)
	{
		auto col = std::dynamic_pointer_cast<ColliderModel>(m.collider_);
		col->Update();
	}

}

void MapBase::Draw_Object(void)
{
	// マップ描画
	map_[(int)TYPE::UNBREAK].transform_.Draw();
	map_[(int)TYPE::NORMAL].transform_.Draw();
}

void MapBase::Release_Object(void)
{
	// モデル削除(仮)
	transform_.Release();

	for (auto m : map_)
	{
		m.transform_.Release();
	}
}

void MapBase::OnHit(int no)
{

	auto m = std::dynamic_pointer_cast<ColliderModel>(map_[(int)TYPE::NORMAL].collider_);



	// 衝突用マップ
	// 衝突したフレーム(衝突用マップ)を削除(スケールを0に)
	m->HideFrame(no);

	// 衝突したフレームの名前を取得
	auto name = MV1GetFrameName(m->GetModelId(), no);

	std::string fNo = "";
	
	int size = _tcslen(name);
	for (int i = 1; i < size; i++)
	{
		fNo += name[i];
	}
	

	// 衝突用フレームの各フレームの名前が
	// 表示用フレームのフレーム番号に対応している(しないといけないめんどくさい)

	//// 対象フレームの位置を取得する
	//auto fMat = MV1GetFrameLocalMatrix(map_[(int)TYPE::NORMAL].transform_.modelId, std::stoi(fNo));
	//auto fPos = MGetTranslateElem(fMat);	// 行列から移動値を取り出す

	//EffectManager::EFFECT temp;
	//temp.pos = fPos;
	//VECTOR localRot = Quaternion::ToEuler(transform_.quaRot);
	//temp.rot = localRot;
	//temp.scl = { 40.0f,40.0f,40.0f };
	//temp.speed = 1.5f;
	//temp.type = EffectManager::EFFECT_TYPE::BREAK;
	//EffectManager::GetInstance().AddEffect(temp);

	// 衝突したフレーム(表示用マップ)を非表示に
	map_[(int)TYPE::NORMAL].transform_.HideFrame(std::stoi(fNo));


}

int MapBase::GetMapModel(TYPE type)
{
	return map_[(int)type].transform_.modelId;
}
