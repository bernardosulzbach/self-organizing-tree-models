#version 460 core

#pragma debug(on)

uniform mat4 modelMatrix;
uniform mat4 modelInverseTransposedMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 cameraPositionInWorld;

uniform vec3 lightPositionInWorld;

uniform float ambientLightIntensity;

in vec4 pInWorld;
in vec4 nInWorld;

in vec4 fragmentColor;

layout(location = 0) out vec4 pixelColor;

float evaluateAttenuation(float d) {
  const float attenuationFlatCoefficient = 0.1;
  const float attenuationLinearCoefficient = 0.1;
  const float attenuationSquareCoefficient = 0.01;
  return min(1, 1 / (attenuationFlatCoefficient + attenuationLinearCoefficient * d + attenuationSquareCoefficient * d * d));
}

vec3 phongWithSpecular(vec4 light, vec4 normal, vec4 reflection, vec4 view, vec3 color, float distance) {
  const float attenuation = evaluateAttenuation(distance);
  const float specularCoefficient = 0.1;
  const float surfaceSpecularity = 0.1;
  const float specularComponent = attenuation * specularCoefficient * pow(max(0.0, dot(view, reflection)), surfaceSpecularity);
  return ambientLightIntensity * color + attenuation * max(0.0, dot(normal, light)) * color + specularComponent;
}

void main(void) {
  const vec4 lInWorld = normalize(vec4(lightPositionInWorld, 1.0) - pInWorld);
  const vec4 rInWorld = normalize(2.0 * (dot(nInWorld, lInWorld) * nInWorld) - lInWorld);
  const vec4 vInWorld = normalize(vec4(cameraPositionInWorld, 1.0) - pInWorld);
  const float distance = distance(vec4(cameraPositionInWorld, 1.0), pInWorld);
  pixelColor = vec4(phongWithSpecular(lInWorld, nInWorld, rInWorld, vInWorld, vec3(fragmentColor), distance), 1.0);
}
