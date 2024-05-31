#pragma once
#include "UILayerBase.h"

class UILayer_sLobby : public UILayerBase {
public:
	UILayer_sLobby(void) = default;
	~UILayer_sLobby(void) override = default;

private:

	void SetDefaultUI(void) override;
};

