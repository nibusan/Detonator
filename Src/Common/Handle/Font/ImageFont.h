#pragma once
#include <filesystem>
#include <memory> 
#include <string>
#include <vector>
#include <DxLib.h>
#include "../../Vector2.h"
#include "../HandleBase.h"

class ImageFont final : public HandleBase {
private:
	int* graphics_;
	int numX_;
	int numY_;
	int sizeX_;
	int sizeY_;
	Vector2<int> size_;

public:
	ImageFont(void);
	ImageFont(int* handles, int numX, int numY, int sizeX, int sizeY, char start);

	~ImageFont(void) override;

	[[nodiscard]] Vector2<int> GetFontSize(void) const;

};

using Unique_ImageFont = std::unique_ptr<ImageFont>;
using Shared_ImageFont = std::shared_ptr<ImageFont>;
using Weak_ImageFont = std::weak_ptr<ImageFont>;