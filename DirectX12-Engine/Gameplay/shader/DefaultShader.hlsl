Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VertexIn
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer cbPass : register(b0)
{
    float4x4 gViewProj;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 gWorld;
};

VertexOut vsmain(VertexIn vin)
{
    VertexOut vout;
    float4 posW = mul(float4(vin.position, 1.0f), gWorld);
    vout.position = mul(posW, gViewProj);
    vout.color = vin.color;
    vout.texcoord = vin.texcoord;
    return vout;
}

float4 psmain(VertexOut pin) : SV_TARGET
{
    return gTexture.Sample(gSampler, pin.texcoord) * pin.color;
}