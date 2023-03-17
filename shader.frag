#version 450

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 outTexcoord;
layout(location = 1) in vec3 fragColor;

layout(set = 1, binding = 0) uniform sampler2D Sampler;



void main() {
    outColor = texture(Sampler, outTexcoord);
}