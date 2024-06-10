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

	/// @brief 描画する画像を返す 
	/// @return 画像
	const Weak_Graphic GetGraphic(void) const;

	/// @brief 描画する画像をセットする
	/// @param graphic 画像
	void SetGraphic(Weak_Graphic graphic);

	/// @brief 使用するピクセルシェーダーを返す 
	/// @return ピクセルシェーダー
	Weak_PixelShader GetUsingPixelShader(void) const;

private:
	// 強調表示状態で使用するスケール値
	static constexpr float HIGHLIGHTED_SCALE = 1.5f;

	// 画像
	Weak_Graphic graphic_;

	void HighlightedUpdate(void) override;
};

