#pragma once
#include "UILayerBase.h"

class UILayer_mGame : public UILayerBase {
public:
	UILayer_mGame(void) = default;
	~UILayer_mGame(void) override = default;

private:

	void SetDefaultUI(void) override;
};