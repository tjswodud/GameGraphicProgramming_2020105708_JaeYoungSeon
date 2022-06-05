//--------------------------------------------------------------------------------------
// File: PhongShaders.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

#define NUM_LIGHTS (1)
#define NEAR_PLANE (0.01f)
#define FAR_PLANE (1000.0f)

Texture2D txDiffuse : register(t0);
SamplerState sampState : register(s0);

Texture2D normalMapTexture : register(t1);
SamplerState normalMapSampler : register(s1);

Texture2D shadowMapTexture : register(t2);
SamplerState shadowMapSampler : register(s2);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbChangeOnCameraMovement : register(b0)
{
    matrix View;
    float4 CameraPosition;
};

cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
    float4 OutputColor;
    bool HasNormalMap;
};

cbuffer cbLights : register(b3)
{
    float4 LightPositions[NUM_LIGHTS];
    float4 LightColors[NUM_LIGHTS];
    float4 AttenuationDistance[NUM_LIGHTS];
};

struct VS_PHONG_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    row_major matrix mTransform : INSTANCE_TRANSFORM;
};

struct PS_PHONG_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float4 LightViewPosition : TEXCOORD1;
};

struct PS_LIGHT_CUBE_INPUT
{
    float4 Position : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_PHONG_INPUT VSPhong(VS_PHONG_INPUT input)
{
    PS_PHONG_INPUT output = (PS_PHONG_INPUT)0;

    output.Position = input.Position;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = normalize(mul(float4(input.Normal, 0.0f), World).xyz);

    if (HasNormalMap)
    {
        output.Tangent = normalize(mul(float4(input.Tangent, 0.0f), World).xyz);
        output.Bitangent = normalize(mul(float4(input.Bitangent, 0.0f), World).xyz);
    }

    output.WorldPosition = mul(input.Position, World);
    output.TexCoord = input.TexCoord;

    return output;
}

PS_LIGHT_CUBE_INPUT VSLightCube(VS_PHONG_INPUT input)
{
    PS_LIGHT_CUBE_INPUT output = (PS_LIGHT_CUBE_INPUT)0;
    output.Position = input.Position;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return ((2.0 * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - z * (FAR_PLANE - NEAR_PLANE))) / FAR_PLANE;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSPhong(PS_PHONG_INPUT input) : SV_Target
{
    float3 normal = normalize(input.Normal);

    float attenuation[NUM_LIGHTS];
    float3 attenuationDistanceSquared = float3(0.0f, 0.0f, 0.0f);

    for (uint i = 0; i < NUM_LIGHTS; ++i)
    {
        float attenuationDistance = distance(LightPositions[i].xyz, input.WorldPosition);
        attenuationDistanceSquared += dot(attenuationDistance, attenuationDistance);
        attenuation[i] = AttenuationDistance[i].zw / (attenuationDistanceSquared + 0.000001f);
    }

    if (HasNormalMap)
    {
        float4 bumpMap = normalMapTexture.Sample(normalMapSampler, input.TexCoord);

        bumpMap = (bumpMap * 2.0f) - 1.0f;

        float3 bumpNormal = bumpMap.x * input.Tangent + bumpMap.y * input.Bitangent + bumpMap.z * normal;
        normal = normalize(bumpNormal);
    }

    float4 albedo = txDiffuse.Sample(sampState, input.TexCoord);

    float2 depthTexCoord =
    {
        input.LightViewPosition.x / input.LightViewPosition.w / 2.0f + 0.5f,
        -input.LightViewPosition.y / input.LightViewPosition.w / 2.0f + 0.5f
    };

    float closestDepth = shadowMapTexture.Sample(shadowMapSampler, depthTexCoord).r;
    closestDepth = LinearizeDepth(closestDepth);

    float currentDepth = input.LightViewPosition.z / input.LightViewPosition.w;
    currentDepth = LinearizeDepth(currentDepth);

    /* shading */
    // ambient light
    float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedo.rgb;

    for (uint i = 0u; i < NUM_LIGHTS; ++i)
    {
        ambient += float3(0.1f, 0.1f, 0.1f) * LightColors[i].xyz * attenuation[i];
    }

    // diffuse light
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 lightDirection = float3(0.0f, 0.0f, 0.0f);

    for (uint i = 0; i < NUM_LIGHTS; ++i)
    {
        lightDirection = normalize(LightPositions[i].xyz - input.WorldPosition);
        diffuse += saturate(dot(normal, lightDirection)) * LightColors[i] * attenuation[i];
    }

    // specular light
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    float3 viewDirection = normalize(CameraPosition.xyz - input.WorldPosition);

    for (uint i = 0; i < NUM_LIGHTS; ++i)
    {
        float3 lightDirection = normalize(LightPositions[i].xyz - input.WorldPosition);
        float3 reflectDirection = reflect(-lightDirection, input.Normal);

        specular += pow(saturate(dot(reflectDirection, viewDirection)), 40.0f)
            * LightColors[i] // color of the light
            * albedo.rgb // color sampled from the texture
            * attenuation[i];
    }

    return float4(ambient + diffuse + specular, 1.0f) * albedo;
}

float4 PSLightCube(PS_LIGHT_CUBE_INPUT input) : SV_Target
{
    return OutputColor;
}