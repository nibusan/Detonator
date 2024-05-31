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

	// ���W���Z�b�g
	virtual void SetPos(VECTOR& pos);

	// �X�P�[���l���Z�b�g
	virtual void SetScale(VECTOR& scl);

	// �p�x���Z�b�g
	virtual void SetAngle(VECTOR& rot);

	// �F���Z�b�g
	virtual void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	
	// �X�s�[�h���Z�b�g
	virtual void SetSpeed(float speed);
	
	// �X�s�[�h��Ԃ�
	virtual float GetSpeed(void) const;
	
	// �G�t�F�N�g���Đ�
	virtual void Play(VECTOR& pos);
	
	// �Đ�����Ă邩��Ԃ�
	virtual bool IsPlaying(void) const;
	
	// �G�t�F�N�g���~������
	virtual void Stop(void) const;
};

using Unique_Effect = std::unique_ptr<EffectBase>;
using Shared_Effect = std::shared_ptr<EffectBase>;