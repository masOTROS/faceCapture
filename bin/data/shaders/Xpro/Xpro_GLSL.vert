//
// Vertex shader for Xpro
//
varying vec2 texcoord;
varying vec2 norm;
void main (void)
{
    gl_Position = ftransform();
    texcoord = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
    norm = gl_MultiTexCoord0.xy;
}