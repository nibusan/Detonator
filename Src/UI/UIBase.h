#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "UIType.h"
#include "../Common/Vector2.h"
#include "../Common/Handle/Graphic/Graphic.h"
#include "../Common/Handle/PixelShader/PixelShader.h"
#include "../Common/Collision2D/Collision2D.h"
#include "../Common/Collision2D/Collision2D_Point.h"
#include "../Common/Collision2D/Collision2D_Rect.h"
#include "../Common/Collision2D/Collision2D_Circle.h"
#include "../Common/Collision2D/Collision2D_Polygon.h"

class UIBase : public std::enable_shared_from_this<UIBase> {
public:
	// UI�̃t�F�[�h���
	enum class FADE_STATE {
		FADE_IN,
		FADE_OUT,
		NONE
	};

	// �w�肳�ꂽ���W��UI�̂ǂ������_�Ƃ��Ĕz�u���邩�̎��
	enum class DRAW_POSITION_TYPE {
		UP_LEFT,		// ��i ��
		UP_CENTER,		// ��i ���S
		UP_RIGHT,		// ��i �E
		CENTER_LEFT,	// ���i ��
		CENTER_CENTER,	// ���i ���S
		CENTER_RIGHT,	// ���i �E
		DOWN_LEFT,		// ���i ��
		DOWN_CENTER,	// ���i ���S
		DOWN_RIGHT		// ���i �E
	};

protected:
	// �t���[���J�E���^
	int frameCNT_;

	// UI�̎��
	UI::TYPE type_;

	// ���W(�eUI���ݒ肳��Ă���ꍇ�A�e����̑��΍��W�Ƃ��Ĉ����̂Œ���)
	Vector2<float> pos_;

	// ��΍��W(�eUI�����݂���ꍇ�ɂ̂ݎg�p)
	Vector2<float> totalPos_;

	// �X�P�[���l
	float scale_;

	// �p�x
	float angle_;

	// UI�`��p�X�N���[��
	Unique_Graphic drawScreen_;

	// UI�`�掞�Ɏg�p����s�N�Z���V�F�[�_�[
	Weak_PixelShader usingPixelShader_;

	// �����x(0.0 ~ 1.0)
	float alpha_;

	// �N���b�N���ꂽ��
	bool isClick_;

	// �N���b�N�ł��邩
	bool isClickable_;

	// �t�F�[�h�̏��
	FADE_STATE fadeState_;

	// �t�F�[�h�̑��x
	float fadeSpeed_;

	// UI�������ꂽ�Ƃ��Ɏ��s����A�N�V����
	std::function<void(void)> onClickFunction_;

	// �ǂ������_�Ƃ��ĕ`�悷�邩
	DRAW_POSITION_TYPE drawPositionType_;

	// UI�̓����蔻��
	Shared_Collision2D collision_;

	// �qUI
	std::vector<std::shared_ptr<UIBase>> childs_;

	// �eUI
	std::weak_ptr<UIBase> parent_;

	// ��\�����ǂ���
	bool isActive_;

	// �����\������Ă��邩
	bool isHighlighted_;

	virtual void Init_UI(void) = 0;
	virtual bool Update_UI(void) = 0;
	virtual void Draw_UI(void) = 0;
	virtual void Release_UI(void) = 0;

	// �`�悷����W��Ԃ�
	Vector2<float> GetDrawPos(const Vector2<float>& pos, const Vector2<float>& size, DRAW_POSITION_TYPE type) const;

	/// @brief 
	/// @param  
	virtual void HighlightedUpdate(void);
public:
	// �R���X�g���N�^
	UIBase(void);

	// �f�X�g���N�^
	virtual ~UIBase(void) = default;

	void Init(void);
	bool Update(void);
	void Draw(void);
	void Release(void);

	/// @brief UI�`��p�̃X�N���[���𐶐�����
	/// @param size �X�N���[���̃T�C�Y
	/// @note Init���Ăяo������ɂ��̊֐����Ă�ł�������
	void CreateDrawScreen(Vector2<int> size);

