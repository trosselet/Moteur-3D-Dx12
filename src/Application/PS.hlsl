Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
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

float4 psmain(PSInput pin) : SV_TARGET
{
    float3 normal = normalize(pin.normal);
    float3 lightDir = normalize(mLightPosition.xyz - pin.worldPosition);
    float3 viewDir = normalize(pin.directionToCamera);
    
    // ambiant light
    float ka = 0.9;
    float3 ambient_light = ka * float3(1.0, 1.0, 1.0);
    
    // diffuse light
    float kd = 1;
    float3 id = float3(1.0, 1.0, 1.0);
    float amount_diffuse_light = max(0.0, dot(normal, lightDir));
    float3 diffuse_light = kd * amount_diffuse_light * id;
    
    // Specular light
    float ks = 0.5;
    float3 is = float3(1.0, 1.0, 1.0);
    float3 reflectDir = reflect(-lightDir, normal);
    float shininess = 16.0f;
    float amount_specular_light = pow(max(0.0, dot(viewDir, reflectDir)), shininess);
    float3 specular_light = ks * amount_specular_light * is;
    
    // light attenuation with distance
    float distance = length(mLightPosition.xyz - pin.worldPosition);
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.02 * (distance * distance));
    
    float4 texColor = gTexture.Sample(gSampler, pin.texCoord);
    
    float3 final_light = (ambient_light + (diffuse_light + specular_light) * attenuation) * texColor.rgb;

    return float4(final_light, texColor.a);
    
}
