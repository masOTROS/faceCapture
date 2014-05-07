//
// Fragment shader for EarlyBird
//
 
 varying vec2 textureCoordinate;
 varying vec2 textureCoordinateNorm;

 
 uniform sampler2DRect inputImageTexture;
 uniform sampler2DRect inputImageTexture2; //earlyBirdCurves
 uniform sampler2DRect inputImageTexture3; //earlyBirdOverlay
 uniform sampler2DRect inputImageTexture4; //vig
 uniform sampler2DRect inputImageTexture5; //earlyBirdBlowout
 uniform sampler2DRect inputImageTexture6; //earlyBirdMap
 
 const mat3 saturate = mat3(
                            1.210300,
                            -0.089700,
                            -0.091000,
                            -0.176100,
                            1.123900,
                            -0.177400,
                            -0.034200,
                            -0.034200,
                            1.265800);
 const vec3 rgbPrime = vec3(0.25098, 0.14640522, 0.0); 
 const vec3 desaturate = vec3(.3, .59, .11);
 
 void main()
 {
     
     vec3 texel = texture2DRect(inputImageTexture, textureCoordinate).rgb;
     
     
     vec2 lookup;    
     lookup.y = 0.5;
     
     lookup.x = 256.*texel.r;
     texel.r = texture2DRect(inputImageTexture2, lookup).r;
     
     lookup.x = 256.*texel.g;
     texel.g = texture2DRect(inputImageTexture2, lookup).g;
     
     lookup.x = 256.*texel.b;
     texel.b = texture2DRect(inputImageTexture2, lookup).b;
     
     float desaturatedColor;
     vec3 result;
     desaturatedColor = dot(desaturate, texel);
     
     
     lookup.x = 256.*desaturatedColor;
     result.r = texture2DRect(inputImageTexture3, lookup).r;
     lookup.x = 256.*desaturatedColor;
     result.g = texture2DRect(inputImageTexture3, lookup).g;
     lookup.x = 256.*desaturatedColor;
     result.b = texture2DRect(inputImageTexture3, lookup).b;
     
     texel = saturate * mix(texel, result, .5);
     
     vec2 tc = (2.0 * textureCoordinate) - 1.0;
     float d = dot(tc, tc);
     
     vec3 sampled;
     lookup.y = .5;
     
     /*
      lookup.x = texel.r;
      sampled.r = texture2DRect(inputImageTexture4, lookup).r;
      
      lookup.x = texel.g;
      sampled.g = texture2DRect(inputImageTexture4, lookup).g;
      
      lookup.x = texel.b;
      sampled.b = texture2DRect(inputImageTexture4, lookup).b;
      
      float value = smoothstep(0.0, 1.25, pow(d, 1.35)/1.65);
      texel = mix(texel, sampled, value);
      */
     
     //---
     
     lookup = vec2(d, texel.r);
     texel.r = texture2DRect(inputImageTexture4, 255.*lookup).r;
     lookup.y = texel.g;
     texel.g = texture2DRect(inputImageTexture4, 255.*lookup).g;
     lookup.y = texel.b;
     texel.b	= texture2DRect(inputImageTexture4, 255.*lookup).b;
     float value = smoothstep(0.0, 1.25, pow(d, 1.35)/1.65);
     
     //---
     
     lookup.x = 256.*texel.r;
     sampled.r = texture2DRect(inputImageTexture5, lookup).r;
     lookup.x = 256.*texel.g;
     sampled.g = texture2DRect(inputImageTexture5, lookup).g;
     lookup.x = 256.*texel.b;
     sampled.b = texture2DRect(inputImageTexture5, lookup).b;
     texel = mix(sampled, texel, value);
     
     
     lookup.x = 256.*texel.r;
     texel.r = texture2DRect(inputImageTexture6, lookup).r;
     lookup.x = 256.*texel.g;
     texel.g = texture2DRect(inputImageTexture6, lookup).g;
     lookup.x = 256.*texel.b;
     texel.b = texture2DRect(inputImageTexture6, lookup).b;
     
     gl_FragColor = vec4(texel, 1.0);
 }
