#include <DxLib.h>
#include "Grid.h"

Grid::Grid(void)
{
}

Grid::~Grid(void)
{
}

void Grid::Init(void)
{
}

void Grid::Update(void)
{
}

void Grid::Draw(void)
{

	// Åyó˚èKÅzç≈èâÇÃÇPñ{
	//VECTOR sPos = { 0.0f, 0.0f, 0.0f };
	//VECTOR ePos = { HLEN, 0.0f, 0.0f };
	//VECTOR sPos = { -HLEN, 0.0f, 0.0f };
	//VECTOR ePos = {  HLEN, 0.0f, 0.0f };
	//DrawLine3D(sPos, ePos, 0xff0000);

	// XZäÓñ{é≤(ÉOÉäÉbÉh)
	float num;
	VECTOR sPos;
	VECTOR ePos;
	for (int i = -HNUM; i < HNUM; i++)
	{

		num = static_cast<float>(i);

		// Xé≤(ê‘)
		sPos = { -HLEN, 0.0f, num * TERM };
		ePos = {  HLEN, 0.0f, num * TERM };
		DrawLine3D(sPos, ePos, 0xff0000);
		DrawSphere3D(ePos, 20.0f, 10, 0xff0000, 0xff0000, true);

		// Zé≤(ê¬)
		sPos = { num * TERM, 0.0f, -HLEN };
		ePos = { num * TERM, 0.0f,  HLEN };
		DrawLine3D(sPos, ePos, 0x0000ff);
		DrawSphere3D(ePos, 20.0f, 10, 0x0000ff, 0x0000ff, true);

	}

	// Yé≤(óŒ)
	sPos = { 0.0f, -HLEN, 0.0f };
	ePos = { 0.0f,  HLEN, 0.0f };
	DrawLine3D(sPos, ePos, 0x00ff00);
	DrawSphere3D(ePos, 20.0f, 10, 0x00ff00, 0x00ff00, true);


	DrawSphere3D({0.0f,200.0f,200.0f}, 10.0f, 10, 0x00ff00, 0x00ff00, true);
}

void Grid::Release(void)
{
}
