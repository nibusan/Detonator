#pragma once
#include "UILayerBase.h"

class UILayer_mLobby : public UILayerBase {
public:
	UILayer_mLobby(void) = default;
	~UILayer_mLobby(void) override = default;

private:

	void SetDefaultUI(void) override;
};