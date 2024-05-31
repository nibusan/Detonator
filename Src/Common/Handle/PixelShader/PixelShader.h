#pragma once
#include <memory>
#include <filesystem>
#include <string>
#include <DxLib.h>
#include "../../Vector2.h"
#include "../HandleBase.h"

class PixelShader : public HandleBase
{

public:
	PixelShader(void);
	PixelShader(const std::filesystem::path& path);
	PixelShader(int handle);
	~PixelShader(void);

	void Begin(void);
	void End(void);
	void Draw(void);
	void SetRenderTarget(int targetIndex, int screenHandle);

	//Graphic��Draw�֐��Ŏg���ꍇ��
	//0�ԂɃ��C���̃e�N�X�`��������̂�
	//1�Ԃ������Ă�������
	void SetUseTexture(int stageIndex, int graphicHandle);

	void SetUseTexture(int stageIndex, int graphicHandle, Vector2<float> pos);

	void SetConstantValue(int index, float value);
	void CreateConstantBuffer(int size);

private:
	VERTEX2DSHADER vertex_[6];
	int shaderScreen_;	//�`��Ώۂ̃X�N���[��
	int cbuff_;    //�萔�o�b�t�@
};

using Unique_PixelShader = std::unique_ptr<PixelShader>;
using Shared_PixelShader = std::shared_ptr<PixelShader>;
using Weak_PixelShader = std::weak_ptr<PixelShader>;