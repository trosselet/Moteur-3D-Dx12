Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_SPOT 1
#define LIGHT_TYPE_POINT 2
#define MAX_LIGHTS 10

struct VertexIn
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : POSITION1;
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

cbuffer cbMaterial : register(b2)
{
    float4 ambientLightColor;
    float4 ambientLight;
    float4 diffuseLight;
    float4 specularLight;
    float shininess;
    float padding[3];
};

cbuffer cbLight : register(b3)
{
    float3 lightColor;
    int type;
    float3 lightDirection;
    float intensity;
    float3 lightPosition;
    float spotLightAngle;
    float padding2[4];
};

VertexOut vsmain(VertexIn vin)
{
    VertexOut vout;
    vout.position = mul(mul(mul(float4(vin.position, 1.0f), gWorld), gView), gProj);
    vout.color = vin.color;
    vout.texcoord = vin.texcoord;
    vout.normal = mul((float3x3) gWorld, vin.normal);
    vout.worldPos = mul(float4(vin.position, 1.0f), gWorld).xyz;
    
    return vout;
}

float4 psmain(VertexOut pin) : SV_TARGET
{
    float3 normal = normalize(pin.normal);
    float3 viewDir = normalize(-pin.worldPos);

    float3 ambient = ambientLight.rgb * ambientLightColor.rgb;

    float3 lightDir = float3(0.0f, -1.0f, 0.0f);
    float attenuation = 1.0;
    float spotFactor = 1.0;

    if (type == LIGHT_TYPE_DIRECTIONAL)
    {
        lightDir = normalize(-lightDirection);
    }
    else if (type == LIGHT_TYPE_SPOT)
    {
        float3 lightToFrag = pin.worldPos - lightPosition;
        float distance = length(lightToFrag);
        lightDir = normalize(-lightToFrag);
        attenuation = 1.0 / (distance * distance);

        float3 spotDir = normalize(-lightDirection);
        float angle = dot(lightDir, spotDir);

        float innerCone = cos(spotLightAngle * 0.8);
        float outerCone = cos(spotLightAngle);
        spotFactor = saturate((angle - outerCone) / (innerCone - outerCone));
    }
    else if (type == LIGHT_TYPE_POINT)
    {
        float3 lightToFrag = pin.worldPos - lightPosition;
        float distance = length(lightToFrag);
        lightDir = normalize(-lightToFrag);
        attenuation = 1.0 / (distance * distance);
    }

    float diff = max(dot(normal, lightDir), 0.0);
    float3 diffuse = diffuseLight.rgb * diff * lightColor.rgb;

    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), shininess);
    float3 specular = specularLight.rgb * spec * lightColor.rgb;

    float3 lighting = ambient + (diffuse + specular) * intensity * attenuation * spotFactor;

    float4 texColor = gTexture.Sample(gSampler, pin.texcoord);
    float3 finalColor = texColor.rgb * lighting * pin.color.rgb;

    return float4(finalColor, texColor.a * pin.color.a);
}
