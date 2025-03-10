struct VSInput
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 textCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 directionToCamera : TEXCOORD2;
};

cbuffer cb : register(b0)
{
    row_major float4x4 m_world;
    row_major float4x4 m_view;
    row_major float4x4 m_proj;
    float4 mLightPosition;
    float4 mCameraPosition;
}

VSOutput vsmain(VSInput vin)
{
    VSOutput vout;
    
    vout.position = mul(float4(vin.position, 1.0f), m_world);
    vout.position = mul(vout.position, m_view);
    vout.position = mul(vout.position, m_proj);

    
    vout.worldPosition = mul(float4(vin.position, 1.0f), m_world).xyz;
    vout.worldPosition.z = 0.0f;
    
    vout.textCoord = vin.uv;
    
    float3x3 normalMatrix = (float3x3) m_world;
    vout.normal = mul(vin.normal, normalMatrix);
    vout.normal = normalize(vout.normal);
    
    vout.directionToCamera = normalize((float3) mCameraPosition - vout.worldPosition);

    return vout;
}
