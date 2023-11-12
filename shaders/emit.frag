#version 440 core

out vec4 FragColor;

uniform vec4 emitColor;

void main() {
    FragColor = emitColor;
}
