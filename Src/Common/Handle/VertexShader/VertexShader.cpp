#include <DxLib.h>
#include "VertexShader.h"

VertexShader::VertexShader(void) {

}

VertexShader::VertexShader(const std::filesystem::path& path) {
	handle_ = LoadVertexShader(path.string().c_str());
}

VertexShader::VertexShader(int handle) {
	handle_ = handle;
}

VertexShader::~VertexShader(void) {

}

void VertexShader::SetUseShader(void) {
	SetUseVertexShader(GetHandle());
}
