#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"
#include "Lighting.hlsl"

void VS(float4 iPos : POSITION,
    out float2 oScreenPos : TEXCOORD0,
    out float4 oPos : OUTPOSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oScreenPos = GetScreenPosPreDiv(oPos);
}

void PS(float2 iScreenPos : TEXCOORD0,
    out float4 oColor : OUTCOLOR0)
{
	// DiffMap - WorldA = tex unit 0
	// NormalMap - WorldP = tex unit 1
	// SpecMap - WorldB = tex unit 2

	float3 MaskRgb = Sample2D(NormalMap, iScreenPos).rgb;
	float MaskIntensity = GetIntensity(MaskRgb);
	
	float3 Wa = Sample2D(DiffMap, iScreenPos).rgb;
	float3 Wb = Sample2D(SpecMap, iScreenPos).rgb;

	oColor = float4(lerp(Wa, Wb, MaskIntensity), 1.0);

	//oColor = float4(lerp(float3(1.0, 1.0, 1.0), float3(1.0, 0.0, 0.0) , MaskIntensity), 1.0);

	//oColor = float4(1.0, 1.0, 1.0, 1.0);
	
	//oColor = float4(MaskRgb, 1.0);

}
