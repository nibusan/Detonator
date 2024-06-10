#pragma once
#include <filesystem>
#include <memory> 
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../HandleBase.h"
#include "../Graphic/Graphic.h"

class EffectBase : public HandleBase
{

public:
	EffectBase(void);
	EffectBase(const std::filesystem::path& path);
	EffectBase(int handle);

	~EffectBase(void) override;

	/// @brief ���W���Z�b�g
	/// @param pos ���W
	virtual void SetPos(const VECTOR& pos);

	/// @brief �X�P�[���l���Z�b�g
	/// @param scl �X�P�[���l
	virtual void SetScale(const VECTOR& scl);

	/// @brief �p�x���Z�b�g
	/// @param rot �p�x
	virtual void SetAngle(const VECTOR& rot);

	/// @brief �F���Z�b�g
	/// @param r Red
	/// @param g Green
	/// @param b Blue
	/// @param a Alpha
	virtual void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	
	/// @brief �X�s�[�h���Z�b�g
	/// @param speed �X�s�[�h
	virtual void SetSpeed(float speed);
	
	/// @brief �X�s�[�h��Ԃ�
	/// @return �X�s�[�h
	virtual float GetSpeed(void) const;
	
	/// @brief �G�t�F�N�g���Đ�
	/// @param pos ���W
	virtual void Play(const VECTOR& pos);
	
	/// @brief �Đ�����Ă邩��Ԃ�
	/// @return �Đ�����Ă邩
	virtual bool IsPlaying(void) const;
	
	/// @brief �G�t�F�N�g���~������
	virtual void Stop(void) const;
};