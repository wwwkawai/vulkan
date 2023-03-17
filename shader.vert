#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec2 inTexcoord;
layout (location = 0) out vec2 outTexcoord;
layout (location = 1) out vec3 fragColor;

layout (push_constant) uniform PushConstant {
    mat4 model;
} pc;
layout (set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;
void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(position, 1.);
    fragColor = vertexColor;
    outTexcoord = inTexcoord;
}