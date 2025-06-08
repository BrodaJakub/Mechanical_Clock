#version 330

out vec4 pixelColor;

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;
uniform sampler2D textureMap2;
uniform sampler2D textureMap3;

uniform int textureIndex; // <== określa, którą teksturę wybrać (0–3)

in vec4 ic;
in vec4 n;
in vec4 l;
in vec4 v;
in vec2 iTexCoord0;

void main(void) {
    vec4 ml = normalize(l);
    vec4 mn = normalize(n);
    vec4 mv = normalize(v);
    vec4 mr = reflect(-ml, mn);

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

    float nl = clamp(dot(mn, ml), 0.0, 5.0);
    float rv = pow(clamp(dot(mr, mv), 0.0, 1.0), 25.0);

    pixelColor = vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb * rv, 0.0);
}
