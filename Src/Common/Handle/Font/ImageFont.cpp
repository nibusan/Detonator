#include <DxLib.h>
#include "../HandleBase.h"
#include "ImageFont.h"

ImageFont::ImageFont(void) :
size_({}) {

}

ImageFont::ImageFont(int* handles, int numX, int numY, int sizeX, int sizeY, char start) :
graphics_(handles),
numX_(numX),
numY_(numY),
sizeX_(sizeX),
sizeY_(sizeY),
size_({}) {
	handle_ = CreateFontToHandle(NULL, 64, 0);
	int count = 0;
	for (int i = 0; i < numY_; i++) {
		for (int j = 0; j < numX_; j++) {
			TCHAR entryChar[2];

			// “o˜^‚·‚é1•¶Žš‚¾‚¯‚Ì•¶Žš—ñ‚ðì¬
			entryChar[0] = (char)(start + count);
			entryChar[1] = 0;

			// •¶Žš‰æ‘œ‚ð“o˜^
			AddFontImageToHandle(handle_, entryChar, handles[count], 0, 0, 40);

			count++;
		}
	}
}

ImageFont::~ImageFont(void) {
	if (IsAutoDeleteHandle()) {
		int count = 0;
		for (int i = 0; i < numY_; i++) {
			for (int j = 0; j < numX_; j++) {
				DeleteGraph(graphics_[count]);
				count++;
			}
		}
		delete[] graphics_;
		DeleteFontToHandle(handle_);
	}
}

Vector2<int> ImageFont::GetFontSize(void) const {
	return Vector2<int>();
}
