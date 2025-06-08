#version 330

out vec4 pixelColor;

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;
uniform sampler2D textureMap2;
uniform sampler2D textureMap3;

uniform int textureIndex; // Określa, którą teksturę wybrać (0–3)

in vec4 ic;
in vec4 n;
in vec4 l1;  // Light vector for first light
in vec4 l2;  // Light vector for second light
in vec4 v;
in vec2 iTexCoord0;

void main(void) {
    vec4 ml1 = normalize(l1);
    vec4 ml2 = normalize(l2);
    vec4 mn = normalize(n);
    vec4 mv = normalize(v);

    vec4 mr1 = reflect(-ml1, mn);
    vec4 mr2 = reflect(-ml2, mn);

    vec4 texColor;
    if (textureIndex == 0)
        texColor = texture(textureMap0, iTexCoord0);
    else if (textureIndex == 1)
        texColor = texture(textureMap1, iTexCoord0);
    else if (textureIndex == 2)
        texColor = texture(textureMap2, iTexCoord0);
    else
        texColor = texture(textureMap3, iTexCoord0);

    vec4 kd = texColor;
    vec4 ks = vec4(1.0);

    float nl1 = clamp(dot(mn, ml1), 0.0, 1.0);
    float rv1 = pow(clamp(dot(mr1, mv), 0.0, 1.0), 25.0);

    float nl2 = clamp(dot(mn, ml2), 0.0, 1.0);
    float rv2 = pow(clamp(dot(mr2, mv), 0.0, 1.0), 25.0);

    vec3 diffuse = kd.rgb * (nl1 + nl2);
    vec3 specular = ks.rgb * (rv1 + rv2);

    pixelColor = vec4(diffuse + specular, kd.a);

}