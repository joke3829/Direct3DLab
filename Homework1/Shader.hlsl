//==================================================
// Shader.hlsl
// 셰이더 코드들을 모아둔 셰이더 파일
//==================================================

// 구조체 모음 =====================================

struct VS_TEXTURED_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
};
//====================================================
struct VS_NLightTextred_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_NLightTextured_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};
//====================================================
struct VS_Diffused_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_Diffused_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// 셰이더 변수 모음 ================================

// 상수 버퍼
// 0번 카메라 정보
cbuffer CameraInfo : register(b0)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProj : packoffset(c4);
    float3 cameraEye : packoffset(c8);
}
// 1번 월드변환과 반짝임 계수
cbuffer WorldInfo : register(b1)
{
    matrix gmtxWorld : packoffset(c0);
}
// 2번 전역 조명 정보
cbuffer DirectionLight : register(b2)
{
    float4 cDiffused;
    float4 cSpecular;
    float4 cAmbient;
    float3 LightDirction;
}
// 3번 반짝임 계수
cbuffer shInfo : register(b3)
{
    float1 sh;
}
// 지형 월드
cbuffer TerrainInfo : register(b4)
{
    matrix gmtxTerrainWorld : packoffset(c0);
}

cbuffer ElapsedInfo : register(b5)
{
    float fElapsedTime;
}

Texture2D gtxtTexture : register(t0);
Texture2D gTerrainBaseTexture : register(t1);
Texture2D gTerrainDetailTexture : register(t2);
sampler gStaticSampler : register(s0);

//==================================================

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProj);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxWorld);
    output.normalW = mul(input.normal, (float3x3) gmtxWorld);
    output.uv = input.uv;
    return output;
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    float4 pixelColor = gtxtTexture.Sample(gStaticSampler, input.uv);
    
    float Diffuse = max(dot(input.normalW, LightDirction), 0.0f);
    float4 PhongD = Diffuse * cDiffused * pixelColor;
    
    float3 Ref1 = 2.0f * input.normalW * dot(input.normalW, LightDirction) - LightDirction;
    float View = normalize(cameraEye - input.positionW);
    float Specular = pow(max(dot(Ref1, View), 0.0f), 1.0f);
    if (Diffuse <= 0.0f)
        Specular = 0.0f;
    float4 PhongS = Specular * float4(1.0, 1.0, 1.0, 1.0) * cSpecular;
    
    float4 PhongA = cAmbient * pixelColor * 0.2;
    
    float4 finalColor = PhongD + PhongS + PhongA;
    finalColor.w = 1.0f;
    
    return finalColor;
}



VS_NLightTextured_OUTPUT VSNoLightTextured(VS_NLightTextred_INPUT input)
{
    VS_NLightTextured_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProj);
    output.uv = input.uv;
    return output;
}

float4 PSNoLightTextured(VS_NLightTextured_OUTPUT input) : SV_TARGET
{
    return gtxtTexture.Sample(gStaticSampler, input.uv);
}


VS_Diffused_OUTPUT VSDiffused(VS_Diffused_INPUT input)
{
    VS_Diffused_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProj);
    output.color = input.color;
    return output;
}

float4 PSDiffused(VS_Diffused_OUTPUT input) : SV_TARGET
{
    return input.color;
}

//====================================

struct VS_TERRAIN_INPUT
{
    float3 position : POSITION;
    float2 uv1 : TEXCOORD0;
    float2 uv2 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv1 : TEXCOORD0;
    float2 uv2 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
    VS_TERRAIN_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxTerrainWorld), gmtxView), gmtxProj);
    //output.position = mul(float4(input.position, 1.0f), gmtxView);
    output.uv1 = input.uv1;
    output.uv2 = input.uv2;
    return output;
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
    float4 baseColor = gTerrainBaseTexture.Sample(gStaticSampler, input.uv1);
    float4 detailColor = gTerrainDetailTexture.Sample(gStaticSampler, input.uv2);
    
    return saturate((baseColor * 0.7) + (detailColor * 0.3));
}

