#version 450

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 vertexColor;
layout (location = 1) out vec3 fragColor;
layout (set = 0, binding = 0) uniform UBO {
    mat4 proj;
    mat4 view;
    mat4 model;
} ubo;
void main() {
    gl_Position = ubo.view * ubo.proj * ubo.model * vec4(position, .0, 1.);
    fragColor = vertexColor;
}