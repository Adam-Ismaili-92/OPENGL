#version 450
// Shader to split normal fragment and bright color
layout(location = 0) out vec4 FragmentColor;
layout(location = 1) out vec4 BrightColor;

uniform vec4 objectColor;
const float threshold = 0.5;
void main() {
    FragmentColor = objectColor;
    if (length(objectColor.rgb) > threshold)
        BrightColor = objectColor;
    else
        BrightColor = vec4(0.0f, 0.0f, 0.0f, objectColor.a);
}
