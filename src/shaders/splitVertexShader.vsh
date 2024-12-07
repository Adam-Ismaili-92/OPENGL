#version 450
in vec3 position;

uniform vec4 objectColor;
uniform vec3 cameraUp;
uniform vec3 cameraRight;
uniform mat4 projectionMatrix;

void main() {
    vec4 projectionPos = projectionMatrix * (vec4(position.x * cameraRight, 1) + vec4(position.y * cameraUp, 1));
    gl_Position = projectionPos;
}
