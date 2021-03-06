uniform sampler2D matTextures[32];
uniform uint mapsMask[32];
uniform vec3 matDiffuse[32];
uniform vec3 matSpecular[32];
uniform float matAlpha[32];

uniform uint textureIndexes[80];

struct Light {
    vec3 position;
    vec3 color;
    float strength;
};

uniform Light lights[4];
uniform uint lightCount;
uniform vec3 globalAmbient;
uniform vec3 viewPosition;

#define USE_DIFFUSE_TEXTURE (1<<0)
#define USE_SPECULAR_TEXTURE (1<<1)
#define USE_ALPHA_TEXTURE (1<<2)

#define DIFFUSE_TEXTURE 0
#define SPECULAR_TEXTURE 1
#define ALPHA_TEXTUER 2
#define STRIDE_TEXTURE 3

#define TEXTURE_INDEX(text) matTextures[textureIndexes[text]]

vec4 calculateLighting(float alphaClip) {
    vec3 color = matDiffuse[fMatIndex];
    vec3 specular = matSpecular[fMatIndex];
    float alpha = matAlpha[fMatIndex];

    int mapIndex = fMatIndex * STRIDE_TEXTURE;

    uint samplerIndex = textureIndexes[mapIndex];

    if ((mapsMask[fMatIndex] & USE_ALPHA_TEXTURE) != 0) {
        alpha = texture(TEXTURE_INDEX(mapIndex+ALPHA_TEXTUER), fUV).r;
    }

    if (alpha < alphaClip) {
        discard;
    }

    if ((mapsMask[fMatIndex] & USE_DIFFUSE_TEXTURE) != 0) {
       color = texture(TEXTURE_INDEX(mapIndex+DIFFUSE_TEXTURE), fUV).rgb;
    }

    if ((mapsMask[fMatIndex] & USE_SPECULAR_TEXTURE) != 0) {
        specular = texture(TEXTURE_INDEX(mapIndex+SPECULAR_TEXTURE), fUV).rgb;
    }

    vec3 finalColor = globalAmbient * color;
    vec3 normal = normalize(fNormal);
    vec3 viewDirection = normalize(viewPosition - fPosition);

    for (uint i = 0; i < lightCount; i++) {
        float lightDistance = distance(lights[i].position, fPosition);
        float lightStrength = lights[i].strength / lightDistance;

        if (lightStrength < 0.01) {
            continue;
        }
        
        vec3 lightDirection = normalize(lights[i].position - fPosition);

        float diffuseAmount = max(dot(normal, lightDirection), 0.0);
        vec3 diffuse = diffuseAmount * lights[i].color * color;

        vec3 reflectDirection = reflect(-lightDirection, normal);

        float specularAmount = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
        vec3 specularColor = specular * specularAmount * lights[i].color;

        finalColor += (diffuse + specularColor) * lightStrength;
    }

    return vec4(finalColor, alpha);
}
