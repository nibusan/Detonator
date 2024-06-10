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
	const VECTOR& pos,
	const VECTOR& localPos,
	const VECTOR& rot,
	const VECTOR& localRot,
	const VECTOR& scl,
	std::shared_ptr<VertexShader> vs = nullptr,
	std::shared_ptr<PixelShader> ps = nullptr
) {

	// シェーダーがセットされているかを判定
	bool isSettingOrigShader = (vs != nullptr) && (ps != nullptr);

	// そのシェーダーを使って描画
	if (isSettingOrigShader) {
		MV1SetUseOrigShader(true);
		SetUsePixelShader(ps->GetHandle());
		SetUseVertexShader(vs->GetHandle());
	}

	// モデルの座標などの情報をセットする
	auto handle = GetHandle();
	MV1SetPosition(handle, VAdd(pos, localPos));
	MV1SetRotationXYZ(handle, VAdd(rot, localRot));
	MV1SetScale(handle, scl);

	// モデルの描画
	MV1DrawModel(handle);

	// シェーダーをセットしている場合は
	// セットしたシェーダーを解除する
	if (isSettingOrigShader) {
		MV1SetUseOrigShader(false);
		SetUsePixelShader(-1);
		SetUseVertexShader(-1);
	}
}
