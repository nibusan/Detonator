#include <string>
#include <DxLib.h>
#include "../PixelShader/PixelShader.h"
#include "../VertexShader/VertexShader.h"
#include "Model.h"

Model::Model(void) {

}

Model::Model(const std::filesystem::path& path) {
	handle_ = MV1LoadModel(path.string().c_str());
}

Model::Model(int handle) {
	handle_ = MV1DuplicateModel(handle);
}

Model::~Model(void) {
	if (IsAutoDeleteHandle()) {
		MV1DeleteModel(handle_);
	}
}

void Model::Draw(
	VECTOR pos,
	VECTOR localPos,
	VECTOR rot,
	VECTOR localRot,
	VECTOR scl,
	Shared_VertexShader vs = nullptr,
	Shared_PixelShader ps = nullptr
) {

	// �V�F�[�_�[���Z�b�g����Ă��邩�𔻒�
	bool isSettingOrigShader = vs != nullptr && ps != nullptr;
	 
	// ���̃V�F�[�_�[���g���ĕ`��
	if (isSettingOrigShader) {
		MV1SetUseOrigShader(true);
		SetUsePixelShader(ps->GetHandle());
		SetUseVertexShader(vs->GetHandle());
	}

	auto handle = GetHandle();
	MV1SetPosition(handle, VAdd(pos, localPos));
	MV1SetRotationXYZ(handle, VAdd(rot, localRot));
	MV1SetScale(handle, scl);
	MV1DrawModel(handle);

	// �V�F�[�_�[���Z�b�g���Ă���ꍇ��
	// �Z�b�g�����V�F�[�_�[����������
	if (isSettingOrigShader) {
		MV1SetUseOrigShader(false);
		SetUsePixelShader(-1);
		SetUseVertexShader(-1);
	}
}
