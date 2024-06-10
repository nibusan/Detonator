#include <filesystem>
#include <DxLib.h>
#include "../../Vector2.h"
#include "../PixelShader/PixelShader.h"
#include "Graphic.h"

Graphic::Graphic(void) {

}

Graphic::Graphic(const std::filesystem::path& path) {
	handle_ = LoadGraph(path.string().c_str());
	GetGraphSize(handle_, &size_.x, &size_.y);
	beforeShaderScreen_ = MakeScreen(size_.x, size_.y, true);
	afterShaderScreen_ = MakeScreen(size_.x, size_.y, true);
}

Graphic::Graphic(int handle) {
	handle_ = handle;
	GetGraphSize(handle_, &size_.x, &size_.y);
	beforeShaderScreen_ = MakeScreen(size_.x, size_.y, true);
	afterShaderScreen_ = MakeScreen(size_.x, size_.y, true);
}

Graphic::~Graphic(void) {
	DeleteGraph(beforeShaderScreen_);
	DeleteGraph(afterShaderScreen_);
	if (IsAutoDeleteHandle()) {
		DeleteGraph(handle_);
	}
}

void Graphic::Draw(const Vector2<float>& pos, bool isCenterDraw, const std::shared_ptr<PixelShader>& ps = nullptr) {
	if (ps == nullptr) {
		if (isCenterDraw) {
			DrawRotaGraph(pos.x, pos.y, 1.0f, 0.0f, handle_, true);
		}
		else {
			DrawGraph(pos.x, pos.y, handle_, true);
		}
	}
	else {
		// 元の描画スクリーンを退避
		auto defScreen = GetDrawScreen();

		SetDrawScreen(afterShaderScreen_);
		ClearDrawScreen();
		if (isCenterDraw) {
			DrawRotaGraph(0.0f, 0.0f, 1.0f, 0.0f, handle_, true);
		}
		else {
			DrawGraph(0.0f, 0.0f, handle_, true);
		}

		SetDrawScreen(beforeShaderScreen_);
		ClearDrawScreen();
		ps->SetUseTexture(0, afterShaderScreen_);
		ps->Draw();

		SetDrawScreen(defScreen);
		DrawGraph(pos.x, pos.y, beforeShaderScreen_, true);
	}
}

void Graphic::Draw(const Vector2<float>& pos, bool isCenterDraw, int divX, int divY, int numX, int numY, const std::shared_ptr<PixelShader>& ps) {
	Vector2<int> gDiv = { size_.x / divX, size_.y / divY };
	Vector2<int> gStartPos = { gDiv.x * numX, gDiv.y * numY };

	if (ps == nullptr) {
		if (isCenterDraw) {
			DrawRectRotaGraph(pos.x, pos.y, gStartPos.x, gStartPos.y, gDiv.x, gDiv.y, 1.0f, 0.0f, handle_, true);
		}
		else {
			DrawRectGraph(pos.x, pos.y, gStartPos.x, gStartPos.y, gDiv.x, gDiv.y, handle_, true);
		}
	}
	else {
		// 元の描画スクリーンを退避
		auto defScreen = GetDrawScreen();

		SetDrawScreen(afterShaderScreen_);
		ClearDrawScreen();
		if (isCenterDraw) {
			DrawRectRotaGraph(0, 0, gStartPos.x, gStartPos.y, gDiv.x, gDiv.y, 1.0f, 0.0f, handle_, true);
		}
		else {
			DrawRectGraph(0, 0, gStartPos.x, gStartPos.y, gDiv.x, gDiv.y, handle_, true);
		}

		SetDrawScreen(beforeShaderScreen_);
		ClearDrawScreen();
		ps->SetUseTexture(0, afterShaderScreen_);
		ps->Draw();

		SetDrawScreen(defScreen);
		DrawGraph(pos.x, pos.y, beforeShaderScreen_, true);
	}
}

void Graphic::Draw(const Vector2<float>& pos, float scale, float angle, int divX, int divY, int numX, int numY, const std::shared_ptr<PixelShader>& ps) {
	Vector2<int> gDiv = { size_.x / divX, size_.y / divY };
	Vector2<int> gStartPos = { gDiv.x * numX, gDiv.y * numY };

	if (ps == nullptr) {
		DrawRectRotaGraph(pos.x, pos.y, gStartPos.x, gStartPos.y, gDiv.x, gDiv.y, scale, angle, handle_, true);
	}
	else {
		// 元の描画スクリーンを退避
		auto defScreen = GetDrawScreen();

		SetDrawScreen(afterShaderScreen_);
		ClearDrawScreen();
		DrawRectRotaGraph(pos.x, pos.y, gStartPos.x, gStartPos.y, gDiv.x, gDiv.y, scale, angle, handle_, true);

		SetDrawScreen(beforeShaderScreen_);
		ClearDrawScreen();
		ps->SetUseTexture(0, afterShaderScreen_);
		ps->Draw();

		SetDrawScreen(defScreen);
		DrawGraph(pos.x, pos.y, beforeShaderScreen_, true);
	}
}

