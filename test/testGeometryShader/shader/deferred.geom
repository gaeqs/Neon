#version 460

layout(points) in;
layout(triangle_strip, max_vertices = 10) out;

layout (location = 0) in mat4 model[];

layout (binding = 0) uniform Matrices
{
    mat4 view;
    mat4 viewProjection;
    mat4 inverseProjection;
    float near;
    float far;
};

void emit(vec4 pos) {
    gl_Position = viewProjection * model[0] * pos;
    EmitVertex();
}

void main() {
    vec4 position = gl_in[0].gl_Position;
    emit(position + vec4(-0.2, -0.2, 0.0, 0.0));
    emit(position + vec4(0.2, -0.2, 0.0, 0.0));
    emit(position + vec4(-0.2, 0.2, 0.0, 0.0));
    emit(position + vec4(0.2, 0.2, 0.0, 0.0));
    emit(position + vec4(0.0, 0.4, 0.0, 0.0));
    EndPrimitive();
}