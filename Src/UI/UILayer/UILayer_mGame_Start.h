#pragma once
#include "UILayerBase.h"

class UILayer_mGame_Start : public UILayerBase {
public:
	UILayer_mGame_Start(void) = default;
	~UILayer_mGame_Start(void) override = default;

private:

	void SetDefaultUI(void) override;
};