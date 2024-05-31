#include <DxLib.h>
#include <algorithm>
#include "../Application.h"
#include "Controller/PlayerController.h"
#include "../Net/NetManager.h"
#include "GameManager.h"
#include "SceneManager.h"
#include "InputManager.h"


InputManager* InputManager::instance_ = nullptr;

void InputManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new InputManager();
	}
	instance_->Init();
}

InputManager& InputManager::GetInstance(void)
{
	if (instance_ == nullptr)
	{
		InputManager::CreateInstance();
	}
	return *instance_;
}

void InputManager::Init(void)
{

	// �Q�[���Ŏg�p�������L�[���A
	// ���O�ɂ����œo�^���Ă����Ă�������
	InputManager::GetInstance().Add(KEY_INPUT_ESCAPE);
	InputManager::GetInstance().Add(KEY_INPUT_SPACE);
	InputManager::GetInstance().Add(KEY_INPUT_N);
	InputManager::GetInstance().Add(KEY_INPUT_Z);
	InputManager::GetInstance().Add(KEY_INPUT_B);

	InputManager::GetInstance().Add(KEY_INPUT_W);
	InputManager::GetInstance().Add(KEY_INPUT_A);
	InputManager::GetInstance().Add(KEY_INPUT_S);
	InputManager::GetInstance().Add(KEY_INPUT_D);
	InputManager::GetInstance().Add(KEY_INPUT_Q);
	InputManager::GetInstance().Add(KEY_INPUT_E);
	InputManager::GetInstance().Add(KEY_INPUT_R);

	InputManager::GetInstance().Add(KEY_INPUT_LEFT);
	InputManager::GetInstance().Add(KEY_INPUT_RIGHT);
	InputManager::GetInstance().Add(KEY_INPUT_UP);
	InputManager::GetInstance().Add(KEY_INPUT_DOWN);

	// �f�o�b�O�p
	InputManager::GetInstance().Add(KEY_INPUT_LSHIFT);
	InputManager::GetInstance().Add(KEY_INPUT_LCONTROL);
	InputManager::GetInstance().Add(KEY_INPUT_G);
	InputManager::GetInstance().Add(KEY_INPUT_P);
	InputManager::GetInstance().Add(KEY_INPUT_RETURN);
	InputManager::GetInstance().Add(KEY_INPUT_H);
	InputManager::GetInstance().Add(KEY_INPUT_C);
	InputManager::GetInstance().Add(KEY_INPUT_0);

	

	InputManager::MouseInfo info;

	// ���N���b�N
	info = InputManager::MouseInfo();
	info.key = MOUSE_INPUT_LEFT;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	mouseInfos_.emplace(info.key, info);

	// �E�N���b�N
	info = InputManager::MouseInfo();
	info.key = MOUSE_INPUT_RIGHT;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	mouseInfos_.emplace(info.key, info);

}

void InputManager::Update(void)
{

	// �L�[�{�[�h���m
	for (auto& p : keyInfos_)
	{
		p.second.keyOld = p.second.keyNew;
		p.second.keyNew = CheckHitKey(p.second.key);
		p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
		p.second.keyTrgUp = !p.second.keyNew && p.second.keyOld;
	}

	// �}�E�X���m
	mouseInput_ = GetMouseInput();
	GetMousePoint(&mousePos_.x, &mousePos_.y);

	for (auto& p : mouseInfos_)
	{
		p.second.keyOld = p.second.keyNew;
		p.second.keyNew = mouseInput_ == p.second.key;
		p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
		p.second.keyTrgUp = !p.second.keyNew && p.second.keyOld;
	}

	// �p�b�h���
	SetJPadInState(JOYPAD_NO::KEY_PAD1);
	SetJPadInState(JOYPAD_NO::PAD1);
	SetJPadInState(JOYPAD_NO::PAD2);
	SetJPadInState(JOYPAD_NO::PAD3);
	SetJPadInState(JOYPAD_NO::PAD4);



	if (!SceneManager::GetInstance().IsGameScene()) return;

	NetManager::GetInstance().ResetAction();


	if (!GameManager::GetInstance().IsStart() ||
		!GameManager::GetInstance().IsEndGame())
	{
		// �}�E�X�ʒu�𒆐S�ɃZ�b�g
		Vector2 center = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };
		SetMousePoint(center.x, center.y);
		return;
	}

	// �R���g���[���[�N���X�X�V
	for (auto con : controller_) {
		if (con == nullptr) continue;
		con->Update();
	}

	if (!GameManager::GetInstance().IsActive())return;

	// �}�E�X�ʒu�𒆐S�ɃZ�b�g
	Vector2 center = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };
	SetMousePoint(center.x, center.y);
}

