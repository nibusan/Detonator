#include "../Object/ObjectBase.h"
#include "../Object/Common/ColliderModel.h"
#include "../Object/Common/ColliderSphere.h"
#include "../Object/Common/ColliderCapsule.h"
#include "../Object/Common/ColliderLine.h"
#include "../Utility/Utility.h"
#include "CollisionManager.h"

CollisionManager* CollisionManager::instance_ = nullptr;

void CollisionManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new CollisionManager();
	}
	instance_->Init();
}

CollisionManager& CollisionManager::GetInstance(void)
{
	return *instance_;
}

void CollisionManager::Init(void)
{
}

void CollisionManager::Destroy(void)
{
	for (auto obj : colliders_)
	{
		obj.second.clear();
	}

	colliders_.clear();

	delete instance_;
}

void CollisionManager::ClearColliders(void)
{
	for (auto obj : colliders_)
	{
		obj.second.clear();
	}

	colliders_.clear();
}

void CollisionManager::SetCollider(std::weak_ptr<Collider> obj)
{

	// オブジェクトタイプを取得
	auto type = obj.lock()->objType;

	// マップ配列のキーを使ってその中の要素数を数える
	if (colliders_.count(type) <= 0)
	{
		// 要素0なら初期設定
		std::vector<std::weak_ptr<Collider>> temp;
		colliders_.emplace(type, temp);
	}

	// マップ配列の可変長配列の中にオブジェクトを格納
	colliders_.at(type).emplace_back(obj);
}

void CollisionManager::DeleteColliderNonParent(void)
{
	for (auto cols : colliders_)
	{
		std::erase_if(cols.second, 
			[](std::weak_ptr<Collider> col) {return col.lock()->parent_.expired(); });
	}
}

void CollisionManager::DeleteCollider(std::weak_ptr<Collider> col)
{
	// 特定のコライダーを削除
	auto& cols = colliders_.at(col.lock()->objType);

	std::erase_if(cols, [&](const std::weak_ptr<Collider> c)
					{return c.lock() == col.lock(); });
}

void CollisionManager::CollisionSingle(void)
{
	// 親がいなくなったコライダーを自動的に削除
	DeleteColliderNonParent();

	// プレイヤーとマップの衝突
	CollisionPlayerMap();

	// エネミーとマップ
	CollisionEnemyMap();

	// カメラとマップの衝突判定
	//CollisionCameraMap();

	// グレネードとマップの衝突
	CollisionGrenadeMap();


	// 爆発とマップの衝突
	CollisionExplosionMap();

	// 爆発とプレイヤー
	CollisionExplosionPlayer();

	// スモークとプレイヤー
	CollisionSmokePlayer();

	// 爆発とエネミー
	CollisionExplosionEnemy();

	CollisionExplosionShield();

	// プレイヤーの弾とシールド
	CollisionPlayerBulletShield();



	// プレイヤーの弾とエネミー
	CollisionPlayerBulletEnemy();

	//// プレイヤーとプレイヤーの弾
	CollisionPlayerBullet();

	// プレイヤーとエネミーの弾
	CollisionPlayerEnemyBullet();

	// プレイヤーの弾とマップ
	CollisionPlayerBulletMap();

	// エネミーの弾とマップ
	CollisionEnemyBulletMap();

}

void CollisionManager::CollisionMulti(void)
{
	// 親がいなくなったコライダーを自動的に削除
	DeleteColliderNonParent();

	// プレイヤーとマップの衝突
	CollisionPlayerMap();

	// グレネードとマップの衝突
	CollisionGrenadeMap();

	// 爆発とマップの衝突
	CollisionExplosionMap();

	// 爆発とプレイヤー
	CollisionExplosionPlayer();

	// プレイヤーとプレイヤーの弾
	CollisionPlayerBullet();

	// プレイヤーの弾とマップ
	CollisionPlayerBulletMap();
}



void CollisionManager::CollisionPlayerMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_CAP) <= 0) return;

	auto& objsPC = colliders_.at(Collider::OBJECT_TYPE::PLAYER_CAP);

	int no = 0;

	for (auto pc : objsPC)
	{
		// 移動量
		VECTOR moveXZpow = VSub(pc.lock()->pos,pc.lock()->prePos);
		moveXZpow.y = 0.0f;
		VECTOR moveYpow = VSub(pc.lock()->pos, pc.lock()->prePos);
		moveYpow.x = moveYpow.z = 0.0f;

		// プレイヤーとマップの衝突(壁)
		CollisionPlayerMapSide(moveXZpow, pc.lock(), no);

		// プレイヤーとマップの衝突(天井)
		CollisionPlayerMapHead(pc.lock(), no);

		// プレイヤーとマップの衝突(地面)
		CollisionPlayerMapFoot(pc.lock(), no);

		// プレイヤーとマップの衝突(斜め上)
		CollisionPlayerMapSlopeUp(pc.lock(), no);

		// プレイヤーとマップの衝突(斜め下)
		CollisionPlayerMapSlopeDown(pc.lock(), no);

		no++;
	}
}

void CollisionManager::CollisionPlayerMapSide(const VECTOR movePow, std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	//auto objsPC = objects_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	// カプセルとモデル
	//for (auto pc : objsPC)
	//{
		auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());

		// 横移動後座標
		VECTOR movePosA = VAdd(tempPC->GetPrePosA(), movePow);
		movePosA.y -= tempPC->GetPreRadius();
		VECTOR movePosB = VAdd(tempPC->GetPrePosB(), movePow);
		movePosB.y += tempPC->GetPreRadius();

		// マップオブジェクト
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			// 横移動だけさせた状態のカプセルと衝突判定させて
			// 衝突していない場合処理を行わない
			auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
				movePosA, movePosB, tempPC->GetRadius());

			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{
				// 垂直ポリゴン
				if (info.Dim[i].Normal.y != 0.0f) continue;

				int pHit = HitCheck_Capsule_Triangle(
					tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (pHit)
				{
					tempPC->parent_.lock()->OnHit(info.Dim[i]);
				}
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}


		// マップ壁地面
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
				movePosA, movePosB, tempPC->GetRadius());


			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{

				// 垂直ポリゴン
				if (info.Dim[i].Normal.y != 0.0f) continue;

				int pHit = HitCheck_Capsule_Triangle(
					tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (pHit)
				{
					tempPC->parent_.lock()->OnHit(info.Dim[i]);
				}

			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}
	//}
}

