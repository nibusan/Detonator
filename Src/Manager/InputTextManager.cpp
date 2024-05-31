#include <DxLib.h>
#include "../Common/InputTextArea.h"
#include "InputTextManager.h"

InputTextManager* InputTextManager::instance_ = nullptr;

void InputTextManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new InputTextManager();
	}
}

InputTextManager& InputTextManager::GetInstance(void)
{
	return *instance_;
}

void InputTextManager::Update(void)
{

	if (instance_->inputTextArea_ == nullptr)
	{
		return;
	}
	
	// ���͂��I�����Ă��邩���f
	// �O�@�F�L�[���͂͂܂��������L�����Z��������Ă��Ȃ�
	// �P�@�F�L�[���͂͐���ɏI������
	// �Q�@�F�L�[���͂̓L�����Z�����ꂽ
	int handle = instance_->inputTextArea_->GetKeyHandle();
	int ret = CheckKeyInput(handle);
	if (ret != 0)
	{
		
		// ���͂��ꂽ��������Z�b�g
		instance_->inputTextArea_->SetKeyInputStringBuffer();
		
		// �L�[���͂��I�����Ă���͂�
		instance_->inputTextArea_ = nullptr;

	}

}

void InputTextManager::SetActive(InputTextArea* inputTextArea)
{
	// �쐬�����L�[���̓n���h�����A�N�e�B�u�ɂ���
	SetActiveKeyInput(inputTextArea->GetKeyHandle());
	instance_->inputTextArea_ = inputTextArea;
}

bool InputTextManager::IsActiveHandle(InputTextArea* inputTextArea)
{
	if (instance_->inputTextArea_ == nullptr)
	{
		return false;
	}
	// ���ݓ��͒��̃e�L�X�g�G���A�ƍ��v������true��Ԃ�
	return instance_->inputTextArea_ == inputTextArea;
}

InputTextManager::InputTextManager(void)
{
	inputTextArea_ = nullptr;
}

InputTextManager::InputTextManager(const InputTextManager& ins)
{
}

InputTextManager::~InputTextManager(void)
{
}
