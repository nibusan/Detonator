#pragma once
#include <filesystem>
#include <memory> 
#include <string>
#include <DxLib.h>
#include "../HandleBase.h"

class Font final : public HandleBase {
private:
	std::filesystem::path path_;
	std::string name_;
	int size_;

public:
	Font(void);
	Font(const std::filesystem::path& path, std::string name, int fontSize);
	Font(std::string name, int size);

	~Font(void) override;

	[[nodiscard]] int GetFontSize(void) const;
	[[nodiscard]] std::string GetFontName(void) const;

};

using Unique_Font = std::unique_ptr<Font>;
using Shared_Font = std::shared_ptr<Font>;
using Weak_Font = std::weak_ptr<Font>;