void CollisionManager::CollisionPlayerMapSide2(const VECTOR movePow, std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	//auto objsPC = objects_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	// カプセルとモデル
	//for (auto pc : objsPC)
	//{
	auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());

	// 横移動後座標
	//VECTOR movePosA = VAdd(tempPC->GetPrePosA(), movePow);
	//movePosA.y -= tempPC->GetPreRadius();
	//VECTOR movePosB = VAdd(tempPC->GetPrePosB(), movePow);
	//movePosB.y += tempPC->GetPreRadius();

	// マップオブジェクト
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		// 横移動だけさせた状態のカプセルと衝突判定させて
		// 衝突していない場合処理を行わない
		auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());

		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{
			// 垂直ポリゴン
			if (info.Dim[i].Normal.y != 0.0f) continue;

			int pHit = HitCheck_Capsule_Triangle(
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (pHit)
			{
				tempPC->parent_.lock()->OnHit(info.Dim[i]);
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}


	// マップ壁地面
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{

			// 垂直ポリゴン
			if (info.Dim[i].Normal.y != 0.0f) continue;

			int pHit = HitCheck_Capsule_Triangle(
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (pHit)
			{
				tempPC->parent_.lock()->OnHit(info.Dim[i]);
			}

		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}
}

void CollisionManager::CollisionPlayerMapHead(std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_TOPLINE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsPL = colliders_.at(Collider::OBJECT_TYPE::PLAYER_TOPLINE);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	// 法線ベクトルが
	// {0.0f,1.0f,0.0f}のもの

	// 処理順　カプセル→線分
	// Y == 1　→　0 < Y < 1
	// 平面→斜め
	// 衝突している物の中で
	// プレイヤーに最も近い平面のポリゴンを探索する

	// カプセルとモデル
	//for (auto pc : objsPC)
	//{
		auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());

		// 最も低い平面ポリゴン
		MV1_COLL_RESULT_POLY low = {};
		bool isHit = false;

		// マップオブジェクト
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{

				if (info.Dim[i].Normal.y != -1) continue;

				if (!isHit)
				{
					low = info.Dim[i];
					isHit = true;
				}

				if (info.Dim[i].HitPosition.y < low.HitPosition.y)
				{
					low = info.Dim[i];
				}
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}

		// マップ壁地面
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{

				if (info.Dim[i].Normal.y != -1) continue;

				if (!isHit)
				{
					low = info.Dim[i];
					isHit = true;
				}

				if (info.Dim[i].HitPosition.y < low.HitPosition.y)
				{
					low = info.Dim[i];
				}
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}

		// 平面ポリゴン
		if (isHit)
		{
			// 下方向に
			for (int n = 0; n < MAP_COL_MAX; n++)
			{
				int pHit = HitCheck_Capsule_Triangle(
					tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
					low.Position[0], low.Position[1], low.Position[2]);

				if (pHit)
				{
					VECTOR dir = { 0.0f,-1.0f,0.0f };
					// プレイヤーを押し返す
					tempPC->parent_.lock()->OnHit(dir);

					// 衝突していたらもう一度判定
					continue;
				}

				// 衝突しなくなったら回数ループを抜ける	
				break;
			}
		}
	//}

	// 線分とモデル
	//for (auto pl : objsPL)
	//{
		auto tempPL = std::dynamic_pointer_cast<ColliderLine>(objsPL[no].lock());

		low = {};
		isHit = false;

		// マップオブジェクト
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
				tempPL->GetPosA(), tempPL->GetPosB());
			if (info.HitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				if (info.Dim[i].Normal.y != -1) continue;

				if (!isHit)
				{
					low = info.Dim[i];
					isHit = true;
				}

				if (info.Dim[i].HitPosition.y < low.HitPosition.y)
				{
					low = info.Dim[i];
				}
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}


		// マップオブジェクト
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
				tempPL->GetPosA(), tempPL->GetPosB());
			if (info.HitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				if (info.Dim[i].Normal.y != 1) continue;

				if (!isHit)
				{
					low = info.Dim[i];
					isHit = true;
				}

				if (info.Dim[i].HitPosition.y < low.HitPosition.y)
				{
					low = info.Dim[i];
				}
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}

		if (isHit)
		{
			// プレイヤーをぴったりに移動

			// プレイヤーの高さ分マイナスする
			float height = tempPL->parent_.lock()->GetTransform().height;
			low.HitPosition.y -= height;

			tempPL->parent_.lock()->OnHit(low);
		}
	//}

}

