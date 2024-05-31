#pragma once
#include <filesystem>
#include <string>
#include <DxLib.h>
#include "../HandleBase.h"
#include "../VertexShader/VertexShader.h"
#include "../PixelShader/PixelShader.h"

class VertexShader;
class PixelShader;
class Model : public HandleBase
{

public:

private:
	
public:
	Model(void);
	Model(const std::filesystem::path& path);
	Model(int handle);

	~Model(void) override;

	void Draw(
		VECTOR pos, 
		VECTOR localPos, 
		VECTOR rot, 
		VECTOR localRot, 
		VECTOR scl, 
		Shared_VertexShader vs,
		Shared_PixelShader ps
	);
};

using Unique_Model = std::unique_ptr<Model>;
using Shared_Model = std::shared_ptr<Model>;
using Weak_Model = std::weak_ptr<Model>;