void Graphic::Draw(const Vector2<float>& pos1, const Vector2<float>& pos2, bool isCenterDraw, int divX, int divY, int numX, int numY, const Shared_PixelShader& ps) {
	Vector2<int> gDiv = { size_.x / divX, size_.y / divY };
	Vector2<int> gDiff = (pos2 - pos1).ToVector2i();
	Vector2<int> gStartPos = { gDiv.x * numX, gDiv.y * numY };

	if (ps == nullptr) {
		if (isCenterDraw) {
			DrawRectExtendGraph(pos1.x, pos1.y, pos2.x, pos2.y, gStartPos.x, gStartPos.y, gDiv.x, gDiv.y, handle_, true);
		}
		else {
			DrawRectExtendGraph(pos1.x, pos1.y, pos2.x, pos2.y, gStartPos.x, gStartPos.y, gDiv.x, gDiv.y, handle_, true);
		}
	}
	else {
		// 元の描画スクリーンを退避
		auto defScreen = GetDrawScreen();

		SetDrawScreen(afterShaderScreen_);
		ClearDrawScreen();
		if (isCenterDraw) {
			DrawRectExtendGraph(pos1.x, pos1.y, pos2.x, pos2.y, gStartPos.x, gStartPos.y, gDiv.x, gDiv.y, handle_, true);
		}
		else {
			DrawRectExtendGraph(pos1.x, pos1.y, pos2.x, pos2.y, gStartPos.x, gStartPos.y, gDiv.x, gDiv.y, handle_, true);
		}

		SetDrawScreen(beforeShaderScreen_);
		ClearDrawScreen();
		ps->SetUseTexture(0, afterShaderScreen_);
		ps->Draw();

		SetDrawScreen(defScreen);
		DrawExtendGraph(pos1.x, pos1.y, pos2.x, pos2.y, beforeShaderScreen_, true);
	}
}

void Graphic::Draw(const Vector2<float>& pos1, const Vector2<float>& pos2, const std::shared_ptr<PixelShader>& ps = nullptr) {
	if (ps == nullptr) {
		DrawExtendGraph(pos1.x, pos1.y, pos2.x, pos2.y, handle_, true);
	}
	else {
		// 元の描画スクリーンを退避
		auto defScreen = GetDrawScreen();

		SetDrawScreen(afterShaderScreen_);
		ClearDrawScreen();
		DrawGraph(0, 0, handle_, true);

		SetDrawScreen(beforeShaderScreen_);
		ClearDrawScreen();
		ps->SetUseTexture(0, afterShaderScreen_);
		ps->Draw();

		SetDrawScreen(defScreen);
		DrawExtendGraph(pos1.x, pos1.y, pos2.x, pos2.y, beforeShaderScreen_, true);
	}
}

void Graphic::Draw(const Vector2<float>& pos1, const Vector2<float>& pos2, const Vector2<float>& pos3, const std::shared_ptr<PixelShader>& ps = nullptr) {
	if (ps == nullptr) {
		DrawExtendGraph(pos1.x - pos3.x, pos1.y - pos3.y, pos2.x - pos3.x, pos2.y - pos3.y, handle_, true);
	}
	else {
		// 元の描画スクリーンを退避
		auto defScreen = GetDrawScreen();

		SetDrawScreen(afterShaderScreen_);
		ClearDrawScreen();
		DrawGraph(0, 0, handle_, true);

		SetDrawScreen(beforeShaderScreen_);
		ClearDrawScreen();
		ps->SetUseTexture(0, afterShaderScreen_);
		ps->Draw();

		SetDrawScreen(defScreen);
		DrawExtendGraph(pos1.x - pos3.x, pos1.y - pos3.y, pos2.x - pos3.x, pos2.y - pos3.y, beforeShaderScreen_, true);
	}
}

void Graphic::Draw(const Vector2<float>& pos, float scale, float angle, const std::shared_ptr<PixelShader>& ps = nullptr) {
	if (ps == nullptr) {
		DrawRotaGraph(pos.x, pos.y, scale, angle, handle_, true);
	}
	else {
		// 元の描画スクリーンを退避
		auto defScreen = GetDrawScreen();

		SetDrawScreen(afterShaderScreen_);
		ClearDrawScreen();
		DrawGraph(pos.x, pos.y, handle_, true);

		SetDrawScreen(beforeShaderScreen_);
		ClearDrawScreen();
		ps->SetUseTexture(0, afterShaderScreen_, Vector2<float>(-100.0f, 0.0f));
		ps->Draw();

		SetDrawScreen(defScreen);
		DrawRotaGraph(pos.x, pos.y, scale, angle, beforeShaderScreen_, true);
	}
}

const Vector2<int>& Graphic::GetSize(void) const {
	return size_;
}
