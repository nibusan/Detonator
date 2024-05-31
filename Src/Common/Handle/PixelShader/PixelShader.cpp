#include <filesystem>
#include <DxLib.h>
#include "../../Vector2.h"
#include "PixelShader.h"

PixelShader::PixelShader(void) {

}

PixelShader::PixelShader(const std::filesystem::path& path) {
	handle_ = LoadPixelShader(path.string().c_str());

}

PixelShader::PixelShader(int handle) {
	handle_ = handle;

}

PixelShader::~PixelShader(void) {
	if (IsAutoDeleteHandle()) {
		DeleteShader(handle_);
	}
}

void PixelShader::Begin(void) {
	UpdateShaderConstantBuffer(cbuff_);
	SetShaderConstantBuffer(cbuff_, DX_SHADERTYPE_PIXEL, 0);
	SetUseAlphaTestFlag(true);
	SetUsePixelShader(handle_);
}

void PixelShader::End(void) {
	DrawPrimitive2DToShader(vertex_, 6, DX_PRIMTYPE_TRIANGLESTRIP);
	SetUsePixelShader(-1);
	SetRenderTargetToShader(0, -1);
}

void PixelShader::Draw(void) {
	UpdateShaderConstantBuffer(cbuff_);
	SetShaderConstantBuffer(cbuff_, DX_SHADERTYPE_PIXEL, 0);
	SetUseAlphaTestFlag(true);
	SetUsePixelShader(handle_);
	DrawPrimitive2DToShader(vertex_, 6, DX_PRIMTYPE_TRIANGLESTRIP);
	SetUsePixelShader(-1);
	SetRenderTargetToShader(0, -1);
}

void PixelShader::SetRenderTarget(int targetIndex, int screenHandle) {
	SetRenderTargetToShader(targetIndex, screenHandle);
}

void PixelShader::SetUseTexture(int stageIndex, int graphicHandle) {
	SetUseTextureToShader(stageIndex, graphicHandle);

	if (stageIndex == 0) {
		Vector2<int> tmp_ScreenSize = {};
		GetGraphSize(graphicHandle, &tmp_ScreenSize.x, &tmp_ScreenSize.y);
		for (int i = 0; i < 4; i++) {
			vertex_[i].pos = VGet(
				(i % 2) * tmp_ScreenSize.x,
				(i / 2) * tmp_ScreenSize.y,
				0
			);
			vertex_[i].rhw = 1.0f;
			vertex_[i].dif = GetColorU8(255, 255, 255, 255);
			vertex_[i].spc = GetColorU8(0, 0, 0, 0);
			vertex_[i].u = vertex_[i].su = (float)(i % 2);
			vertex_[i].v = vertex_[i].sv = (float)(i / 2);
		}
		vertex_[4] = vertex_[2];
		vertex_[5] = vertex_[1];
	}
}

void PixelShader::SetUseTexture(int stageIndex, int graphicHandle, Vector2<float> pos) {
	SetUseTextureToShader(stageIndex, graphicHandle);

	if (stageIndex == 0) {
		Vector2<int> tmp_ScreenSize = {};
		GetGraphSize(graphicHandle, &tmp_ScreenSize.x, &tmp_ScreenSize.y);
		Vector2<float> tmp_vertexPos[] = {
			{pos.x,pos.y},
			{pos.x + tmp_ScreenSize.x,pos.y},
			{pos.x,pos.y + tmp_ScreenSize.y},
			{pos.x + tmp_ScreenSize.x,pos.y + tmp_ScreenSize.y},
		};
		for (int i = 0; i < 4; i++) {
			vertex_[i].pos = VGet(
				tmp_vertexPos[i].x,
				tmp_vertexPos[i].y,
				0
			);
			vertex_[i].rhw = 1.0f;
			vertex_[i].dif = GetColorU8(255, 255, 255, 255);
			vertex_[i].spc = GetColorU8(0, 0, 0, 0);
			vertex_[i].u = vertex_[i].su = (float)(i % 2);
			vertex_[i].v = vertex_[i].sv = (float)(i / 2);
		}
		vertex_[4] = vertex_[2];
		vertex_[5] = vertex_[1];
	}
}

void PixelShader::SetConstantValue(int index, float value) {
	float* cbuffF = static_cast<float*>(GetBufferShaderConstantBuffer(cbuff_));
	cbuffF[index] = value;
}

void PixelShader::CreateConstantBuffer(int size) {
	cbuff_ = CreateShaderConstantBuffer(size);
}
