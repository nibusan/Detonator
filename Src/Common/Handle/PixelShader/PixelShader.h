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

	//GraphicのDraw関数で使う場合は
	//0番にメインのテクスチャが入るので
	//1番から入れてください
	void SetUseTexture(int stageIndex, int graphicHandle);

	void SetUseTexture(int stageIndex, int graphicHandle, Vector2<float> pos);

	void SetConstantValue(int index, float value);
	void CreateConstantBuffer(int size);

private:
	VERTEX2DSHADER vertex_[6];
	int shaderScreen_;	//描画対象のスクリーン
	int cbuff_;    //定数バッファ
};

using Unique_PixelShader = std::unique_ptr<PixelShader>;
using Shared_PixelShader = std::shared_ptr<PixelShader>;
using Weak_PixelShader = std::weak_ptr<PixelShader>;