//
// Vertex shader for EarlyBird
//
varying vec2 textureCoordinate;
varying vec2 textureCoordinateNorm;
void main (void)
{
    gl_Position = ftransform();
  textureCoordinate = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
  textureCoordinateNorm = gl_MultiTexCoord0.xy;
}