void InputManager::NetUpdate(void)
{
	// �R���g���[���[�N���X�X�V
	for (auto con : controller_) {
		if (con == nullptr) continue;
		con->NetUpdate();
	}
}

void InputManager::Destroy(void)
{
	keyInfos_.clear();
	mouseInfos_.clear();
	delete instance_;
}

void InputManager::Add(int key)
{
	InputManager::Info info = InputManager::Info();
	info.key = key;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	keyInfos_.emplace(key, info);
}

void InputManager::Clear(void)
{
	keyInfos_.clear();
}

bool InputManager::IsNew(int key) const
{
	return Find(key).keyNew;
}

bool InputManager::IsTrgDown(int key) const
{
	return Find(key).keyTrgDown;
}

bool InputManager::IsTrgUp(int key) const
{
	return Find(key).keyTrgUp;
}

Vector2<int> InputManager::GetMousePos(void) const
{
	return mousePos_;
}

int InputManager::GetMouse(void) const
{
	return mouseInput_;
}

bool InputManager::IsClickMouseLeft(void) const
{
	return mouseInput_ & MOUSE_INPUT_LEFT;
}

bool InputManager::IsClickMouseRight(void) const
{
	return mouseInput_ & MOUSE_INPUT_RIGHT;
}

bool InputManager::IsTrgMouseLeft(void) const
{
	return FindMouse(MOUSE_INPUT_LEFT).keyTrgDown;
}

bool InputManager::IsTrgMouseRight(void) const
{
	return FindMouse(MOUSE_INPUT_RIGHT).keyTrgDown;
}

InputManager::InputManager(void)
{
	mouseInput_ = -1;
}

InputManager::InputManager(const InputManager& manager)
{
}

const InputManager::Info& InputManager::Find(int key) const
{

	auto it = keyInfos_.find(key);
	if (it != keyInfos_.end())
	{
		return it->second;
	}

	return infoEmpty_;

}

const InputManager::MouseInfo& InputManager::FindMouse(int key) const
{
	auto it = mouseInfos_.find(key);
	if (it != mouseInfos_.end())
	{
		return it->second;
	}

	return mouseInfoEmpty_;
}

InputManager::JOYPAD_TYPE InputManager::GetJPadType(JOYPAD_NO no)
{
	return static_cast<InputManager::JOYPAD_TYPE>(GetJoypadType(static_cast<int>(no)));
}

DINPUT_JOYSTATE InputManager::GetJPadDInputState(JOYPAD_NO no)
{
	// �R���g���[�����
	GetJoypadDirectInputState(static_cast<int>(no), &joyDInState_);
	return joyDInState_;
}

XINPUT_STATE InputManager::GetJPadXInputState(JOYPAD_NO no)
{
	// �R���g���[�����
	GetJoypadXInputState(static_cast<int>(no), &joyXInState_);
	return joyXInState_;
}

void InputManager::SetJPadInState(JOYPAD_NO jpNo)
{

	int no = static_cast<int>(jpNo);
	auto stateNew = GetJPadInputState(jpNo);
	auto& stateNow = padInfos_[no];

	int max = static_cast<int>(JOYPAD_BTN::MAX);
	for (int i = 0; i < max; i++)
	{

		stateNow.ButtonsOld[i] = stateNow.ButtonsNew[i];
		stateNow.ButtonsNew[i] = stateNew.ButtonsNew[i];

		stateNow.IsOld[i] = stateNow.IsNew[i];
		//stateNow.IsNew[i] = stateNow.ButtonsNew[i] == 128 || stateNow.ButtonsNew[i] == 255;
		stateNow.IsNew[i] = stateNow.ButtonsNew[i] > 0;

		stateNow.IsTrgDown[i] = stateNow.IsNew[i] && !stateNow.IsOld[i];
		stateNow.IsTrgUp[i] = !stateNow.IsNew[i] && stateNow.IsOld[i];


		stateNow.AKeyLX = stateNew.AKeyLX;
		stateNow.AKeyLY = stateNew.AKeyLY;
		stateNow.AKeyRX = stateNew.AKeyRX;
		stateNow.AKeyRY = stateNew.AKeyRY;

	}

}