void CollisionManager::CollisionPlayerMapFoot(std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_DOWNLINE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsPL = colliders_.at(Collider::OBJECT_TYPE::PLAYER_DOWNLINE);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	// 法線ベクトルが
	// {0.0f,1.0f,0.0f}のもの


	// 処理順　カプセル→線分
	// Y == 1　→　0 < Y < 1
	// 平面→斜め
	// 衝突している物の中で
	// プレイヤーに最も近い平面のポリゴンを探索する

	// カプセルとモデル
	auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());
	if (!tempPC->isCollision) return;

	// 最も高い平面ポリゴン
	MV1_COLL_RESULT_POLY high = {};
	bool isHit = false;

	// マップオブジェクト
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{

			if (info.Dim[i].Normal.y != 1) continue;

			if (!isHit)
			{
				high = info.Dim[i];
				isHit = true;
			}

			if (high.HitPosition.y < info.Dim[i].HitPosition.y)
			{
				high = info.Dim[i];
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}

	// マップ壁地面
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{

			if (info.Dim[i].Normal.y != 1) continue;

			if (!isHit)
			{
				high = info.Dim[i];
				isHit = true;
			}

			if (high.HitPosition.y < info.Dim[i].HitPosition.y)
			{
				high = info.Dim[i];
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}

	// 平面ポリゴン
	if (isHit)
	{
		// 上方向に
		for (int n = 0; n < MAP_COL_MAX; n++)
		{
			int pHit = HitCheck_Capsule_Triangle(
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
				high.Position[0], high.Position[1], high.Position[2]);

			if (pHit)
			{
				VECTOR dir = { 0.0f,1.0f,0.0f };
				// プレイヤーを押し返す
				tempPC->parent_.lock()->OnHit(dir);

				// 衝突していたらもう一度判定
				continue;
			}

			// 衝突しなくなったら回数ループを抜ける	
			break;
		}
	}
	else
	{
		return;
	}

	// 線分とモデル
	auto tempPL = std::dynamic_pointer_cast<ColliderLine>(objsPL[no].lock());

	MV1_COLL_RESULT_POLY hit = {};
	isHit = false;

	// マップオブジェクト
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
			tempPL->GetPosA(), tempPL->GetPosB());
		if (info.HitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			if (info.Dim[i].Normal.y != 1) continue;

			if (!isHit)
			{
				hit = info.Dim[i];
				isHit = true;
			}

			if (hit.HitPosition.y < info.Dim[i].HitPosition.y)
			{
				hit = info.Dim[i];
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}


	// マップオブジェクト
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
			tempPL->GetPosA(), tempPL->GetPosB());
		if (info.HitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			if (info.Dim[i].Normal.y != 1) continue;

			if (!isHit)
			{
				hit = info.Dim[i];
				isHit = true;
			}

			if (hit.HitPosition.y < info.Dim[i].HitPosition.y)
			{
				hit = info.Dim[i];
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}

	if (isHit)
	{
		// プレイヤーをマップ上に移動
		tempPL->parent_.lock()->OnHit(hit);
	}

}


void CollisionManager::CollisionPlayerMapSlopeUp(std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_DOWNLINE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	//auto objsPC = objects_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsPL = colliders_.at(Collider::OBJECT_TYPE::PLAYER_DOWNLINE);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);


	// 処理順　カプセル→線分
	// Y == 1　→　0 < Y < 1
	// 平面→斜め
	// 衝突している物の中で
	// プレイヤーに最も近い平面のポリゴンを探索する

	// カプセルとモデル
	auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());

	// 最も近い斜め(0 < y < 1)ポリゴン
	MV1_COLL_RESULT_POLY nea = {};
	bool isHit = false;

	// マップオブジェクト
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{
			// 0 < y < 1
			if (info.Dim[i].Normal.y <= 0.0f || 1.0f <= info.Dim[i].Normal.y) continue;

			if (!isHit)
			{
				nea = info.Dim[i];
				isHit = true;
			}

			// プレイヤーと現在の最近値との距離
			float dis = Triangle_Point_MinLength(
				nea.Position[0], nea.Position[1], nea.Position[2], tempPC->pos);

			// プレイヤーと新しい値との距離
			float disNew = Triangle_Point_MinLength(
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPC->pos);

			if (disNew < dis)
			{
				nea = info.Dim[i];
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}

	// マップ壁地面
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{

			// 0 < y < 1
			if (info.Dim[i].Normal.y <= 0.0f || 1.0f <= info.Dim[i].Normal.y) continue;

			if (!isHit)
			{
				nea = info.Dim[i];
				isHit = true;
			}

			// プレイヤーと現在の最近値との距離
			float dis = Triangle_Point_MinLength(
				nea.Position[0], nea.Position[1], nea.Position[2], tempPC->pos);
				
			// プレイヤーと新しい値との距離
			float disNew = Triangle_Point_MinLength(
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPC->pos);

			if (disNew < dis)
			{
				nea = info.Dim[i];
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}

	// 平面ポリゴン
	if (isHit)
	{
		// 上方向に
		for (int n = 0; n < MAP_COL_MAX; n++)
		{
			int pHit = HitCheck_Capsule_Triangle(
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
				nea.Position[0], nea.Position[1], nea.Position[2]);

			if (pHit)
			{
				VECTOR dir = { 0.0f,1.0f,0.0f };
				// プレイヤーを押し返す
				tempPC->parent_.lock()->OnHit(dir);

				// 衝突していたらもう一度判定
				continue;
			}

			// 衝突しなくなったら回数ループを抜ける	
			break;
		}
	}
	else
	{
		return;
	}

	// 線分とモデル
	auto tempPL = std::dynamic_pointer_cast<ColliderLine>(objsPL[no].lock());

	nea = {};
	isHit = false;

	// マップオブジェクト
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
			tempPL->GetPosA(), tempPL->GetPosB());
		if (info.HitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			// 0 < y < 1
			if (info.Dim[i].Normal.y <= 0.0f || 1.0f <= info.Dim[i].Normal.y) continue;

			if (!isHit)
			{
				nea = info.Dim[i];
				isHit = true;
			}

			// プレイヤーと現在の最近値との距離
			float dis = Triangle_Point_MinLength(
				nea.Position[0], nea.Position[1], nea.Position[2], tempPL->pos);

			// プレイヤーと新しい値との距離
			float disNew = Triangle_Point_MinLength(
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPL->pos);

			if (disNew < dis)
			{
				nea = info.Dim[i];
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}


	// マップオブジェクト
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
			tempPL->GetPosA(), tempPL->GetPosB());
		if (info.HitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			// 0 < y < 1
			if (info.Dim[i].Normal.y <= 0.0f || 1.0f <= info.Dim[i].Normal.y) continue;

			if (!isHit)
			{
				nea = info.Dim[i];
				isHit = true;
			}

			// プレイヤーと現在の最近値との距離
			float dis = Triangle_Point_MinLength(
				nea.Position[0], nea.Position[1], nea.Position[2], tempPL->pos);

			// プレイヤーと新しい値との距離
			float disNew = Triangle_Point_MinLength(
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPL->pos);

			if (disNew < dis)
			{
				nea = info.Dim[i];
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}

	if (isHit)
	{
		// プレイヤーをマップ上に移動
		tempPL->parent_.lock()->OnHit(nea);
	}


}

void CollisionManager::CollisionPlayerMapSlopeDown(std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	//auto objsPC = objects_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);


	// 処理順　カプセル→線分
	// Y == 1　→　0 < Y < 1
	// 平面→斜め
	// 衝突している物の中で
	// プレイヤーに最も近い平面のポリゴンを探索する

	// カプセルとモデル
	//for (auto pc : objsPC)
	//{
		auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());

		// 最も近い斜め(-1 < y < 0)ポリゴン
		MV1_COLL_RESULT_POLY nea = {};
		bool isHit = false;

		// マップオブジェクト
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());

			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{

				if (-1.0f >= info.Dim[i].Normal.y || 0.0f <= info.Dim[i].Normal.y) continue;

				if (!isHit)
				{
					nea = info.Dim[i];
					isHit = true;
				}

				// プレイヤーと現在の最近値との距離
				float dis = Triangle_Point_MinLength(
					nea.Position[0], nea.Position[1], nea.Position[2], tempPC->pos);

				// プレイヤーと新しい値との距離
				float disNew = Triangle_Point_MinLength(
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPC->pos);

				if (disNew < dis)
				{
					nea = info.Dim[i];
				}
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}

		// マップ壁地面
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{

				//-1 < y < 0
				if (-1.0f >= info.Dim[i].Normal.y || 0.0f <= info.Dim[i].Normal.y) continue;

				if (!isHit)
				{
					nea = info.Dim[i];
					isHit = true;
				}

				// プレイヤーと現在の最近値との距離
				float dis = Triangle_Point_MinLength(
					nea.Position[0], nea.Position[1], nea.Position[2], tempPC->pos);

				// プレイヤーと新しい値との距離
				float disNew = Triangle_Point_MinLength(
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPC->pos);

				if (disNew < dis)
				{
					nea = info.Dim[i];
				}
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}

		// 平面ポリゴン
		if (isHit)
		{
			// 移動前方向に
			for (int n = 0; n < MAP_COL_MAX; n++)
			{
				int pHit = HitCheck_Capsule_Triangle(
					tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
					nea.Position[0], nea.Position[1], nea.Position[2]);

				if (pHit)
				{

					VECTOR dir = VNorm(
						VSub(tempPC->prePos, tempPC->pos));


					// プレイヤーを押し返す
					tempPC->parent_.lock()->OnHit(dir);

					// 衝突していたらもう一度判定
					continue;
				}

				// 衝突しなくなったら回数ループを抜ける	
				break;
			}
		}
	//}
}

