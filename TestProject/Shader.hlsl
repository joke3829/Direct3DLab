
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
    output.positionW = float4(input.position, 1.0f); //mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProj);
    output.color = input.color;
    return output;
}


float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
}

float4 VSNoShader(uint nVertexID : SV_VertexID) : SV_POSITION
{
    float4 output;
    if (nVertexID == 0)
        output = float4(0.0, 0.5, 0.5, 1.0);
    else if(nVertexID == 1)
        output = float4(0.5, -0.5, 0.5, 1.0);
    else if(nVertexID == 2)
        output = float4(-0.5, -0.5, 0.5, 1.0);
    
    return output;
}

float4 PSNoShader(float4 input : SV_POSITION) : SV_TARGET
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}