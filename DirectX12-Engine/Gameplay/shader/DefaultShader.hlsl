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
    float4x4 gView;
    float4x4 gProj;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 gWorld;
};

VertexOut vsmain(VertexIn vin)
{
    VertexOut vout;
    vout.position = mul(mul(mul(float4(vin.position, 1.0f), gWorld), gView), gProj);
    vout.color = vin.color;
    vout.texcoord = vin.texcoord;
    return vout;
}


float4 psmain(VertexOut pin) : SV_TARGET
{
    return gTexture.Sample(gSampler, pin.texcoord) * pin.color;
}