#pragma once
#include <memory>
#include <filesystem>
#include <string>
#include <DxLib.h>
#include "../../Vector2.h"
#include "../HandleBase.h"

class VertexShader : public HandleBase
{

public:
	VertexShader(void);
	VertexShader(const std::filesystem::path& path);
	VertexShader(int handle);
	~VertexShader(void);

	void SetUseShader(void);

private:
	
};

using Unique_VertexShader = std::unique_ptr<VertexShader>;
using Shared_VertexShader = std::shared_ptr<VertexShader>;
using Weak_VertexShader = std::weak_ptr<VertexShader>;