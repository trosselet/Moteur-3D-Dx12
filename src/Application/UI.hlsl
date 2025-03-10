cbuffer cb : register(b0)
{
    float4x4 m_world;
    float4x4 m_view;
    float4x4 m_proj;
};

struct VSInput
{
    float3 position : POSITION;
    float2 uv : UV;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

PSInput vsmain(VSInput input)
{
    PSInput output;

    float4 worldPosition = mul(float4(input.position, 1.0f), m_world);
    float4 viewPosition = mul(worldPosition, m_view);
    float4 projPosition = mul(viewPosition, m_proj);

    output.position = projPosition;
    output.uv = input.uv;

    return output;
}

Texture2D uiTexture : register(t0);
SamplerState samplerState : register(s0);

float4 psmain(PSInput input) : SV_TARGET
{
    float4 color = uiTexture.Sample(samplerState, input.uv);
    
    return color;

}
