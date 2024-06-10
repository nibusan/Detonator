#pragma once
#include <memory>
#include <filesystem>
#include <string>
#include <DxLib.h>
#include "../../Vector2.h"
#include "../HandleBase.h"

class PixelShader : public HandleBase {
public:
	PixelShader(void);
	PixelShader(const std::filesystem::path& path);
	PixelShader(int handle);
	~PixelShader(void);

	/// @brief �`�揈��(Graphic�N���X�Ŏg�p���Ă܂�) 
	void Draw(void);

	/// @brief �g�p����e�N�X�`�����Z�b�g
	/// @brief 0�ԂɃ��C���̃e�N�X�`��������̂�
	/// @brief 1�Ԃ������Ă�������
	/// @param stageIndex �X���b�gID
	/// @param graphicHandle �e�N�X�`���̃O���t�B�b�N�n���h��
	void SetUseTexture(int stageIndex, int graphicHandle);
	void SetUseTexture(int stageIndex, int graphicHandle, const Vector2<float>& pos);

	/// @brief �萔�o�b�t�@�ɒ萔���Z�b�g
	/// @param index �X���b�gID
	/// @param value �萔
	void SetConstantValue(int index, float value);

	/// @brief �萔�o�b�t�@�𐶐�����
	/// @param size �萔�o�b�t�@�̃T�C�Y
	void CreateConstantBuffer(int size);

	/// @brief �萔�o�b�t�@��Ԃ�
	/// @return �萔�o�b�t�@
	int GetConstantBuffer(void) const;

private:
	VERTEX2DSHADER vertex_[6];

	// �`��Ώۂ̃X�N���[��
	int shaderScreen_;

	// �萔�o�b�t�@
	int cbuff_;
};

using Unique_PixelShader = std::unique_ptr<PixelShader>;
using Shared_PixelShader = std::shared_ptr<PixelShader>;
using Weak_PixelShader = std::weak_ptr<PixelShader>;