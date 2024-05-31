#include "../PixelShader_Header.hlsli"

Texture2D tex_Mask : register(t1);
SamplerState texSampler_Mask : register(s1);

//定数バッファ
cbuffer buff : register(b0)
{
	
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 color = tex.Sample(texSampler, input.uv);
	float4 color_Mask = tex_Mask.Sample(texSampler_Mask, input.uv);
	float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	if (color_Mask.r == 1.0f && color_Mask.g == 1.0f && color_Mask.b == 1.0f && color.a != 0.0f)
	{
		outColor = color;
	}
	
	return outColor;
}