//=====================================

VS_NLightTextured_OUTPUT VSSkyBox(VS_NLightTextred_INPUT input)
{
    VS_NLightTextured_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProj);
    output.uv = input.uv;
    output.position.w = output.position.z;
    
    return output;
}

float4 PSSkyBox(VS_NLightTextured_OUTPUT input) : SV_TARGET
{
    return gtxtTexture.Sample(gStaticSampler, input.uv);
}

//=============================================================

struct VS_STANDARD_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float3 bitangentW : BITANGENT;
    float2 uv : TEXCOORD;
};

VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxWorld);
    output.normalW = mul(input.normal, (float3x3) gmtxWorld);
    output.tangentW = (float3) mul(float4(input.tangent, 1.0f), gmtxWorld);
    output.bitangentW = (float3) mul(float4(input.bitangent, 1.0f), gmtxWorld);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProj);
    output.uv = input.uv;

    return (output);
}

[earlydeapthstencil]
float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
    float4 pixelColor = gtxtTexture.Sample(gStaticSampler, input.uv);
    
    float Diffuse = max(dot(input.normalW, LightDirction), 0.0f);
    float4 PhongD = Diffuse * cDiffused * pixelColor;
    
    float3 Ref1 = 2.0f * input.normalW * dot(input.normalW, LightDirction) - LightDirction;
    float View = normalize(cameraEye - input.positionW);
    float Specular = pow(max(dot(Ref1, View), 0.0f), 1.0f);
    if(Diffuse <= 0.0f)
        Specular = 0.0f;
    float4 PhongS = Specular * float4(1.0, 1.0, 1.0, 1.0) * cSpecular;
    
    float4 PhongA = cAmbient * pixelColor * 0.2;
    
    float4 finalColor = PhongD + PhongS + PhongA;
    finalColor.w = 1.0f;
    
    return finalColor;
}

//===============================================

struct VS_BILLBOARD_INPUT
{
    float3 center : POSITION;
    float2 size : SIZE;
};

struct VS_BILLBOARD_OUTPUT
{
    float3 center : POSITION;
    float2 size : SIZE;
};

struct GS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_BILLBOARD_OUTPUT VSBillboard(VS_BILLBOARD_INPUT input)
{
    VS_BILLBOARD_OUTPUT output;
    output.center = mul(float4(input.center, 1.0f), gmtxWorld);
    output.size = input.size;
    return output;
}

[maxvertexcount(4)]
void GSBillboard(point VS_BILLBOARD_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> outStream)
{
    float3 vUp = float3(0.0, 1.0, 0.0);
    float3 vLook = cameraEye - input[0].center;
    vLook.y = 0.0f;
    vLook = normalize(vLook);
    float3 vRight = cross(vUp, vLook);
    //vUp = cross(vLook, vRight);
    float4 Vertices[4];
    float fHalfW = input[0].size.x / 2;
    float fHalfH = input[0].size.y / 2;
    Vertices[0] = float4(input[0].center + fHalfW * vRight - fHalfH * vUp, 1.0f);
    Vertices[1] = float4(input[0].center + fHalfW * vRight + fHalfH * vUp, 1.0f);
    Vertices[2] = float4(input[0].center - fHalfW * vRight - fHalfH * vUp, 1.0f);
    Vertices[3] = float4(input[0].center - fHalfW * vRight + fHalfH * vUp, 1.0f);
    float2 UVs[4] = { float2(0.0, 1.0), float2(0.0, 0.0), float2(1.0, 1.0), float2(1.0, 0.0) };
    GS_OUTPUT output;
    for (int i = 0; i < 4; ++i)
    {
        output.position = mul(mul(Vertices[i], gmtxView), gmtxProj);
        output.uv = UVs[i];
        outStream.Append(output);
    }
}

[earlydeathstencil]
float4 PSBillboard(GS_OUTPUT input) : SV_TARGET
{
    float4 fTexColor = gtxtTexture.Sample(gStaticSampler, input.uv);
    if(fTexColor.w == 0.0f)
        discard;
    return fTexColor;
}


