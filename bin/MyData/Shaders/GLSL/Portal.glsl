#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "Lighting.glsl"

varying vec2 vScreenPos;

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vScreenPos = GetScreenPosPreDiv(gl_Position);
    vScreenPos = vec2(vScreenPos.x, 1 - vScreenPos.y); // strange bug when screen is vertically mirrored
}

void PS()
{
	// DiffMap - WorldA = tex unit 0
	// NormalMap - WorldP = tex unit 1
	// SpecMap - WorldB = tex unit 2

	vec3 MaskRgb = texture2D(sNormalMap, vScreenPos).rgb;
	float MaskIntensity = GetIntensity(MaskRgb);
	
	vec3 Wa = texture2D(sDiffMap, vScreenPos).rgb;
	vec3 Wb = texture2D(sSpecMap, vScreenPos).rgb;

	gl_FragColor = vec4(mix(Wa, Wb, MaskIntensity), 1.0);
}
