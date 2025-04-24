struct VertexIn
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


VertexOut vsmain(VertexIn vin)
{
    VertexOut vout;
    vout.position = float4(vin.position, 1.0f);
    vout.color = vin.color;
    return vout;
}

float4 psmain(VertexOut pin) : SV_TARGET
{
    return pin.color;
}