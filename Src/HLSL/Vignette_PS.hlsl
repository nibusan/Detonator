#include "PixelShader_Header.hlsli"

//定数バッファ
cbuffer buff : register(b0)
{
	float colorR;
	float colorG;
	float colorB;
	float colorPow;
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 color = tex.Sample(texSampler, input.uv);
	float2 calcUV = input.uv - 0.5f;
	float bright = clamp(length(pow(calcUV, 5)), 0.0f, 1.0f);
	float4 addColor = float4(0.5f, 0.0f, 0.0f, 1.0f);
	addColor *= bright * colorPow;

	return float4(color.rgb + (addColor.rgb - color.rgb) * addColor.a, 1.0f);
}