
struct VS_INPUT 
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT 
{
    float4 positionW : SV_POSITION;
    float4 color : COLOR;
};

cbuffer cbWorldInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProj : packoffset(c4);
};

VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;
    output.positionW = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProj);
    output.color = input.color;
    return output;
}

float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
}