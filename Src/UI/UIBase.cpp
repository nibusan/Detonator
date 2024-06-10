#include "../Manager/SceneManager.h"
#include "../Manager/Collision2DManager.h"
#include "../Common/Collision2D/Collision2D.h"
#include "../Common/Collision2D/Collision2D_Point.h"
#include "../Common/Collision2D/Collision2D_Rect.h"
#include "../Common/Collision2D/Collision2D_Circle.h"
#include "../Common/Collision2D/Collision2D_Polygon.h"
#include "UIBase.h"

Vector2<float> UIBase::GetDrawPos(const Vector2<float>& pos, const Vector2<float>& size, DRAW_POSITION_TYPE type) const {
	// �߂�l�i�[�p
	Vector2<float> ret;

	switch (type) {
	case UIBase::DRAW_POSITION_TYPE::UP_LEFT:
		ret = pos + size / 2.0f;
		break;

	case UIBase::DRAW_POSITION_TYPE::UP_CENTER:
		ret = {
			pos.x,
			pos.y + size.y / 2.0f
		};
		break;

	case UIBase::DRAW_POSITION_TYPE::UP_RIGHT:
		ret = {
			pos.x - size.x / 2.0f,
			pos.y + size.y / 2.0f
		};
		break;

	case UIBase::DRAW_POSITION_TYPE::CENTER_LEFT:
		ret = {
			pos.x + size.x / 2.0f,
			pos.y
		};
		break;

	case UIBase::DRAW_POSITION_TYPE::CENTER_CENTER:
		ret = pos;
		break;

	case UIBase::DRAW_POSITION_TYPE::CENTER_RIGHT:
		ret = {
			pos.x - size.x / 2.0f,
			pos.y
		};
		break;

	case UIBase::DRAW_POSITION_TYPE::DOWN_LEFT:
		ret = {
			pos.x + size.x / 2.0f,
			pos.y - size.y / 2.0f
		};
		break;

	case UIBase::DRAW_POSITION_TYPE::DOWN_CENTER:
		ret = {
			pos.x,
			pos.y - size.y / 2.0f
		};
		break;

	case UIBase::DRAW_POSITION_TYPE::DOWN_RIGHT:
		ret = {
			pos.x - size.x / 2.0f,
			pos.y - size.y / 2.0f
		};
		break;

	default:
		ret = pos;
		break;
	}

	return ret;
}

void UIBase::HighlightedUpdate(void) {

}

UIBase::UIBase(void) :
frameCNT_(0),
type_(UI::TYPE::IMAGE),
pos_{},
scale_(0.0f),
angle_(0.0f),
alpha_(1.0f),
isClick_(false),
isClickable_(false),
fadeState_(FADE_STATE::NONE),
fadeSpeed_(0.0f),
onClickFunction_(nullptr),
isActive_(false),
isHighlighted_(false) {

}

void UIBase::Init(void) {
	//�eUI�̏�����
	Init_UI();

}

bool UIBase::Update(void) {
	// �����A�N�e�B�u����Ȃ������牽�����Ȃ�
	if (!IsActive()) return false;

	auto& sceneManager = SceneManager::GetInstance();

	// �R���C�_�[�̍X�V
	auto&& drawPos = GetDrawPos(totalPos_, drawScreen_->GetSize().ToVector2f(), GetDrawPositionType());
	collision_->SetPos(drawPos);

	// UI�̍X�V�����ƂƂ��ɃN���b�N����Ă邩���Z�b�g����
	SetIsClick(Update_UI());

	// �����\����Ԃ��������p�̍X�V�������s��
	if (IsHighlighted()) HighlightedUpdate();

	// �N���b�N����ĂȂ����A�N�V�����֐����Z�b�g����Ă����炻�̊֐����Ăяo��
	if (IsClick()) {
		if (onClickFunction_ != nullptr) {
			onClickFunction_();
		}
	}

	// ���W�̍X�V
	if (parent_.lock() != nullptr) {
		totalPos_ = GetTotalPos();
	}

	// �t�F�[�h�̍X�V
	switch (fadeState_) {
	case UIBase::FADE_STATE::FADE_IN:
		alpha_ += sceneManager.GetDeltaTime() * fadeSpeed_;
		if (alpha_ > 1.0f) {
			alpha_ = 1.0f;
			fadeState_ = FADE_STATE::NONE;
		}
		break;

	case UIBase::FADE_STATE::FADE_OUT:
		alpha_ -= sceneManager.GetDeltaTime() * fadeSpeed_;
		if (alpha_ < 0.0f) {
			alpha_ = 0.0f;
			fadeState_ = FADE_STATE::NONE;
		}
		break;

	case UIBase::FADE_STATE::NONE:
	default:
		break;
	}

	frameCNT_++;

	return IsClick();
}