void CollisionManager::CollisionPlayerBullet(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_CAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::PLAYER_BULLET) <= 0) return;

	auto& objsPC = colliders_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsPB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BULLET);

	for (auto pc : objsPC)
	{
		// 衝突判定を行わないものはスキップ
		if (!pc.lock()->isCollision) continue;

		auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(pc.lock());

		for (auto pb : objsPB)
		{
			// 衝突判定を行わないものはスキップ
			if (!pb.lock()->isCollision) continue;

			auto tempPB = std::dynamic_pointer_cast<ColliderLine>(pb.lock());

			auto isHit = HitCheck_Capsule_Capsule(tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
				tempPB->GetPosA(), tempPB->GetPosB(), 0.1f);

			if (!isHit) continue;

			// 仮のプレイヤーと衝突していた場合
			// 細かく衝突処理を行う

			isHit = false;

			// 頭
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_HEAD) > 0)
			{
				auto objsH = colliders_.at(Collider::OBJECT_TYPE::PLAYER_HEAD);

				for (auto h : objsH)
				{
					auto tempH = std::dynamic_pointer_cast<ColliderSphere>(h.lock());

					isHit = HitCheck_Sphere_Capsule(tempH->pos, tempH->GetRadius(),
						tempPB->GetPosA(), tempPB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// 衝突後処理
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(h, pb);
					pb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;

			// 体
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_BODY) > 0)
			{
				auto objsB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BODY);

				for (auto b : objsB)
				{
					auto tempB = std::dynamic_pointer_cast<ColliderCapsule>(b.lock());

					isHit = HitCheck_Capsule_Capsule(tempB->GetPosA(), tempB->GetPosB(), tempB->GetRadius(),
						tempPB->GetPosA(), tempPB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// 衝突後処理
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(b, pb);
					pb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;

			// 手足
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_LIMBS) > 0)
			{
				auto objsL = colliders_.at(Collider::OBJECT_TYPE::PLAYER_LIMBS);

				for (auto l : objsL)
				{
					auto tempL = std::dynamic_pointer_cast<ColliderCapsule>(l.lock());

					isHit = HitCheck_Capsule_Capsule(tempL->GetPosA(), tempL->GetPosB(), tempL->GetRadius(),
						tempPB->GetPosA(), tempPB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// 衝突後処理
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(l, pb);
					pb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;
		}
	}
}

void CollisionManager::CollisionPlayerEnemyBullet(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_CAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::ENEMY_BULLET) <= 0) return;

	auto& objsPC = colliders_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsEB = colliders_.at(Collider::OBJECT_TYPE::ENEMY_BULLET);

	for (auto pc : objsPC)
	{
		// 衝突判定を行わないものはスキップ
		if (!pc.lock()->isCollision) continue;

		auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(pc.lock());

		for (auto eb : objsEB)
		{
			// 衝突判定を行わないものはスキップ
			if (!eb.lock()->isCollision) continue;

			auto tempEB = std::dynamic_pointer_cast<ColliderLine>(eb.lock());

			auto isHit = HitCheck_Capsule_Capsule(tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
												  tempEB->GetPosA(), tempEB->GetPosB(), 0.1f);

			if (!isHit) continue;
			
			// 仮のプレイヤーと衝突していた場合
			// 細かく衝突処理を行う

			isHit = false;

			// 頭
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_HEAD) > 0)
			{
				auto objsH = colliders_.at(Collider::OBJECT_TYPE::PLAYER_HEAD);
				
				for (auto h : objsH)
				{
					auto tempH = std::dynamic_pointer_cast<ColliderSphere>(h.lock());

					isHit = HitCheck_Sphere_Capsule(tempH->pos, tempH->GetRadius(),
						tempEB->GetPosA(), tempEB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// 衝突後処理
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(h, eb);
					eb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;

			// 体
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_BODY) > 0)
			{
				auto objsB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BODY);

				for (auto b : objsB)
				{
					auto tempB = std::dynamic_pointer_cast<ColliderCapsule>(b.lock());

					isHit = HitCheck_Capsule_Capsule(tempB->GetPosA(), tempB->GetPosB(), tempB->GetRadius(),
						tempEB->GetPosA(), tempEB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// 衝突後処理
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(b, eb);
					eb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;

			// 手足
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_LIMBS) > 0)
			{
				auto objsL = colliders_.at(Collider::OBJECT_TYPE::PLAYER_LIMBS);

				for (auto l : objsL)
				{
					auto tempL = std::dynamic_pointer_cast<ColliderCapsule>(l.lock());

					isHit = HitCheck_Capsule_Capsule(tempL->GetPosA(), tempL->GetPosB(), tempL->GetRadius(),
						tempEB->GetPosA(), tempEB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// 衝突後処理
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(l, eb);
					eb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;
		}
	}
}

void CollisionManager::CollisionPlayerBulletEnemy(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_BULLET) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::ENEMY) <= 0) return;

	auto& objsPB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BULLET);
	auto& objsE = colliders_.at(Collider::OBJECT_TYPE::ENEMY);

	for (auto pb : objsPB)
	{
		// 衝突判定を行わないものはスキップ
		if (!pb.lock()->isCollision) continue;

		auto tempPB = std::dynamic_pointer_cast<ColliderLine>(pb.lock());

		// エネミー
		for (auto e : objsE)
		{
			// 衝突判定を行わないものはスキップ
			if (!e.lock()->isCollision) continue;

			auto tempE = std::dynamic_pointer_cast<ColliderSphere>(e.lock());

			auto isHit = HitCheck_Line_Sphere(tempPB->GetPosA(), tempPB->GetPosB(),
											  tempE->pos, tempE->GetRadius());

			if (isHit)
			{
				// 衝突後処理
				pb.lock()->parent_.lock()->OnHit(e.lock());
				e.lock()->parent_.lock()->OnHit(pb.lock());
			}
		}
	}

}

