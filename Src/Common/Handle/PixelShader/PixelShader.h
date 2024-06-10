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

	/// @brief 描画処理(Graphicクラスで使用してます) 
	void Draw(void);

	/// @brief 使用するテクスチャをセット
	/// @brief 0番にメインのテクスチャが入るので
	/// @brief 1番から入れてください
	/// @param stageIndex スロットID
	/// @param graphicHandle テクスチャのグラフィックハンドル
	void SetUseTexture(int stageIndex, int graphicHandle);
	void SetUseTexture(int stageIndex, int graphicHandle, const Vector2<float>& pos);

	/// @brief 定数バッファに定数をセット
	/// @param index スロットID
	/// @param value 定数
	void SetConstantValue(int index, float value);

	/// @brief 定数バッファを生成する
	/// @param size 定数バッファのサイズ
	void CreateConstantBuffer(int size);

	/// @brief 定数バッファを返す
	/// @return 定数バッファ
	int GetConstantBuffer(void) const;

private:
	VERTEX2DSHADER vertex_[6];

	// 描画対象のスクリーン
	int shaderScreen_;

	// 定数バッファ
	int cbuff_;
};

using Unique_PixelShader = std::unique_ptr<PixelShader>;
using Shared_PixelShader = std::shared_ptr<PixelShader>;
using Weak_PixelShader = std::weak_ptr<PixelShader>;