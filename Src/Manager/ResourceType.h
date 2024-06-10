#pragma once

namespace Resource {
	/// @brief リソースファイルの種類
	enum class RESOURCE_FILE_TYPE {
		IMAGE,			// 画像
		SOUND_2D,		// 2D音声
		SOUND_3D,		// 3D音声
		MODEL,			// 3Dモデル
		FONT,			// フォント
		PIXEL_SHADER,	// ピクセルシェーダー
		VERTEX_SHADER,	// 頂点シェーダー
		MAX				// 要素数カウント用
	};
}