void UIBase::Draw(void) {
	// �����A�N�e�B�u����Ȃ������牽�����Ȃ�
	if (!IsActive()) return;

	int defScreen = GetDrawScreen();
	SetDrawScreen(drawScreen_->GetHandle());
	ClearDrawScreen();
	// UI���̂��̂̕`��
	Draw_UI();
	SetDrawScreen(defScreen);

	// �`�悷����W���v�Z
	Vector2<float> drawPos = GetDrawPos(totalPos_, drawScreen_->GetSize().ToVector2f(), GetDrawPositionType());

	if (GetAlpha() == 1.0f) {
		// alpha��1.0�̏ꍇ�̓u�����h���[�h�͎g��Ȃ�
		drawScreen_->Draw(drawPos, scale_, angle_, nullptr);
	}
	else {
		// ���S�ɓ�����������`�悵�Ȃ�
		if (GetAlpha() != 0.0f) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_ * 255));
			drawScreen_->Draw(drawPos, scale_, angle_, nullptr);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

	//DrawBox(totalPos_.x, totalPos_.y, totalPos_.x + drawScreen_->GetSize().x, totalPos_.y + drawScreen_->GetSize().y, 0xFF0000, false);
}

void UIBase::Release(void) {

}

void UIBase::CreateDrawScreen(Vector2<int> size) {
	drawScreen_ = std::make_unique<Graphic>(MakeScreen(size.x, size.y, true));
}

const Vector2<float>& UIBase::GetPos(void) const {
	return pos_;
}

Vector2<float> UIBase::GetTotalPos(void) const {
	// �e�̍��W�Ȃǂ�S�čl�����Čv�Z����
	Vector2<float> totalPos = {};
	if (parent_.lock() != nullptr) {
		totalPos = totalPos + parent_.lock()->GetTotalPos();
		totalPos = totalPos + pos_;
	}
	else {
		totalPos = pos_;
	}

	return totalPos;
}

void UIBase::SetPos(const Vector2<float>& pos) {
	pos_ = pos;

	// �e�̍��W�Ȃǂ�S�čl�����Čv�Z����
	Vector2<float> totalPos = {};
	if (parent_.lock() != nullptr) {
		totalPos = totalPos + parent_.lock()->GetTotalPos();
	}
	totalPos_ = totalPos + pos_;
}

void UIBase::SetTotalPos(const Vector2<float>& pos) {

}

const Vector2<int>& UIBase::GetSize(void) const {
	return drawScreen_->GetSize();
}

UI::TYPE UIBase::GetType(void) const {
	return type_;
}

float UIBase::GetScale(void) const {
	return scale_;
}

void UIBase::SetScale(float scale) {
	scale_ = scale;
}

float UIBase::GetAngle(void) const {
	return angle_;
}

void UIBase::SetAngle(float rad) {
	angle_ = rad;
}

float UIBase::GetAlpha(void) const {
	return alpha_;
}

void UIBase::SetAlpha(float alpha) {
	alpha_ = alpha;
}

bool UIBase::IsClick(void) const {
	return isClick_;
}

void UIBase::SetIsClick(bool isClick) {
	isClick_ = isClick;
}

bool UIBase::IsClickable(void) const {
	return isClickable_;
}

void UIBase::SetIsClickable(bool isClickable) {
	isClickable_ = isClickable;
}

void UIBase::SetUsePixelShader(Weak_PixelShader ps) {
	usingPixelShader_ = ps;
}

void UIBase::SetFade(FADE_STATE state, float speed) {
	// �����ꍇ�͉������Ȃ�
	if (fadeState_ == state) return;

	fadeState_ = state;
	fadeSpeed_ = speed;

	switch (fadeState_) {
	case UIBase::FADE_STATE::FADE_IN:
		alpha_ = 0.0f;
		break;

	case UIBase::FADE_STATE::FADE_OUT:
		alpha_ = 1.0f;
		break;

	case UIBase::FADE_STATE::NONE:
	default:
		break;
	}
}

void UIBase::SetOnClickFunction(std::function<void(void)> func) {
	onClickFunction_ = func;
}

void UIBase::OnClick(void) {
	onClickFunction_();
}

UIBase::DRAW_POSITION_TYPE UIBase::GetDrawPositionType(void) const {
	return drawPositionType_;
}

void UIBase::SetDrawPositionType(DRAW_POSITION_TYPE type) {
	drawPositionType_ = type;
}

Shared_Collision2D UIBase::GetCollisionData(void) const {
	return collision_;
}

void UIBase::CreateCollisionData(DRAW_POSITION_TYPE posType, Shared_Collision2D collision) {
	collision_ = collision;
}

const std::vector<std::shared_ptr<UIBase>> UIBase::GetChilds(void) const {
	return childs_;
}

void UIBase::AddChild(std::shared_ptr<UIBase> child) {
	child->SetParent(shared_from_this());
	childs_.emplace_back(child);
}

const std::weak_ptr<UIBase> UIBase::GetParent(void) const {
	return parent_;
}

void UIBase::SetParent(std::weak_ptr<UIBase> parent) {
	parent_ = parent;
}

bool UIBase::IsActive(void) const {
	return isActive_;
}

void UIBase::SetIsActive(bool flag) {
	isActive_ = flag;
}

bool UIBase::IsHighlighted(void) const {
	return isHighlighted_;
}

void UIBase::SetIsHighlighted(bool flag) {
	isHighlighted_ = flag;
}
