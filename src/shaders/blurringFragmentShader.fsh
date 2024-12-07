#version 450 core
// Shader to blur bright fragment and pass it to the other pingponging texture
in vec2 textureCoords;
layout(location = 0) out vec4 FragmentColor;

uniform sampler2D inputTexture;
uniform bool horizontal;
uniform float blurSpread;
uniform float xIncrease;
const int radius = 6;
float kernel[radius];

void main()
{
    float x = 0.0f;
    for (int i = 0; i < radius; i++)
    {
        x += xIncrease;
        kernel[i] = exp(-0.5f * pow(x / blurSpread, 2.0f)) / (blurSpread * sqrt(2 * 3.14159265f));
    }
    vec2 textureOffset = 1.0f / textureSize(inputTexture, 0);
    vec3 result = texture(inputTexture, textureCoords).rgb * kernel[0];

    if(horizontal)
    {
        for(int i = 1; i < radius; i++)
        {
            result += texture(inputTexture, textureCoords + vec2(textureOffset.x * i, 0.0)).rgb * kernel[i];
            result += texture(inputTexture, textureCoords - vec2(textureOffset.x * i, 0.0)).rgb * kernel[i];
        }
    }
    else
    {
        for(int i = 1; i < radius; i++)
        {
            result += texture(inputTexture, textureCoords + vec2(0.0, textureOffset.y * i)).rgb * kernel[i];
            result += texture(inputTexture, textureCoords - vec2(0.0, textureOffset.y * i)).rgb * kernel[i];
        }
    }
    FragmentColor = vec4(result, 1.0f);
}
