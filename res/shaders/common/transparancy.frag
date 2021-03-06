struct Fragment {
    uint next;
    vec4 color;
    float depth;
};

uniform layout(binding=0) atomic_uint counter;

#ifdef LINKED_LIST_READONLY
layout(std430, binding=1) readonly buffer head {
    uint tail[];
};

layout(std430, binding=2) readonly buffer list {
    Fragment fragments[];
};
#else
layout(std430, binding=1) coherent buffer head {
    uint tail[];
};

layout(std430, binding=2) writeonly buffer list {
    Fragment fragments[];
};
#endif

uniform uint screenWidth;
uniform uint screenHeight;

uniform sampler2D sceneDepth;

uint calculateHead() {
    uvec2 index = uvec2(gl_FragCoord.xy);
    return index.y * screenWidth + index.x;
}

void doZPrePass(float fDepth) {
    float depth = texelFetch(sceneDepth, ivec2(gl_FragCoord.xy), 0).r;

    if (depth < fDepth) {
        discard;
    }
}
