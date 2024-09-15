#version 460

layout (triangles) in;
layout(triangle_strip, max_vertices = 18) out;

layout (location = 0) in vec2 texCoord[];
layout (location = 0) out vec2 fragTexCoord;
layout (location = 1) out vec3 fragWorldDir;

vec3 getWorldDir (int layer, vec2 coords) {
    switch (layer) {
        case 0:
        return vec3(1.0f, -coords.y, -coords.x);
        case 1:
        return vec3(-1.0f, -coords.y, coords.x);
        case 2:
        return vec3(coords.x, 1.0f, coords.y);
        case 3:
        return vec3(coords.x, -1.0f, -coords.y);
        case 4:
        return vec3(coords.x, -coords.y, 1.0f);
        case 5:
        return vec3(-coords.x, -coords.y, -1.0f);
        default :
        return vec3(0.0f, 0.0f, 0.0f);
    }
}

void main () {
    for (int i = 0; i < 6; i++) {
        gl_Layer = i;
        gl_Position = gl_in[0].gl_Position;
        fragTexCoord = texCoord[0];
        fragWorldDir = normalize(getWorldDir(i, texCoord[0]));
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        fragTexCoord = texCoord[1];
        fragWorldDir = normalize(getWorldDir(i, texCoord[1]));
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        fragTexCoord = texCoord[2];
        fragWorldDir = normalize(getWorldDir(i, texCoord[2]));
        EmitVertex();
        EndPrimitive();
    }
}
