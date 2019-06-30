#version 460 core

#pragma debug(on)

uniform vec4 vertexColor;

uniform mat4 modelMatrix;
uniform mat4 modelInverseTransposedMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 cameraPositionInWorld;

uniform vec3 lightPositionInWorld;

uniform float ambientLightIntensity;

in vec3 vertexPosition;
in vec3 vertexNormal;

out vec4 pInWorld;
out vec4 nInWorld;

out vec4 fragmentColor;

void main(void) {
  pInWorld = modelMatrix * vec4(vertexPosition, 1.0);
  nInWorld = normalize(modelInverseTransposedMatrix * vec4(vertexNormal, 0.0));
  gl_Position = projectionMatrix * viewMatrix * pInWorld;
  fragmentColor = vertexColor;
}
