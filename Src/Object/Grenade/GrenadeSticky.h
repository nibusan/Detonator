#pragma once
#include"GrenadeBase.h"

class GrenadeSticky final : public GrenadeBase
{
public:


	GrenadeSticky(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GrenadeSticky() = default;

	// Õ“Ë‚µ‚½‚Æ‚«‚Ìˆ—
	//void OnHit(MV1_COLL_RESULT_POLY info) override;
	void OnHit(VECTOR normal, VECTOR hitPos) override;


private:
	int soundHandle_;

	// Še©“Æ©‚Ìİ’è
	void SetParam(void) override;

	// ”š”­
	void Blast(void) override;

	// ˆÚ“®
	void UpdateNone(void)override;

	void UpdateBlast(void)override;

	void Release_Object(void) override;

	// ‚»‚ê‚¼‚ê‚Ì‰Šú‰»
	void Reset(void) override;
};

