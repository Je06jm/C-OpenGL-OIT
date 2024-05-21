uniform sampler2D matDiffuse;
uniform vec3 matDiffuseColor;
uniform sampler2D matSpecular;
uniform vec3 matSpecularColor;
uniform sampler2D matAlpha;
uniform float matAlphaValue;

uniform uint matMask;

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

vec4 calculateLighting(float alphaClip) {
    vec3 color = matDiffuseColor;
    vec3 specular = matSpecularColor;
    float alpha = matAlphaValue;

    if ((matMask & USE_ALPHA_TEXTURE) != 0) {
        alpha = texture(matAlpha, fUV).r;
    }

    if (alpha < alphaClip) {
        discard;
    }

    if ((matMask & USE_DIFFUSE_TEXTURE) != 0) {
       color = texture(matDiffuse, fUV).rgb;
    }

    if ((matMask & USE_SPECULAR_TEXTURE) != 0) {
        specular = texture(matSpecular, fUV).rgb;
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
