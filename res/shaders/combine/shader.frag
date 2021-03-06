#version 430 core

#include <res/shaders/common/extensions.frag>

out vec4 gFragColor;

#define LINKED_LIST_READONLY
#include <res/shaders/common/transparancy.frag>

uniform sampler2D opaqueColor;
uniform sampler2D opaqueZBuffer;

void main() {
    Fragment sortedList[8] = Fragment[](
        Fragment(0, vec4(0.0), 0.0),
        Fragment(0, vec4(0.0), 0.0),
        Fragment(0, vec4(0.0), 0.0),
        Fragment(0, vec4(0.0), 0.0),
        Fragment(0, vec4(0.0), 0.0),
        Fragment(0, vec4(0.0), 0.0),
        Fragment(0, vec4(0.0), 0.0),
        Fragment(0, vec4(0.0), 0.0)
    );

    uint headIndex = calculateHead();
    uint listIndex = tail[headIndex];

    uint sortedIndex = 0;
    uint count;
    uint currentIndex = listIndex;

    for (count = 0; count < 8; count++) {
        if (currentIndex == 0) break;

        sortedList[sortedIndex] = fragments[currentIndex];
        currentIndex = fragments[currentIndex].next-1;
        sortedIndex++;
    }

    currentIndex = listIndex;
    Fragment temp;

    bool allSorted = false;
    for (uint i = 0; i < count-1; i++) {
        allSorted = true;
        for (uint j = 0; j < sortedIndex; j++) {

            if (sortedList[j].depth < sortedList[j+1].depth) {
                temp = sortedList[j];
                sortedList[j] = sortedList[j+1];
                sortedList[j+1] = temp;

                allSorted = false;
            }
        }

        if (allSorted) break;
    }

    vec3 color = texelFetch(opaqueColor, ivec2(gl_FragCoord.xy), 0).rgb;

    for (uint i = 0; i < count; i++) {
        color =
            sortedList[i].color.rgb * sortedList[i].color.a +
            (1.0 - sortedList[i].color.a) * color
        ;
    }

    gFragColor = vec4(color, 1.0);
}
