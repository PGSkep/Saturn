#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler2D texSampler;

void main() {
	outColor = vec4(fragColor, 1.0);
	outColor = texture(texSampler, fragUV);
	outColor *= vec4(fragColor, 1.0);
}