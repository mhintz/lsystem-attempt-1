#version 150

// Modified from Cinder's FrustumCullingApp sample

in vec4 vertViewPosition;
in vec3 vertViewNormal;
in vec3 vertColor;

out vec4 fragColor;

void main()
{
  // Simulate a light at the camera position in view space
  const vec3 cLightPosition = vec3( 0 );

  // Calculate lighting vectors
  vec3 posToLight = normalize( cLightPosition - vertViewPosition.xyz );
  vec3 normal = normalize( vertViewNormal );

  // Diffuse component
  vec3 diffuse = vertColor * max( dot( normal, posToLight ), 0.0 );

  // Output final color.
  // fragColor = vec4( diffuse, 1.0 );

  fragColor = vec4(vertViewNormal, 1.0);
}
