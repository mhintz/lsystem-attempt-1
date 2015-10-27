#version 150

// Modified from Cinder's FrustumCullingApp sample

uniform mat4  ciModelViewProjection;
uniform mat4  ciModelView;
uniform mat3  ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec3 ciColor;

out vec4 vertViewPosition;
out vec3 vertViewNormal;
out vec3 vertColor;

void main()
{
  vertViewPosition = ciModelView * ciPosition;
  vertViewNormal = ciNormalMatrix * ciNormal;
  vertColor = ciColor;
  gl_Position = ciModelViewProjection * ciPosition;
}
