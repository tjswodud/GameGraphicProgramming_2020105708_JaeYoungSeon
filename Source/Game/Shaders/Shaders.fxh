//--------------------------------------------------------------------------------------
// File: Shadersasd.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

#define NUM_LIGHTS (1)
#define NEAR_PLANE (0.01f)
#define FAR_PLANE (1000.0f)

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
TextureCube environmentMapTexture : register(t0);

Texture2D txDiffuse : register(t1);
SamplerState sampLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnCameraMovement

  Summary:  Constant buffer used for view transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbChangeOnCameraMovement : register(b0)
{
    matrix View;
    float4 CameraPosition;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnResize

  Summary:  Constant buffer used for projection transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangesEveryFrame

  Summary:  Constant buffer used for world transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
    float4 OutputColor;
    bool HasNormalMap;
};

struct PointLight
{
    float4 Position;
    float4 Color;
    matrix View;
    matrix Projection;
    float4 AttenuationDistance;
};

cbuffer cbLights : register(b3)
{
    PointLight aPointLight[NUM_LIGHTS];
};

//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   VS_INPUT

  Summary:  Used as the input to the vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
struct VS_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    row_major matrix mTransform : INSTANCE_TRANSFORM;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_INPUT

  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float4 LightViewPosition : TEXCOORD1;
    float3 Reflection : REFLECTION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VSEnvironmentMap(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    output.Position = input.Position;
    output.Position = mul(output.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(float4(input.Normal, 0.0f), World).xyz;
    output.TexCoord = input.TexCoord;

    output.WorldPosition = mul(input.Position, World).xyz;

    if (HasNormalMap)
    {
        output.Tangent = normalize(mul(float4(input.Tangent, 0.0f), World).xyz);
        output.Bitangent = normalize(mul(float4(input.Bitangent, 0.0f), World).xyz);
    }

    output.Reflection = reflect(normalize(output.WorldPosition - CameraPosition.xyz), normalize(mul(float4(input.Normal, 0.0f), World).xyz));

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSEnvironmentMap(PS_INPUT input) : SV_Target
{
    float4 albedo = txDiffuse.Sample(sampLinear, input.TexCoord);
    float3 environmentMapColor = environmentMapTexture.Sample(sampLinear, input.Reflection).rgb;

    /* shading */
    // ambient light
    float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedo.rgb;

    for (uint i = 0u; i < NUM_LIGHTS; ++i)
    {
        ambient += float3(0.1f, 0.1f, 0.1f) * aPointLight[i].Color.xyz;
    }

    // diffuse light
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 lightDirection = float3(0.0f, 0.0f, 0.0f);

    for (uint j = 0; j < NUM_LIGHTS; ++j)
    {
        lightDirection = normalize(aPointLight[j].Position.xyz - input.WorldPosition);
        diffuse += saturate(dot(input.Normal, lightDirection)) * aPointLight[j].Color;
    }

    // specular light
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    float3 viewDirection = normalize(CameraPosition.xyz - input.WorldPosition);

    for (uint k = 0; k < NUM_LIGHTS; ++k)
    {
        float3 lightDirection = normalize(aPointLight[k].Position.xyz - input.WorldPosition);
        float3 reflectDirection = reflect(-lightDirection, input.Normal);

        specular += pow(saturate(dot(reflectDirection, viewDirection)), 40.0f)
            * aPointLight[k].Color; // color of the light
    }

    return float4(saturate(ambient + diffuse + specular + environmentMapColor * 0.5f), albedo.a);
}