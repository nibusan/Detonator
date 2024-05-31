#pragma once
#include"GrenadeBase.h"

class GrenadeSmoke final : public GrenadeBase
{
public:

	GrenadeSmoke(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GrenadeSmoke() = default;

	// Õ“Ë‚µ‚½‚Æ‚«‚Ìˆ—
	void OnHit(MV1_COLL_RESULT_POLY info) override;

private:

	// Še©“Æ©‚Ìİ’è
	void SetParam(void) override;

	// ”š”­
	void Blast(void) override;

	// ‚»‚ê‚¼‚ê‚Ì‰Šú‰»
	void Reset(void) override;
};

