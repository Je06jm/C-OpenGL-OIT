#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aUV;
layout (location = 5) in vec3 aColor;

uniform mat4 PV;
uniform mat4 V;
uniform mat4 M;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fUV;
out vec3 fColor;
out float fZ;

out mat3 TBN;

void main() {
    vec4 pos = M * vec4(aPosition, 1.0);
    fPosition = pos.xyz / pos.w;
    fNormal = mat3(transpose(inverse(M))) * aNormal;
    fUV = aUV;
    fColor = aColor;
    vec4 zPos = V * M * vec4(aPosition, 1.0);
    fZ = zPos.z / zPos.w;

    vec3 T = normalize(vec3(M * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(M * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(M * vec4(aTangent, 0.0)));

    TBN = mat3(T, B, N);

    gl_Position = PV * pos;
}
