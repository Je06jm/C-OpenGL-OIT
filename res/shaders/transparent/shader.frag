#version 430 core

#include <res/shaders/common/extensions.frag>

layout(early_fragment_tests) in;

in vec3 fPosition;
in vec3 fNormal;
in vec2 fUV;
in vec3 fColor;
flat in int fMatIndex;

in mat3 TBN;

#include <res/shaders/common/lighting.frag>
#include <res/shaders/common/transparancy.frag>

void main() {
    doZPrePass(gl_FragCoord.z);

    uint headIndex = calculateHead();
    uint listIndex;
    uint lastIndex = tail[headIndex];

    vec4 color = calculateLighting(0.0);

    listIndex = atomicCounterIncrement(counter);
    lastIndex = atomicExchange(tail[headIndex], listIndex);

    fragments[listIndex].next = lastIndex+1;
    fragments[listIndex].color = color;
    fragments[listIndex].depth = gl_FragCoord.z;
}
