#pragma once
#include <string>
#include "../../../Common/Handle/Graphic/Graphic.h"
#include "../../UIBase.h"

class UI_Image: public UIBase {
public:
	UI_Image() = default;
	UI_Image(Weak_Graphic graphic);
	~UI_Image() = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;

	/// @brief �`�悷��摜��Ԃ� 
	/// @return �摜
	const Weak_Graphic GetGraphic(void) const;

	/// @brief �`�悷��摜���Z�b�g����
	/// @param graphic �摜
	void SetGraphic(Weak_Graphic graphic);

	/// @brief �g�p����s�N�Z���V�F�[�_�[��Ԃ� 
	/// @return �s�N�Z���V�F�[�_�[
	Weak_PixelShader GetUsingPixelShader(void) const;

private:
	// �����\����ԂŎg�p����X�P�[���l
	static constexpr float HIGHLIGHTED_SCALE = 1.5f;

	// �摜
	Weak_Graphic graphic_;

	void HighlightedUpdate(void) override;
};

