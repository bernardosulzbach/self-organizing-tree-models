#version 460 core

#pragma debug(on)

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform vec4 vertexColor;

in vec3 vertexPosition;

out vec4 fragmentColor;

void main(void) {
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
  fragmentColor = vertexColor;
}
