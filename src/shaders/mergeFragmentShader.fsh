#version 450
// Shader to merge normal fragment and bright color after blur
in vec2 textureCoords;
layout(location = 0) out vec4 FragmentColor;

uniform sampler2D normalSceneTexture;
uniform sampler2D bloomTexture;
uniform float gamma;
uniform float exposure;

void main()
{
    vec3 color = texture(normalSceneTexture, textureCoords).rgb + texture(bloomTexture, textureCoords).rgb;
    vec3 toneMapped = vec3(1.0f) - exp(-color * exposure);

    FragmentColor = vec4(pow(toneMapped, vec3(1.0f / gamma)), 1.0f);
}
