//
// Fragment shader for Toaster
//
varying vec2 textureCoordinate;
varying vec2 textureCoordinateNorm;
 
 uniform sampler2DRect inputImageTexture;
 uniform sampler2DRect inputImageTexture2; //toasterMetal
 uniform sampler2DRect inputImageTexture3; //toasterSoftlight
 uniform sampler2DRect inputImageTexture4; //toasterCurves
 uniform sampler2DRect inputImageTexture5; //toasterOverlayMapWarm
 uniform sampler2DRect inputImageTexture6; //toasterColorshift
 
 void main()
 {
     vec3 texel;
     vec2 lookup;
     vec2 blue;
     vec2 green;
     vec2 red;
     vec4 tmpvar_1;
     tmpvar_1 = texture2DRect (inputImageTexture, textureCoordinate);
     texel = tmpvar_1.xyz;
     vec4 tmpvar_2;
     tmpvar_2 = texture2DRect (inputImageTexture2, 512.*textureCoordinateNorm);
     vec2 tmpvar_3;
     tmpvar_3.x = tmpvar_2.x;
     tmpvar_3.y = tmpvar_1.x;
     texel.x = texture2DRect (inputImageTexture3, 256.*tmpvar_3).x;
     vec2 tmpvar_4;
     tmpvar_4.x = tmpvar_2.y;
     tmpvar_4.y = tmpvar_1.y;
     texel.y = texture2DRect (inputImageTexture3, 256.*tmpvar_4).y;
     vec2 tmpvar_5;
     tmpvar_5.x = tmpvar_2.z;
     tmpvar_5.y = tmpvar_1.z;
     texel.z = texture2DRect (inputImageTexture3, 256.*tmpvar_5).z;
     red.x = 256.*texel.x;
     red.y = 0.16666;
     green.x = 256.*texel.y;
     green.y = 0.5;
     blue.x = 256.*texel.z;
     blue.y = 0.833333;
     texel.x = texture2DRect (inputImageTexture4, red).x;
     texel.y = texture2DRect (inputImageTexture4, green).y;
     texel.z = texture2DRect (inputImageTexture4, blue).z;
     vec2 tmpvar_6;
     tmpvar_6 = ((2.0 * textureCoordinateNorm) - 1.0);
     vec2 tmpvar_7;
     tmpvar_7.x = dot (tmpvar_6, tmpvar_6);
     tmpvar_7.y = texel.x;
     lookup = tmpvar_7;
     texel.x = texture2DRect (inputImageTexture5, 256.*lookup).x;
     lookup.y = texel.y;
     texel.y = texture2DRect (inputImageTexture5, 256.*lookup).y;
     lookup.y = texel.z;
     texel.z = texture2DRect (inputImageTexture5, 256.*lookup).z;
     red.x = 256.*texel.x;
     green.x = 256.*texel.y;
     blue.x = 256.*texel.z;
     texel.x = texture2DRect (inputImageTexture6, red).x;
     texel.y = texture2DRect (inputImageTexture6, green).y;
     texel.z = texture2DRect (inputImageTexture6, blue).z;
     vec4 tmpvar_8;
     tmpvar_8.w = 1.0;
     tmpvar_8.xyz = texel;
     gl_FragColor = tmpvar_8;
 }

