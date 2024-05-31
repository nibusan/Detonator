#pragma once
#include <memory>
#include <functional>
#include "UIBase.h"
#include "../Data/UI/UILayerList.h"

class Collision2D;
class UIUtility {
public:
	/// @brief UI�̋��ʕ�����������
	/// @param ui �Ώۂ�UI
	/// @param pos ���W
	/// @param size UI�̃T�C�Y
	/// @param collision 2D�̓����蔻��
	/// @param positionType �ǂ�����ɕ`�悷�邩
	/// @param onClick �N���b�N���ꂽ�ۂɌĂяo�����֐�
	/// @param isActive �\�����邩
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

