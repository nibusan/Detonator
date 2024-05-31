#pragma once
#include <memory>
#include <functional>
#include "UIBase.h"
#include "../Data/UI/UILayerList.h"

class Collision2D;
class UIUtility {
public:
	/// @brief UIの共通部分を初期化
	/// @param ui 対象のUI
	/// @param pos 座標
	/// @param size UIのサイズ
	/// @param collision 2Dの当たり判定
	/// @param positionType どこを基準に描画するか
	/// @param onClick クリックされた際に呼び出される関数
	/// @param isActive 表示するか
	static void InitBaseUI(
		std::shared_ptr<UIBase>& ui,
		Vector2<float> pos, 
		Vector2<int> size, 
		std::shared_ptr<Collision2D> collision,
		UIBase::DRAW_POSITION_TYPE positionType,
		std::function<void(void)> onClick,
		bool isActive
	);
};

