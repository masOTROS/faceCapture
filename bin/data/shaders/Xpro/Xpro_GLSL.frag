//
// Fragment shader for Xpro
//
varying vec2 texcoord;
varying vec2 norm;
uniform sampler2DRect tex;

uniform sampler2DRect texMap;
uniform sampler2DRect texVig;

void main (void)
{
    vec4 texel = texture2DRect(tex, texcoord);
     
    vec2 tc = (2.0 * norm) - 1.0;
    float d = dot(tc, tc);
    vec2 lookup = 255.*vec2(d, texel.r);
    texel.r = texture2DRect(texVig, lookup).r;
    lookup.y = 255.*texel.g;
    texel.g = texture2DRect(texVig, lookup).g;
    lookup.y = 255.*texel.b;
    texel.b	= texture2DRect(texVig, lookup).b;
     
    vec2 red = 256.*vec2(texel.r, 0.16666);
    vec2 green = 256.*vec2(texel.g, 0.5);
    vec2 blue = 256.*vec2(texel.b, 0.83333);
    texel.r = texture2DRect(texMap, red).r;
    texel.g = texture2DRect(texMap, green).g;
    texel.b = texture2DRect(texMap, blue).b;
    
    gl_FragColor = texel;
    
}