//===================================================

struct VS_PARTICLE_POINT
{
    float3 position : POSITION;
    float3 direction : VELOCITY;
    float size : SIZE;
    float lifeTime : LIFETIME;
    uint particleType : PARTICLETYPE;
};



VS_PARTICLE_POINT VSSOParticle(VS_PARTICLE_POINT input)
{
    return input;
}

[maxvertexcount(30)]
void GSSOParticle(point VS_PARTICLE_POINT input[1], inout PointStream<VS_PARTICLE_POINT> outStream)
{
    VS_PARTICLE_POINT particle = input[0];
    particle.lifeTime -= fElapsedTime;
    if (particle.particleType == 0)
    {
        if (particle.lifeTime <= 0)
        {
            particle.lifeTime = 0.5f;
            outStream.Append(particle);
            
            VS_PARTICLE_POINT newParticle = particle;
            newParticle.lifeTime = 4.0f;
            newParticle.direction = float3(0.0, 1.0, 0.0);
            newParticle.particleType = 1;
            newParticle.size = 0.0f;
            outStream.Append(newParticle);
        }
        else
        {
            outStream.Append(particle);
        }
    }
    else
    {
        if (particle.lifeTime > 0)
        {
            particle.position += ((20.0f * particle.direction) * fElapsedTime);
            particle.size += (10.0f * fElapsedTime);
            outStream.Append(particle);
        }
    }
}


VS_PARTICLE_POINT VSDrawParticle(VS_PARTICLE_POINT input)
{
    VS_PARTICLE_POINT output = input;
    output.position = mul(float4(input.position, 1.0f), gmtxWorld);
    return output;
}

[maxvertexcount(4)]
void GSDrawParticle(point VS_PARTICLE_POINT input[1], inout TriangleStream<GS_OUTPUT> outStream)
{
    float3 vUp = float3(0.0, 1.0, 0.0);
    float3 vLook = cameraEye - input[0].position;
    //vLook.y = 0.0f;
    vLook = normalize(vLook);
    float3 vRight = cross(vUp, vLook);
    vUp = cross(vLook, vRight);
    float4 Vertices[4];
    float fHalf = input[0].size / 2;
    Vertices[0] = float4(input[0].position + fHalf * vRight - fHalf * vUp, 1.0f);
    Vertices[1] = float4(input[0].position + fHalf * vRight + fHalf * vUp, 1.0f);
    Vertices[2] = float4(input[0].position - fHalf * vRight - fHalf * vUp, 1.0f);
    Vertices[3] = float4(input[0].position - fHalf * vRight + fHalf * vUp, 1.0f);
    float2 UVs[4] = { float2(0.0, 1.0), float2(0.0, 0.0), float2(1.0, 1.0), float2(1.0, 0.0) };
    GS_OUTPUT output;
    for (int i = 0; i < 4; ++i)
    {
        output.position = mul(mul(Vertices[i], gmtxView), gmtxProj);
        output.uv = UVs[i];
        outStream.Append(output);
    }
}

[earlydeathstencil]
float4 PSDrawParticle(GS_OUTPUT input) : SV_TARGET
{
    float4 fTexColor = gtxtTexture.Sample(gStaticSampler, input.uv);
    if (fTexColor.w == 0.0f)
        discard;
    return fTexColor;

}

//===============================================================

struct VS_WATER_INPUT
{
    float3 position : POSITION;
};

struct VS_WATER_OUTPUT
{
    float4 posW : SV_POSITION;
};

float4 VSWater(float3 input : POSITION) : SV_POSITION
{
    return mul(mul(mul(float4(input, 1.0f), gmtxWorld), gmtxView), gmtxProj);
}

[earlydepthstencil]
float4 PSWater(float4 input : SV_POSITION) : SV_TARGET
{
    return float4(0.0, 0.0, 1.0, 0.5);
}