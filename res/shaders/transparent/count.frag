#version 430 core

#include <res/shaders/common/extensions.frag>

layout(early_fragment_tests) in;

in vec3 fPosition;
in vec3 fNormal;
in vec2 fUV;
in vec3 fColor;
in float fZ;
flat in int fMatIndex;

#include <res/shaders/common/transparancy.frag>

void main() {
    doZPrePass(gl_FragCoord.z);

    atomicCounterIncrement(counter);
}
