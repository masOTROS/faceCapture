varying vec2 texcoord;
void main()
{
    gl_Position = ftransform();
    texcoord = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
}