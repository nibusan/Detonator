#include "../PixelShader_Header.hlsli"

//定数バッファ
cbuffer buff : register(b0)
{
    float value;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = tex.Sample(texSampler, input.uv);
    color.gb -= value;
    return color;
}