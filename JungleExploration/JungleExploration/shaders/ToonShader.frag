#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
	vec3 DlightDir;		// direction of the direct light
	vec3 DlightColor;	// color of the direct light
	vec3 AmbLightColor;	// ambient light
	vec3 eyePos;		// position of the viewer
} gubo;

layout(set = 1, binding = 0) uniform UniformBufferObject {
	float visible;
	float amb;
	float gamma;
	vec3 sColor;
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

layout(set = 1, binding = 1) uniform sampler2D tex;

vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms, float gamma) {
	//vec3 V  - direction of the viewer
	//vec3 N  - normal vector to the surface
	//vec3 L  - light vector (from the light model)
	//vec3 Md - main color of the surface
	//vec3 Ms - specular color of the surface
	if(dot(L, N) >= 0.5f)
	{
		Md = Md;	
	}
	else if(dot(L, N) < 0.6f && 0 <= dot(L, N))
	{
		Md = Md / 4.0f;	
	}
	else if(dot(L, N) < 0)
	{
		Md = vec3(0.0f);	
	}
	vec3 rlx = 2 * N * dot(L, N) - L;
	if(dot(rlx, V) >= 0.9f)
	{
		Ms = Ms;	
	}
	else if(dot(rlx, V) < 0.9f)
	{
		Ms = vec3(0.0f);	
	}
	vec3 specular = Ms;
	vec3 diffuse = Md;
	return specular + diffuse;
}

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
	
	vec3 lightDir = gubo.DlightDir;
	vec3 lightColor = gubo.DlightColor.rgb;

	vec3 DiffSpec = BRDF(EyeDir, Norm, lightDir, texture(tex, fragUV).rgb, ubo.sColor, ubo.gamma);
	vec3 Ambient = texture(tex, fragUV).rgb * ubo.amb * gubo.AmbLightColor;
	
	outColor = vec4(clamp(0.95 * (DiffSpec) * lightColor.rgb + Ambient,0.0,1.0), 1.0f);
}