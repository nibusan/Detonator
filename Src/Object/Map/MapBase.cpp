#include "string"
#include "../../Manager/CollisionManager.h"
#include "../../Manager/EffectManager.h"
#include "../../Manager/ResourceManager.h"
#include "../Common/ColliderModel.h"
#include "MapBase.h"

void MapBase::Init_Object(void)
{
	// ���ǂݍ���
	//transform_.SetModel(
	//	MV1LoadModel("Data/Model/Map/Collision.mv1")
	//);

	//// �ړ���]�傫���������f���ɔ��f
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
	//// �ړ���]�傫���������f���ɔ��f
	//transform_.Update();

	// �R���C�_�[�X�V
	for (auto m : map_)
	{
		auto col = std::dynamic_pointer_cast<ColliderModel>(m.collider_);
		col->Update();
	}

}

void MapBase::Draw_Object(void)
{
	// �}�b�v�`��
	map_[(int)TYPE::UNBREAK].transform_.Draw();
	map_[(int)TYPE::NORMAL].transform_.Draw();
}

void MapBase::Release_Object(void)
{
	// ���f���폜(��)
	transform_.Release();

	for (auto m : map_)
	{
		m.transform_.Release();
	}
}

void MapBase::OnHit(int no)
{

	auto m = std::dynamic_pointer_cast<ColliderModel>(map_[(int)TYPE::NORMAL].collider_);



	// �Փ˗p�}�b�v
	// �Փ˂����t���[��(�Փ˗p�}�b�v)���폜(�X�P�[����0��)
	m->HideFrame(no);

	// �Փ˂����t���[���̖��O���擾
	auto name = MV1GetFrameName(m->GetModelId(), no);

	std::string fNo = "";
	
	int size = _tcslen(name);
	for (int i = 1; i < size; i++)
	{
		fNo += name[i];
	}
	

	// �Փ˗p�t���[���̊e�t���[���̖��O��
	// �\���p�t���[���̃t���[���ԍ��ɑΉ����Ă���(���Ȃ��Ƃ����Ȃ��߂�ǂ�����)

	//// �Ώۃt���[���̈ʒu���擾����
	//auto fMat = MV1GetFrameLocalMatrix(map_[(int)TYPE::NORMAL].transform_.modelId, std::stoi(fNo));
	//auto fPos = MGetTranslateElem(fMat);	// �s�񂩂�ړ��l�����o��

	//EffectManager::EFFECT temp;
	//temp.pos = fPos;
	//VECTOR localRot = Quaternion::ToEuler(transform_.quaRot);
	//temp.rot = localRot;
	//temp.scl = { 40.0f,40.0f,40.0f };
	//temp.speed = 1.5f;
	//temp.type = EffectManager::EFFECT_TYPE::BREAK;
	//EffectManager::GetInstance().AddEffect(temp);

	// �Փ˂����t���[��(�\���p�}�b�v)���\����
	map_[(int)TYPE::NORMAL].transform_.HideFrame(std::stoi(fNo));


}

int MapBase::GetMapModel(TYPE type)
{
	return map_[(int)type].transform_.modelId;
}
