#version 460 core

#pragma debug(on)

in vec4 fragmentColor;

layout(location = 0) out vec4 pixelColor;

void main(void) {
  pixelColor = fragmentColor;
}