InputManager::JOYPAD_IN_STATE InputManager::GetJPadInputState(JOYPAD_NO no)
{

	JOYPAD_IN_STATE ret = JOYPAD_IN_STATE();

	auto type = GetJPadType(no);
	
	switch (type)
	{
	case InputManager::JOYPAD_TYPE::OTHER:
		break;
	case InputManager::JOYPAD_TYPE::XBOX_360:
	{
	}
		break;
	case InputManager::JOYPAD_TYPE::XBOX_ONE:
	{

		auto d = GetJPadDInputState(no);
		auto x = GetJPadXInputState(no);

		int idx;

		//   Y
		// X   B
		//   A

		idx = static_cast<int>(JOYPAD_BTN::TOP);
		ret.ButtonsNew[idx] = d.Buttons[3];// Y

		idx = static_cast<int>(JOYPAD_BTN::LEFT);
		ret.ButtonsNew[idx] = d.Buttons[2];// X

		idx = static_cast<int>(JOYPAD_BTN::RIGHT);
		ret.ButtonsNew[idx] = d.Buttons[1];// B

		idx = static_cast<int>(JOYPAD_BTN::DOWN);
		ret.ButtonsNew[idx] = d.Buttons[0];// A

		idx = static_cast<int>(JOYPAD_BTN::R_TRIGGER);
		ret.ButtonsNew[idx] = x.RightTrigger;// R_TRIGGER

		idx = static_cast<int>(JOYPAD_BTN::L_TRIGGER);
		ret.ButtonsNew[idx] = x.LeftTrigger; // L_TRIGGER

		// ���X�e�B�b�N
		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;
		
		// �E�X�e�B�b�N
		ret.AKeyRX = d.Rx;
		ret.AKeyRY = d.Ry;

	}
		break;
	case InputManager::JOYPAD_TYPE::DUAL_SHOCK_4:
		break;
	case InputManager::JOYPAD_TYPE::DUAL_SENSE:
	{
		
		auto d = GetJPadDInputState(no);
		int idx;

		//   ��
		// ��  �Z
		//   �~

		idx = static_cast<int>(JOYPAD_BTN::TOP);
		ret.ButtonsNew[idx] = d.Buttons[3];// ��

		idx = static_cast<int>(JOYPAD_BTN::LEFT);
		ret.ButtonsNew[idx] = d.Buttons[0];// ��

		idx = static_cast<int>(JOYPAD_BTN::RIGHT);
		ret.ButtonsNew[idx] = d.Buttons[2];// �Z

		idx = static_cast<int>(JOYPAD_BTN::DOWN);
		ret.ButtonsNew[idx] = d.Buttons[1];// �~

		// ���X�e�B�b�N
		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;
		
		// �E�X�e�B�b�N
		ret.AKeyRX = d.Z;
		ret.AKeyRY = d.Rz;

	}
		break;
	case InputManager::JOYPAD_TYPE::SWITCH_JOY_CON_L:
		break;
	case InputManager::JOYPAD_TYPE::SWITCH_JOY_CON_R:
		break;
	case InputManager::JOYPAD_TYPE::SWITCH_PRO_CTRL:
		break;
	case InputManager::JOYPAD_TYPE::MAX:
		break;
	}

	return ret;

}

bool InputManager::IsPadBtnNew(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	return padInfos_[static_cast<int>(no)].IsNew[static_cast<int>(btn)];
}

bool InputManager::IsPadBtnTrgDown(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	return padInfos_[static_cast<int>(no)].IsTrgDown[static_cast<int>(btn)];
}

bool InputManager::IsPadBtnTrgUp(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	return padInfos_[static_cast<int>(no)].IsTrgUp[static_cast<int>(btn)];
}

PlayerController& InputManager::GetController(int no)
{
	return *controller_[no];
}

std::weak_ptr<PlayerController> InputManager::CreateController(int no, int key)
{
	std::shared_ptr<PlayerController> temp = std::make_shared<PlayerController>(key);
	controller_[no] = temp;
	return controller_[no];
}

void InputManager::ClearController(void)
{
	for (auto con : controller_) {
		con = nullptr;
	}
}