void CollisionManager::CollisionPlayerBulletMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_BULLET) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 || 
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsPB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BULLET);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	//for (auto pb : objsPB)
	//{
	//	// 衝突判定を行わないものはスキップ
	//	if (!pb.lock()->isCollision) continue;

	//	auto tempPB = std::dynamic_pointer_cast<ColliderLine>(pb.lock());

	//	// 起点に最も近い衝突点
	//	VECTOR nea = {};
	//	bool isHit = false;

	//	// マップ
	//	for (auto m : objsM)
	//	{


	//		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

	//		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
	//			tempPB->GetPosA(), tempPB->GetPosB());

	//		if (info.HitNum <= 0)
	//		{
	//			// 当たり判定結果ポリゴン配列の後始末をする
	//			MV1CollResultPolyDimTerminate(info);
	//			continue;
	//		}

	//		for (int i = 0; i < info.HitNum; i++)
	//		{
	//			auto hit = info.Dim[i];

	//			if (!isHit)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//				isHit = true;
	//				continue;
	//			}

	//			// 比較
	//			auto disA = Utility::Distance(tempPB->GetPosA(), nea);
	//			auto disB = Utility::Distance(tempPB->GetPosA(), info.Dim[i].HitPosition);

	//			if (disA > disB)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//			}
	//		}

	//		// 当たり判定結果ポリゴン配列の後始末をする
	//		MV1CollResultPolyDimTerminate(info);
	//	}

	//	// マップ(破壊不可能)
	//	for (auto mu : objsMU)
	//	{

	//		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

	//		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
	//			tempPB->GetPosA(), tempPB->GetPosB());


	//		if (info.HitNum <= 0)
	//		{
	//			// 当たり判定結果ポリゴン配列の後始末をする
	//			MV1CollResultPolyDimTerminate(info);
	//			continue;
	//		}

	//		for (int i = 0; i < info.HitNum; i++)
	//		{
	//			auto hit = info.Dim[i];

	//			if (!isHit)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//				isHit = true;
	//				continue;
	//			}

	//			// 比較
	//			auto disA = Utility::Distance(tempPB->GetPosA(), nea);
	//			auto disB = Utility::Distance(tempPB->GetPosA(), info.Dim[i].HitPosition);

	//			if (disA > disB)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//			}
	//		}

	//		// 当たり判定結果ポリゴン配列の後始末をする
	//		MV1CollResultPolyDimTerminate(info);

	//	}

	//	if (!isHit) continue;

	//	// 衝突後処理
	//	pb.lock()->parent_.lock()->OnHit(nea);
	//}

	for (auto pb : objsPB)
	{
		// 衝突判定を行わないものはスキップ
		if (!pb.lock()->isCollision) continue;

		auto tempPB = std::dynamic_pointer_cast<ColliderLine>(pb.lock());

		bool isHit = false;

		// マップ
		for (auto m : objsM)
		{


			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Line(tempM->GetModelId(), -1,
				tempPB->GetPosA(), tempPB->GetPosB());

			if (!info.HitFlag)
			{
				//// 当たり判定結果ポリゴン配列の後始末をする
				//MV1CollResultPolyDimTerminate(info);
				continue;
			}

			// 衝突後処理
			pb.lock()->parent_.lock()->OnHit(info.HitPosition);
			isHit = true;
			break;
			//// 当たり判定結果ポリゴン配列の後始末をする
			//MV1CollResultPolyDimTerminate(info);
		}

		if (isHit) break;

		// マップ(破壊不可能)
		for (auto mu : objsMU)
		{

			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Line(tempMU->GetModelId(), -1,
				tempPB->GetPosA(), tempPB->GetPosB());


			if (!info.HitFlag)
			{
				//// 当たり判定結果ポリゴン配列の後始末をする
				//MV1CollResultPolyDimTerminate(info);
				continue;
			}

			// 衝突後処理
			pb.lock()->parent_.lock()->OnHit(info.HitPosition);
			isHit = true;
			break;
			//// 当たり判定結果ポリゴン配列の後始末をする
			//MV1CollResultPolyDimTerminate(info);

		}

	}
}

void CollisionManager::CollisionPlayerBulletShield(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::SHIELD) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::PLAYER_BULLET) <= 0) return;

	auto& objsS = colliders_.at(Collider::OBJECT_TYPE::SHIELD);
	auto& objsPB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BULLET);

	for (auto s : objsS)
	{
		// 衝突判定を行わないものはスキップ
		if (!s.lock()->isCollision) continue;

		auto tempS = std::dynamic_pointer_cast<ColliderCapsule>(s.lock());

		for (auto pb : objsPB)
		{
			// 衝突判定を行わないものはスキップ
			if (!pb.lock()->isCollision) continue;

			auto tempPB = std::dynamic_pointer_cast<ColliderLine>(pb.lock());

			auto isHit = HitCheck_Capsule_Capsule(tempS->GetPosA(), tempS->GetPosB(), tempS->GetRadius(),
				tempPB->GetPosA(), tempPB->GetPosB(), 0.1f);


			if (!isHit) continue;

			// 衝突後処理
			pb.lock()->parent_.lock()->OnHit(s.lock());
			s.lock()->parent_.lock()->OnHit(pb.lock());

		}
	}
}

void CollisionManager::CollisionEnemyMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::ENEMY) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;


	auto& objsE = colliders_.at(Collider::OBJECT_TYPE::ENEMY);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	for (auto e : objsE)
	{
		if (!e.lock()->isCollision) continue;

		auto tempE = std::dynamic_pointer_cast<ColliderSphere>(e.lock());

		MV1_COLL_RESULT_POLY hit = {};
		bool isHit = false;

		// マップオブジェクト
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Sphere(tempM->GetModelId(), -1,
				tempE->pos, tempE->GetRadius());
			if (info.HitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			// 衝突処理
			tempE->parent_.lock()->OnHit(tempM);

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}


		// マップオブジェクト
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Sphere(tempMU->GetModelId(), -1,
				tempE->pos, tempE->GetRadius());
			if (info.HitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			// 衝突処理
			tempE->parent_.lock()->OnHit(tempMU);

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}

	}
}

