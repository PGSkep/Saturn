#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
	vec4 gl_Position;
};

//layout(location = 0) in vec3 indxPos;
//layout(location = 1) in float padding;
//layout(location = 2) in vec3 vertPos;
//layout(location = 3) in vec2 vertUV;
//layout(location = 4) in vec3 vertNorm;

layout(location = 0) in vec3 indxPos;
layout(location = 1) in vec3 indxColor;
layout(location = 2) in float scale;
layout(location = 3) in vec3 eulers;
layout(location = 4) in vec2 padding;

layout(location = 5) in vec3 vertPos;

layout(location = 6) in vec2 vertUV;
layout(location = 7) in vec3 vertNorm;

// OUT
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;

layout (set = 0, binding = 0) uniform UBO 
{
	mat4 view;
	mat4 projection;
} ubo;

void main()
{
	mat4 model;
	model[0] = vec4(scale, 0, 0, 0);
	model[1] = vec4(0, scale, 0, 0);
	model[2] = vec4(0, 0, scale, 0);
	model[3] = vec4(indxPos.xyz, 1);

	gl_Position = ubo.projection * ubo.view * model * vec4(vertPos, 1.0);

	fragColor = vec3(indxColor);
	fragUV = vertUV;
}