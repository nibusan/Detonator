#include <DxLib.h>
#include "../HandleBase.h"
#include "Font.h"

Font::Font(void) :
size_(0) {

}

Font::Font(const std::filesystem::path& path, std::string name, int fontSize) :
name_(name),
size_(fontSize) {
	AddFontResourceEx(path.string().c_str(), FR_PRIVATE, NULL);
	handle_ = CreateFontToHandle(name_.c_str(), size_, 1);
}

Font::Font(std::string name, int size) :
size_(size),
name_(name) {
	handle_ = CreateFontToHandle(name.c_str(), size_, 1);
}

Font::~Font(void) {
	if (IsAutoDeleteHandle()) {
		DeleteFontToHandle(handle_);
		RemoveFontResourceEx(path_.string().c_str(), FR_PRIVATE, NULL);
	}
}

int Font::GetFontSize(void) const {
	return size_;
}

std::string Font::GetFontName(void) const {
	return name_;
}