void CollisionManager::CollisionEnemyBulletMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::ENEMY_BULLET) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsEB = colliders_.at(Collider::OBJECT_TYPE::ENEMY_BULLET);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	//for (auto eb : objsEB)
	//{
	//	// 衝突判定を行わないものはスキップ
	//	if (!eb.lock()->isCollision) continue;

	//	auto tempEB = std::dynamic_pointer_cast<ColliderLine>(eb.lock());

	//	// 起点に最も近い衝突点
	//	VECTOR nea = {};
	//	bool isHit = false;

	//	// マップ
	//	for (auto m : objsM)
	//	{
	//		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

	//		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
	//			tempEB->GetPosA(), tempEB->GetPosB());

	//		if (info.HitNum <= 0)
	//		{
	//			// 当たり判定結果ポリゴン配列の後始末をする
	//			MV1CollResultPolyDimTerminate(info);
	//			continue;
	//		}

	//		for (int i = 0; i < info.HitNum; i++)
	//		{
	//			auto hit = info.Dim[i];

	//			if (!isHit)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//				isHit = true;
	//				continue;
	//			}

	//			// 比較
	//			auto disA = Utility::Distance(tempEB->GetPosA(), nea);
	//			auto disB = Utility::Distance(tempEB->GetPosA(), info.Dim[i].HitPosition);

	//			if (disA > disB)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//			}
	//		}

	//		// 当たり判定結果ポリゴン配列の後始末をする
	//		MV1CollResultPolyDimTerminate(info);
	//	}

	//	// マップ(破壊不可能)
	//	for (auto mu : objsMU)
	//	{

	//		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

	//		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
	//			tempEB->GetPosA(), tempEB->GetPosB());


	//		if (info.HitNum <= 0)
	//		{
	//			// 当たり判定結果ポリゴン配列の後始末をする
	//			MV1CollResultPolyDimTerminate(info);
	//			continue;
	//		}

	//		for (int i = 0; i < info.HitNum; i++)
	//		{
	//			auto hit = info.Dim[i];

	//			if (!isHit)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//				isHit = true;
	//				continue;
	//			}

	//			// 比較
	//			auto disA = Utility::Distance(tempEB->GetPosA(), nea);
	//			auto disB = Utility::Distance(tempEB->GetPosA(), info.Dim[i].HitPosition);

	//			if (disA > disB)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//			}
	//		}

	//		// 当たり判定結果ポリゴン配列の後始末をする
	//		MV1CollResultPolyDimTerminate(info);

	//	}

	//	if (!isHit) continue;

	//	// 衝突後処理
	//	eb.lock()->parent_.lock()->OnHit(nea);
	//}

	for (auto eb : objsEB)
	{
		// 衝突判定を行わないものはスキップ
		if (!eb.lock()->isCollision) continue;

		auto tempEB = std::dynamic_pointer_cast<ColliderLine>(eb.lock());

		bool isHit = false;

		// マップ
		for (auto m : objsM)
		{


			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Line(tempM->GetModelId(), -1,
				tempEB->GetPosA(), tempEB->GetPosB());

			if (!info.HitFlag)
			{
				//// 当たり判定結果ポリゴン配列の後始末をする
				//MV1CollResultPolyDimTerminate(info);
				continue;
			}

			// 衝突後処理
			eb.lock()->parent_.lock()->OnHit(info.HitPosition);
			isHit = true;
			break;
			//// 当たり判定結果ポリゴン配列の後始末をする
			//MV1CollResultPolyDimTerminate(info);
		}

		if (isHit) break;

		// マップ(破壊不可能)
		for (auto mu : objsMU)
		{

			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Line(tempMU->GetModelId(), -1,
				tempEB->GetPosA(), tempEB->GetPosB());


			if (!info.HitFlag)
			{
				//// 当たり判定結果ポリゴン配列の後始末をする
				//MV1CollResultPolyDimTerminate(info);
				continue;
			}

			// 衝突後処理
			eb.lock()->parent_.lock()->OnHit(info.HitPosition);
			isHit = true;
			break;
			//// 当たり判定結果ポリゴン配列の後始末をする
			//MV1CollResultPolyDimTerminate(info);

		}

	}
}

void CollisionManager::CollisionEnemyEyeMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::ENEMY_EYE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsEE = colliders_.at(Collider::OBJECT_TYPE::ENEMY_EYE);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	for (auto ee : objsEE)
	{
		auto tempEE = std::dynamic_pointer_cast<ColliderLine>(ee.lock());

		MV1_COLL_RESULT_POLY hit = {};
		bool isHit = false;

		// マップオブジェクト
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
				tempEE->GetPosA(), tempEE->GetPosB());
			if (info.HitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				auto hit = HitCheck_Line_Triangle(tempEE->GetPosA(), tempEE->GetPosB(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (hit.HitFlag)
				{
					// 
					tempEE->parent_.lock()->OnHit();
				}
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}


		// マップオブジェクト
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
				tempEE->GetPosA(), tempEE->GetPosB());
			if (info.HitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				auto hit = HitCheck_Line_Triangle(tempEE->GetPosA(), tempEE->GetPosB(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (hit.HitFlag)
				{
					// 
					tempEE->parent_.lock()->OnHit();
				}
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}

	}
}

bool CollisionManager::IsCollisionEnemyEyeMap(std::weak_ptr<Collider> obj)
{
	if (colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return false;

	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);
	auto tempEE = std::dynamic_pointer_cast<ColliderLine>(obj.lock());

	// マップオブジェクト
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
			tempEE->GetPosA(), tempEE->GetPosB());
		if (info.HitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			auto hit = HitCheck_Line_Triangle(tempEE->GetPosA(), tempEE->GetPosB(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (hit.HitFlag)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				return true;
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}


	// マップオブジェクト
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
			tempEE->GetPosA(), tempEE->GetPosB());
		if (info.HitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			auto hit = HitCheck_Line_Triangle(tempEE->GetPosA(), tempEE->GetPosB(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (hit.HitFlag)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				return true;
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}

	return false;
}

void CollisionManager::CollisionCameraMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::CAMERA) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	//auto objsPC = objects_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsC = colliders_.at(Collider::OBJECT_TYPE::CAMERA);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	for (auto c : objsC)
	{
		auto tempC = std::dynamic_pointer_cast<ColliderLine>(c.lock());

		MV1_COLL_RESULT_POLY hit = {};
		bool isHit = false;

		// マップオブジェクト
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
				tempC->GetPosA(), tempC->GetPosB());
			if (info.HitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				auto hit = HitCheck_Line_Triangle(tempC->GetPosA(), tempC->GetPosB(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (hit.HitFlag)
				{
					// カメラ移動
					tempC->parent_.lock()->OnHit(hit);
				}
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}


		// マップオブジェクト
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
				tempC->GetPosA(), tempC->GetPosB());
			if (info.HitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				auto hit = HitCheck_Line_Triangle(tempC->GetPosA(), tempC->GetPosB(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (hit.HitFlag)
				{
					// カメラ移動
					tempC->parent_.lock()->OnHit(hit);
				}
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}

	}

}

void CollisionManager::CollisionGrenadeMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::GRENADE_SPHERE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::GRENADE_LINE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsGS = colliders_.at(Collider::OBJECT_TYPE::GRENADE_SPHERE);
	auto& objsGL = colliders_.at(Collider::OBJECT_TYPE::GRENADE_LINE);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	bool isHit = false;

	for (auto gl : objsGL)
	{

		//VECTOR nea = {};
		//VECTOR neaN = {};
		//float minDis = 0.0f;

		// 衝突判定を行わないものはスキップ
		if (!gl.lock()->isCollision) continue;

		auto tempG = std::dynamic_pointer_cast<ColliderLine>(gl.lock());

		if (tempG->parent_.lock()->GetTransform().isHide) continue;

		// マップオブジェクト
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Line(tempM->GetModelId(), -1,
				tempG->GetPosA(), tempG->GetPosB());

			if (!info.HitFlag) continue;

			tempG->parent_.lock()->OnHit(info.Normal, info.HitPosition);
			isHit = true;
			break;
		}

		if (isHit) continue;

		// 破壊不能壁
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Line(tempMU->GetModelId(), -1,
				tempG->GetPosA(), tempG->GetPosB());

			if (!info.HitFlag) continue;

			tempG->parent_.lock()->OnHit(info.Normal, info.HitPosition);
			isHit = true;
			break;
		}
	}

	if (!isHit) return;

	for (auto gs : objsGS)
	{
		// 衝突判定を行わないものはスキップ
		if (gs.lock() == nullptr) continue;
		if (!gs.lock()->isCollision) continue;

		auto tempG = std::dynamic_pointer_cast<ColliderSphere>(gs.lock());

		if (tempG->parent_.lock()->GetTransform().isHide) continue;

		// マップオブジェクト
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Sphere(tempM->GetModelId(), -1,
				tempG->pos, tempG->GetRadius());
			if (info.HitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				if (tempG->parent_.lock()->GetTransform().isHide) break;

				auto hit = info.Dim[i];
				
				// 衝突後処理
				tempG->parent_.lock()->OnHit(hit);
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}

		// 破壊不能壁
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Sphere(tempMU->GetModelId(), -1,
				tempG->pos, tempG->GetRadius());
			if (info.HitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				if (tempG->parent_.lock()->GetTransform().isHide) break;

				auto hit = info.Dim[i];

				// 衝突後処理
				tempG->parent_.lock()->OnHit(hit);
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}
	}




}

