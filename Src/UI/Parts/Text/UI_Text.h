#pragma once
#include <string>
#include "../../../Common/Handle/Font/Font.h"
#include "../../UIBase.h"

class UI_Text : public UIBase {
public:
	UI_Text() = default;
	~UI_Text() = default;

	/// @brief �R���X�g���N�^
	/// @param text �`�悷��e�L�X�g
	/// @param name �g�p����t�H���g�̖��O
	/// @param size �t�H���g�̃T�C�Y
	/// @param color �e�L�X�g�̐F
	UI_Text(std::string text, std::string name, int size, unsigned int color);

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;

	/// @brief �`�悷��e�L�X�g��Ԃ� 
	/// @return �e�L�X�g
	const std::string& GetText(void) const;

	/// @brief �`�悷��e�L�X�g���Z�b�g����
	/// @param text �e�L�X�g
	void SetText(const std::string& text);

	/// @brief �`�悷��ۂɎg�p����t�H���g�𐶐�����
	/// @param name �t�H���g�̖��O
	/// @param size �t�H���g�̃T�C�Y
	void CreateFont(std::string name, int size);

	/// @brief �`�悷��ۂɎg�p����t�H���g��Ԃ�
	/// @return �t�H���g
	const Weak_Font GetFont(void) const;

private:
	// �e�L�X�g
	std::string text_;

	// �g�p����t�H���g
	Shared_Font usingFont_;

	// �e�L�X�g�̐F
	unsigned int color_;

	std::function<std::string(void)> updateText_;
};

