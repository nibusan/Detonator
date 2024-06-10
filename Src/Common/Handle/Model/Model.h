#pragma once
#include <filesystem>
#include <string>
#include <DxLib.h>
#include "../HandleBase.h"

class VertexShader;
class PixelShader;
class Model : public HandleBase
{
public:
	Model(void);
	Model(const std::filesystem::path& path);
	Model(int handle);

	~Model(void) override;

	/// @brief MV1���f����`��
	/// @param pos ���W
	/// @param localPos ���[�J�����W
	/// @param rot �p�x
	/// @param localRot ���[�J���p�x(���O�Ԉ���Ă邩��)
	/// @param scl �X�P�[���l
	/// @param vs ���_�V�F�[�_�[
	/// @param ps �s�N�Z���V�F�[�_�[
	void Draw(
		const VECTOR& pos, 
		const VECTOR& localPos, 
		const VECTOR& rot, 
		const VECTOR& localRot, 
		const VECTOR& scl, 
		std::shared_ptr<VertexShader> vs,
		std::shared_ptr<PixelShader> ps
	);
};

using Unique_Model = std::unique_ptr<Model>;
using Shared_Model = std::shared_ptr<Model>;
using Weak_Model = std::weak_ptr<Model>;