void CollisionManager::CollisionExplosionMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::EXPLOSION) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0) return;

	auto& objsEP = colliders_.at(Collider::OBJECT_TYPE::EXPLOSION);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);

	for (auto ep : objsEP)
	{
		if (!ep.lock()->isCollision)continue;

		auto tempEP = std::dynamic_pointer_cast<ColliderSphere>(ep.lock());

		// マップオブジェクト
		for (auto m : objsM)
		{
			if (!m.lock()->isCollision)continue;

			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Sphere(tempM->GetModelId(), -1,
				tempEP->pos, tempEP->GetRadius());
			if (info.HitNum <= 0)
			{
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				auto hit = info.Dim[i];

				// 衝突したポリゴンが含まれるフレームを非表示にする
				m.lock()->parent_.lock()->OnHit(hit.FrameIndex);
			}

			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);
		}
	}
}

void CollisionManager::CollisionExplosionPlayer(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::EXPLOSION) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::PLAYER_CAP) <= 0) return;

	auto& objsEP = colliders_.at(Collider::OBJECT_TYPE::EXPLOSION);
	auto& objsP = colliders_.at(Collider::OBJECT_TYPE::PLAYER_CAP);

	for (auto ep : objsEP)
	{
		if (!ep.lock()->isCollision)continue;
		auto tempEP = std::dynamic_pointer_cast<ColliderSphere>(ep.lock());

		for (auto p : objsP)
		{
			if (!p.lock()->isCollision)continue;

			auto tempP = std::dynamic_pointer_cast<ColliderCapsule>(p.lock());

			auto info = HitCheck_Sphere_Capsule(tempEP->pos, tempEP->GetRadius(),
				tempP->GetPosA(), tempP->GetPosB(), tempP->GetRadius());
			if (!info) continue;
			
			p.lock()->parent_.lock()->OnHit(p, ep);
		}
	}
}

void CollisionManager::CollisionSmokePlayer(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::SMOKE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::PLAYER_CAP) <= 0) return;

	auto& objsSP = colliders_.at(Collider::OBJECT_TYPE::SMOKE);
	auto& objsP = colliders_.at(Collider::OBJECT_TYPE::PLAYER_CAP);

	for (auto sp : objsSP)
	{
		if (!sp.lock()->isCollision)continue;
		auto tempEP = std::dynamic_pointer_cast<ColliderSphere>(sp.lock());

		// マップオブジェクト
		for (auto p : objsP)
		{
			if (!p.lock()->isCollision)continue;

			auto tempP = std::dynamic_pointer_cast<ColliderCapsule>(p.lock());

			auto info = HitCheck_Sphere_Capsule(tempEP->pos, tempEP->GetRadius(),
				tempP->GetPosA(), tempP->GetPosB(), tempP->GetRadius());
			if (!info) continue;

			p.lock()->parent_.lock()->OnHit(p, sp);
		}
	}
}

void CollisionManager::CollisionExplosionEnemy(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::EXPLOSION) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::ENEMY) <= 0) return;

	auto& objsEP = colliders_.at(Collider::OBJECT_TYPE::EXPLOSION);
	auto& objsE = colliders_.at(Collider::OBJECT_TYPE::ENEMY);

	for (auto ep : objsEP)
	{
		if (!ep.lock()->isCollision)continue;
		auto tempEP = std::dynamic_pointer_cast<ColliderSphere>(ep.lock());

		// エネミー
		for (auto e : objsE)
		{
			if (!e.lock()->isCollision)continue;

			auto tempE = std::dynamic_pointer_cast<ColliderSphere>(e.lock());

			auto info = HitCheck_Sphere_Sphere(tempEP->pos, tempEP->GetRadius(),
				tempE->pos, tempE->GetRadius());
			if (!info) continue;

			e.lock()->parent_.lock()->OnHit(ep);
		}
	}
}

