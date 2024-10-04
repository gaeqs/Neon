#version 460

const int INVOCATIONS = 10;

layout(points, invocations = INVOCATIONS) in;
layout(triangle_strip, max_vertices = 256) out;

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

void strip(vec4 start, float x1, float x2) {
    const float PI2 = 6.28318530718;
    const int ITERATIONS = 15;
    for (int i = 0; i < ITERATIONS; ++i) {
        float fi = float(i);
        float fir = fi * PI2 / (ITERATIONS - 1);

        float z = cos(fir);
        float y = sin(fir);

        float cosX1 = sqrt(cos(x1 * PI2 / 4));
        float cosX2 = sqrt(cos(x2 * PI2 / 4));

        emit(start + vec4(x1, y * cosX1, z * cosX1, 0.0f));
        emit(start + vec4(x2, y * cosX2, z * cosX2, 0.0f));
    }
}

void main() {
    vec4 position = gl_in[0].gl_Position;


    float x1 = float(gl_InvocationID) * 2.0f / INVOCATIONS - 1.0f;
    float x2 = float(gl_InvocationID + 1) * 2.0f / INVOCATIONS - 1.0f;
    strip(position, x1, x2);
    EndPrimitive();
}