cbuffer cbView : register(b0) //Vertex Shader constant buffer slot 1
{
    matrix viewMatrix;
};

cbuffer cbWorld : register(b1)
{
    matrix worldMatrix;
};

struct VSInput
{
    float3 pos : POSITION0;
    float3 previousPos : POSITION1;
    float age : TEXCOORD0;
    float angle : TEXCOORD1;
    float size : TEXCOORD2;
};

struct GSInput
{
    float4 pos : POSITION;
    float4 previousPos : POSITION1;
    float age : TEXCOORD0;
    float angle : TEXCOORD1;
    float size : TEXCOORD2;
};

GSInput main(VSInput i)
{
    GSInput o = (GSInput) 0;
    //float4 zero = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //zero = -mul(invViewMatrix, zero);
    o.pos = float4(i.pos, 1.0f);
    o.pos = mul(viewMatrix, o.pos);
    o.previousPos = float4(i.previousPos, 1.0f);
    o.previousPos.xyz -= 0.01f;
    o.previousPos = mul(viewMatrix, o.previousPos);
    o.age = i.age;
    o.angle = i.angle;
    o.size = i.size;
    return o;
}