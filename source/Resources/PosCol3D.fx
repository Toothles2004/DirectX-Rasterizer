// Input/Output Structs
struct VS_INPUT
{
	float3 Position : POSITION;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
    float4 WorldPosition : WORLD_POS;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

float4x4 gWorldViewProj : WorldViewProjection;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;

float4x4 gWorldMatrix : WORLD;
float3 gCameraPos : CAMERA;

float3 gLightDir = { 0.577f, -0.577f, 0.577f};
float gPI = 3.14159265359f;
float gLightIntensity = 7.f;
float gShininess = 25.f;
float3 gAmbient = { 0.03f, 0.03f, 0.03f };

bool gNormalOn = {false};

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

// Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.WorldPosition = mul(float4(input.Position, 1.f), gWorldMatrix);
    output.Uv = input.Uv;
    output.Normal = mul(normalize(input.Normal), (float3x3) gWorldMatrix);
    output.Tangent = mul(normalize(input.Tangent), (float3x3) gWorldMatrix);
	return output;
}

// Pixel Shader
float4 PS(VS_OUTPUT input, SamplerState state) : SV_TARGET
{
	float3 invViewDirection = normalize(gCameraPos - input.WorldPosition.xyz);

    float3 binormal = cross(input.Normal, input.Tangent);
    float4x4 tangentSpaceAxis = float4x4(float4(input.Tangent, 0.f), float4(binormal, 0.f), float4(input.Normal, 0.f), float4(0.f, 0.f, 0.f, 1.f));

    float3 glossiness = gGlossinessMap.Sample(state, input.Uv);
    float3 specular =  gSpecularMap.Sample(state, input.Uv);
    float3 diffuse = gDiffuseMap.Sample(state, input.Uv);

    diffuse *= gLightIntensity;
    diffuse /= gPI;

    float3 sampleNormal = (float3)0;

    if (gNormalOn)
    {
        sampleNormal = gNormalMap.Sample(state, input.Uv);
        sampleNormal /= 255.f;
        sampleNormal *= 2.f;
        sampleNormal -= float3(1.f, 1.f, 1.f);
        sampleNormal = mul(float4(sampleNormal, 0.f), (float4x4) tangentSpaceAxis);
    }
    else
    {
        sampleNormal = input.Normal;
    }

    float observedArea = saturate(dot(sampleNormal, gLightDir));

    glossiness *= gShininess;

    specular = specular * (float) (pow(saturate(dot(reflect(-gLightDir, sampleNormal), invViewDirection)), glossiness.r));

    return float4(observedArea * (diffuse + specular + gAmbient), 1.f);
}


float4 PSPoint(VS_OUTPUT input) : SV_TARGET
{
    return PS(input,samPoint);
}

float4 PSLinear(VS_OUTPUT input) : SV_TARGET
{
    return PS(input, samLinear);
}

float4 PSAnisotropic(VS_OUTPUT input) : SV_TARGET
{
    return PS(input, samAnisotropic);
}

//Technique

technique11 PointTechnique
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSPoint()));
    }
}

technique11 LinearTechnique
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSLinear()));
    }
}

technique11 AnisotropicTechnique
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSAnisotropic()));
    }
}