	// UI�̍��W��Ԃ�
	[[nodiscard]] const Vector2<float>& GetPos(void) const;
	[[nodiscard]] Vector2<float> GetTotalPos(void) const;

	// UI�̍��W���Z�b�g
	void SetPos(const Vector2<float>& pos);
	void SetTotalPos(const Vector2<float>& pos);

	// UI�̃T�C�Y��Ԃ�
	[[nodiscard]] const Vector2<int>& GetSize(void) const;

	// UI�̎�ނ�Ԃ�
	[[nodiscard]] UI::TYPE GetType(void) const;

	// UI�̃X�P�[���l��Ԃ�
	[[nodiscard]] float GetScale(void) const;
	
	// UI�̃X�P�[���l���Z�b�g
	void SetScale(float scale);

	// UI�̊p�x��Ԃ�
	[[nodiscard]] float GetAngle(void) const;
	
	// UI�̊p�x���Z�b�g
	void SetAngle(float rad);

	// UI�̓����x��Ԃ�
	[[nodiscard]] float GetAlpha(void) const;
	
	// UI�̓����x���Z�b�g
	void SetAlpha(float alpha);

	// �N���b�N���ꂽ����Ԃ�
	[[nodiscard]] bool IsClick(void) const;

	// �N���b�N���ꂽ�����Z�b�g
	void SetIsClick(bool isClick);

	// ����UI���N���b�N�ł��邩��Ԃ�
	[[nodiscard]] bool IsClickable(void) const;

	// ����UI���N���b�N�ł��邩���Z�b�g
	void SetIsClickable(bool isClickable);

	// �g�p����s�N�Z���V�F�[�_�[���Z�b�g(�萔���X�V����ꍇ�͖��t���[���Ăяo���K�v����)
	void SetUsePixelShader(Weak_PixelShader ps);

	// �t�F�[�h��Ԃ��Z�b�g
	void SetFade(FADE_STATE state, float speed);

	// �N���b�N���ꂽ�Ƃ��ɌĂяo�����֐����Z�b�g
	void SetOnClickFunction(std::function<void(void)> func);

	// �N���b�N���ꂽ�Ƃ��ɌĂяo�����֐����Ă�
	void OnClick(void);

	// �`�掞�ɂǂ������_�Ƃ��ĕ`�悷�邩��Ԃ�
	[[nodiscard]] DRAW_POSITION_TYPE GetDrawPositionType(void) const;
	
	// �`�掞�ɂǂ������_�Ƃ��ĕ`�悷�邩���Z�b�g
	void SetDrawPositionType(DRAW_POSITION_TYPE type);

	// �����蔻��̏���Ԃ�
	[[nodiscard]] Shared_Collision2D GetCollisionData(void) const;

	// �����蔻��̏����Z�b�g
	void CreateCollisionData(DRAW_POSITION_TYPE posType, Shared_Collision2D collision);

	// �qUI��Ԃ�
	const std::vector<std::shared_ptr<UIBase>> GetChilds(void) const;

	// �qUI��ǉ�
	void AddChild(std::shared_ptr<UIBase> child);

	// �eUI��Ԃ�
	const std::weak_ptr<UIBase> GetParent(void) const;

	// �eUI���Z�b�g
	void SetParent(std::weak_ptr<UIBase> parent);

	/// @brief �A�N�e�B�u(�\�����)���ǂ�����Ԃ�
	/// @return �t���O
	bool IsActive(void) const;

	/// @brief �A�N�e�B�u(�\�����)���ǂ������Z�b�g
	/// @param flag �t���O
	void SetIsActive(bool flag);

	/// @brief ����UI�������\������Ă��邩��Ԃ�
	/// @return �t���O
	bool IsHighlighted(void) const;

	/// @brief ����UI�������\������Ă��邩���Z�b�g
	/// @param flag �t���O
	void SetIsHighlighted(bool flag);
};