void CollisionManager::CollisionExplosionShield(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::EXPLOSION) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::SHIELD) <= 0) return;

	auto& objsEP = colliders_.at(Collider::OBJECT_TYPE::EXPLOSION);
	auto& objsS = colliders_.at(Collider::OBJECT_TYPE::SHIELD);

	for (auto ep : objsEP)
	{
		if (!ep.lock()->isCollision)continue;
		auto tempEP = std::dynamic_pointer_cast<ColliderSphere>(ep.lock());

		// シールド
		for (auto s : objsS)
		{
			if (!s.lock()->isCollision)continue;

			auto tempS = std::dynamic_pointer_cast<ColliderCapsule>(s.lock());

			auto info = HitCheck_Sphere_Capsule(tempEP->pos, tempEP->GetRadius(),
				tempS->GetPosA(), tempS->GetPosB(), tempS->GetRadius());
			if (!info) continue;

			s.lock()->parent_.lock()->OnHit(ep);
		}
	}
}

bool CollisionManager::CollisionRayMap(std::weak_ptr<Collider> obj)
{
	if (colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return false;

	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);


	auto tempR = std::dynamic_pointer_cast<ColliderLine>(obj.lock());

	MV1_COLL_RESULT_POLY hit = {};
	bool isHit = false;

	// マップオブジェクト
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
			tempR->GetPosA(), tempR->GetPosB());
		if (info.HitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			auto hit = HitCheck_Line_Triangle(tempR->GetPosA(), tempR->GetPosB(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (hit.HitFlag){
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				return true;
			}
		}

		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}


	// マップオブジェクト
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
			tempR->GetPosA(), tempR->GetPosB());
		if (info.HitNum <= 0)
		{
			// 当たり判定結果ポリゴン配列の後始末をする
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			auto hit = HitCheck_Line_Triangle(tempR->GetPosA(), tempR->GetPosB(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (hit.HitFlag){
				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);

				return true;
			}
		}
		// 当たり判定結果ポリゴン配列の後始末をする
		MV1CollResultPolyDimTerminate(info);
	}

	return false;
}

VECTOR CollisionManager::GetGunImpactPos(void)
{
	VECTOR nea = {};

	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_EYE) <= 0) return nea;

	auto& objsPE = colliders_.at(Collider::OBJECT_TYPE::PLAYER_EYE);

	bool isHit = false;
	
	for (auto pe : objsPE)
	{
		auto tempC = std::dynamic_pointer_cast<ColliderLine>(pe.lock());

		// マップーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
		
		if (colliders_.count(Collider::OBJECT_TYPE::MAP) > 0 &&
			colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) > 0)
		{
			auto objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
			auto objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

			// マップオブジェクト
			for (auto m : objsM)
			{
				auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

				auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
					tempC->GetPosA(), tempC->GetPosB());
				if (info.HitNum <= 0)
				{
					// 当たり判定結果ポリゴン配列の後始末をする
					MV1CollResultPolyDimTerminate(info);

					continue;
				}

				for (int i = 0; i < info.HitNum; i++)
				{
					// 大きさが0の場合処理を飛ばす
					auto scl = tempM->GetScaleFrame(info.Dim[i].FrameIndex);
					if (Utility::EqualsVZero(scl)) continue;

					if (!isHit)
					{
						nea = info.Dim[i].HitPosition;
						isHit = true;
						continue;
					}

					// retとプレイヤーの距離
					float disA = Utility::Distance(tempC->GetPosA(), nea);

					// info.Dim[i].HitPositionとプレイヤーの距離
					float disB = Utility::Distance(tempC->GetPosA(), info.Dim[i].HitPosition);

					if (disA > disB)
					{
						// より近い方をneaに代入
						nea = info.Dim[i].HitPosition;
					}

				}

				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);
			}


			// マップオブジェクト
			for (auto mu : objsMU)
			{
				auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

				auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
					tempC->GetPosA(), tempC->GetPosB());
				if (info.HitNum <= 0)
				{
					// 当たり判定結果ポリゴン配列の後始末をする
					MV1CollResultPolyDimTerminate(info);

					continue;
				}

				for (int i = 0; i < info.HitNum; i++)
				{
					// 大きさが0の場合処理を飛ばす
					auto scl = tempMU->GetScaleFrame(info.Dim[i].FrameIndex);
					if (Utility::EqualsVZero(scl)) continue;

					if (!isHit)
					{
						nea = info.Dim[i].HitPosition;
						isHit = true;
						continue;
					}

					// retとプレイヤーの距離
					float disA = Utility::Distance(tempC->GetPosA(), nea);

					// info.Dim[i].HitPositionとプレイヤーの距離
					float disB = Utility::Distance(tempC->GetPosA(), info.Dim[i].HitPosition);

					if (disA > disB)
					{
						// より近い方をneaに代入
						nea = info.Dim[i].HitPosition;
					}
				}

				// 当たり判定結果ポリゴン配列の後始末をする
				MV1CollResultPolyDimTerminate(info);
			}
		}
		// ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

		// エネミーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

		//if (colliders_.count(Collider::OBJECT_TYPE::ENEMY) > 0)
		//{
		//	auto objsE = colliders_.at(Collider::OBJECT_TYPE::ENEMY);

		//	// マップオブジェクト
		//	for (auto e : objsE)
		//	{
		//		auto tempE = std::dynamic_pointer_cast<ColliderSphere>(e.lock());

		//		

		//		auto info = HitCheck_Line_Sphere(tempC->GetPosA(), tempC->GetPosB(),
		//			)
		//		if (info.HitNum <= 0)
		//		{
		//			// 当たり判定結果ポリゴン配列の後始末をする
		//			MV1CollResultPolyDimTerminate(info);

		//			continue;
		//		}

		//		for (int i = 0; i < info.HitNum; i++)
		//		{
		//			if (!isHit)
		//			{
		//				nea = info.Dim[i].HitPosition;
		//				isHit = true;
		//				continue;
		//			}

		//			// retとプレイヤーの距離
		//			float disA = Utility::Distance(tempC->GetPosA(), nea);

		//			// info.Dim[i].HitPositionとプレイヤーの距離
		//			float disB = Utility::Distance(tempC->GetPosA(), info.Dim[i].HitPosition);

		//			if (disA > disB)
		//			{
		//				// より近い方をneaに代入
		//				nea = info.Dim[i].HitPosition;
		//			}

		//		}

		//		// 当たり判定結果ポリゴン配列の後始末をする
		//		MV1CollResultPolyDimTerminate(info);
		//	}
		//}
		// ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

		if (!isHit) return tempC->GetPosB();

		return nea;
	}


	return nea;
}

CollisionManager::CollisionManager(void)
{
}

CollisionManager::CollisionManager(const CollisionManager&)
{
}
