Texture2D lightMap : register(t0);
Texture2D shadowMap : register(t1);
SamplerState colorSampler : register(s0);

cbuffer cbSurfaceColor : register(b0)
{
	float4 surfaceColor;
};

cbuffer cbMapTransform : register(b1)
{
	matrix mapMatrix;
};

cbuffer cbLightPS : register(b2)
{
	float4 lightPosPS;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL;
	float4 worldPos : POSITION;
	float3 viewVec : TEXCOORD0;
	float3 lightVec : TEXCOORD1;
};

static const float3 ambientColor = float3(0.3f, 0.3f, 0.3f);
static const float3 kd = 0.7, m = 170.0f;
static const float4 defLightColor = float4(0.3f, 0.3f, 0.3f, 0.0f);

float4 main(PSInput i) : SV_TARGET
{
	//TODO: calculate textre coordinates
	float4 v = mul(mapMatrix,i.worldPos);
	v = float4(v.x / v.w, v.y / v.w, v.z / v.w, 1);
	float3 viewVec = normalize(i.viewVec);
	float3 normal = normalize(i.norm);
	float3 lightVec = normalize(i.lightVec);
	float3 halfVec = normalize(viewVec + lightVec);
	float3 color = surfaceColor.rgb * ambientColor;
	float2 vec = float2(v.x, v.y);
	//TODO: determine light color based on light map, clipping plane and shadow map.
	float4 lightColor = lightMap.Sample(colorSampler, vec);
	float shadow = shadowMap.Sample(colorSampler, vec);
	if (lightColor.x < defLightColor.x || i.worldPos.y > lightPosPS.y || shadow < v.z) 
		lightColor = defLightColor;
	color += lightColor.rgb * surfaceColor.rgb * kd * saturate(dot(normal, lightVec)) +
		lightColor.rgb * lightColor.a * pow(saturate(dot(normal, halfVec)), m);
	return float4(saturate(color), surfaceColor.a);
}