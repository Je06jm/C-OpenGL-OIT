#version 430 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fUV;
in vec3 fColor;
in float fZ;

#include <res/shaders/common/lighting.frag>

layout (location = 0) out vec4 gFragColor;
layout (location = 1) out float gZCoord;

void main() {
    //gl_FragColor = vec4(fColor + fNormal + vec3(fUV, 1.0), 1.0);
    gFragColor = calculateLighting(0.9);
    gZCoord = gl_FragCoord.z;
}
