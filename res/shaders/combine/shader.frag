#version 430 core

#include <res/shaders/common/extensions.frag>

out vec4 gFragColor;

#define LINKED_LIST_READONLY
#include <res/shaders/common/transparancy.frag>

uniform sampler2D opaqueColor;
uniform sampler2D opaqueZBuffer;

void main() {
    uint headIndex = calculateHead();
    uint listIndex = tail[headIndex];

    uint currentIndex = listIndex;
    uint count;

    for (count = 0; count < 512; count++) {
        if (currentIndex == 0) break;

        currentIndex = fragments[currentIndex].next-1;
    }

    float minDepth = 0.0f;
    float currentDepth;
    uint closestIndex;

    vec3 color = texelFetch(opaqueColor, ivec2(gl_FragCoord.xy), 0).rgb;

    for (uint i = 0; i < count; i++) {
        currentDepth = 10000000.0f;
        closestIndex = 0xffffffff;
        currentIndex = listIndex;

        for (uint j = 0; j < count; j++) {
            if (fragments[currentIndex].depth >= minDepth && fragments[currentIndex].depth < currentDepth) {
                currentDepth = fragments[currentIndex].depth;
                closestIndex = currentIndex;
            }

            currentIndex = fragments[currentIndex].next-1;
        }

        if (closestIndex != 0xffffffff) {
            color = fragments[closestIndex].color.rgb * fragments[closestIndex].color.a +
                (1.0 - fragments[closestIndex].color.a) * color;
        }
    }

    gFragColor = vec4(color, 1.0);
}
