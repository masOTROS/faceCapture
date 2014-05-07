varying vec2 texcoord;
// samplers
uniform sampler2DRect tex;
uniform sampler2DRect shape;
//variables?
uniform float width;
uniform float height;
// entry point
void
main()
{
    vec2 pos = texcoord;
    if( texture2DRect(shape,texcoord).r > 0.0 ) pos.y=height-pos.y;
    gl_FragColor = texture2DRect(tex,pos);
}
