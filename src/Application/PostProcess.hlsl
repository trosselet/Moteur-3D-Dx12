Texture2D sceneTexture : register(t0);
SamplerState samplerState : register(s0);

cbuffer PostProcessParams : register(b0)
{
    float brightness;
    float contrast;
    float saturation;
    float damageIntensity;
};

float3 AdjustSaturation(float3 color, float saturation)
{
    float luminance = dot(color, float3(0.299, 0.587, 0.114));
    return lerp(float3(luminance, luminance, luminance), color, saturation);
}

float3 AdjustContrast(float3 color, float contrast)
{
    return (color - 0.5) * contrast + 0.5;
}

float3 AdjustBrightness(float3 color, float brightness)
{
    return color + brightness;
}

float ApplyCoDEffect(float2 uv, float intensity)
{
    float2 center = float2(0.5, 0.5);
    float dist = length(uv - center);
    float effect = pow(saturate(dist), 0.7);

    return effect * intensity;
}


float3 ApplyDamageEffect(float3 color, float uvVignette, float intensity)
{
    float3 redOverlay = float3(1.0, 0.0, 0.0);
    return lerp(color, redOverlay, uvVignette * intensity);
}

struct VSInput
{
    float3 position : POSITION;
    float2 uv : UV;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PSInput vsmain(VSInput input)
{
    PSInput output;
    output.position = float4(input.position, 1.0);
    output.uv = input.uv;
    return output;
}

float4 psmain(PSInput input) : SV_TARGET
{
    float4 color = sceneTexture.Sample(samplerState, input.uv);
    
    color.rgb = AdjustSaturation(color.rgb, saturation);
    color.rgb = AdjustContrast(color.rgb, contrast);
    color.rgb = AdjustBrightness(color.rgb, brightness);
    
    float coDEffect = ApplyCoDEffect(input.uv, damageIntensity);
    
    color.rgb = ApplyDamageEffect(color.rgb, coDEffect, damageIntensity);

    return color;
}
