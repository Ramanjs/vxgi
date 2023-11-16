#version 420 core

in vec2 texCoordFrag;

uniform sampler2D tex;

out vec4 outColor;

void main() {
  outColor = texture(tex, texCoordFrag);
}
