#pragma once
#include <DxLib.h>
class IntVector3
{

public:

	int x;
	int y;
	int z;

	// コンストラクタ
	IntVector3();

	// コンストラクタ
	IntVector3(int vX, int vY, int vZ);

	// コンストラクタ
	IntVector3(VECTOR v);

	// デストラクタ
	~IntVector3(void);

	bool operator<(const IntVector3& value) const;
	
	void Add(int v);
	void Sub(int v);
	void Scale(int v);

};

