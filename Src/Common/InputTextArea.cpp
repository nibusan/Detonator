#include <DxLib.h>
#include "../Manager/InputTextManager.h"
#include "InputTextArea.h"

InputTextArea::InputTextArea(Vector2<int> pos, Vector2<int> size, int length)
	: manager_(InputTextManager::GetInstance())
{
	pos_ = pos;
	size_ = size;
	length_ = length;
	keyInputHandle_ = -1;

	// ���̓^�C�v�̓I�[�o�[���C�h���ĕς���Ɨǂ��ł�
	keyInputHandle_ = MakeKeyInput(length_, false, true, true);
	mouseInputOld_ = mouseInputNew_ = -1;
}

InputTextArea::~InputTextArea(void)
{
}

void InputTextArea::Update(void)
{

	// InputManager�̕������������ł����A
	// �����Ă������悤��DxLib���ڂ��g�p
	mouseInputNew_ = GetMouseInput();

	if (mouseInputNew_ == MOUSE_INPUT_LEFT
		&& mouseInputOld_ == 0)
	{

		Vector2<int> mousePos;
		GetMousePoint(&mousePos.x, &mousePos.y);

		int x1 = pos_.x;
		int y1 = pos_.y;
		int x2 = pos_.x + size_.x;
		int y2 = pos_.y + size_.y;
		if (mousePos.x >= x1 && mousePos.y >= y1
			&& mousePos.x <= x2 && mousePos.y <= y2)
		{
			// �쐬�����L�[���̓n���h�����A�N�e�B�u�ɂ���
			manager_.SetActive(this);
		}
	}
	
	mouseInputOld_ = mouseInputNew_;

}

void InputTextArea::Draw(void)
{

	if (manager_.IsActiveHandle(this))
	{
		DrawActive();
	}
	else
	{
		DrawDefault();
	}


}

void InputTextArea::Release(void)
{
	// �p�ς݂̃C���v�b�g�n���h�����폜����
	DeleteKeyInput(keyInputHandle_);
}

int InputTextArea::GetKeyHandle(void)
{
	return keyInputHandle_;
}

std::string InputTextArea::GetText(void)
{
	return text_;
}

void InputTextArea::SetText(std::string text)
{
	text_ = text;
}

void InputTextArea::SetKeyInputStringBuffer(void)
{
	// ���͓��e���擾
	GetKeyInputString(buffer_, keyInputHandle_);
	// char����string��
	text_ = std::string(buffer_, length_);
}

void InputTextArea::DrawDefault(void)
{

	// �w�i
	DrawBox(pos_.x, pos_.y,
		pos_.x + size_.x, pos_.y + size_.y, COLOR_BACK_DEFAULT, true);

	// �g
	DrawBox(pos_.x, pos_.y,
		pos_.x + size_.x, pos_.y + size_.y, COLOR_FRAME, false);

	// �����\��
	DrawString(pos_.x + 5, pos_.y + 7, text_.c_str(), COLOR_CHAR);

}

void InputTextArea::DrawActive(void)
{
	// �w�i
	DrawBox(pos_.x, pos_.y,
		pos_.x + size_.x, pos_.y + size_.y, COLOR_BACK_ACTIVE, true);

	// �g
	DrawBox(pos_.x, pos_.y,
		pos_.x + size_.x, pos_.y + size_.y, COLOR_FRAME, false);

	// ���͕���
	DrawKeyInputString(pos_.x + 5, pos_.y + 7, keyInputHandle_);

}
