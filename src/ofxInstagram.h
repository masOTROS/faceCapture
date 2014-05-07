//
//  ofxInstagram.h
//  faceCapture
//
//  Created by Zaira on 06/05/14.
//
//
#include "ofMain.h"

class ofxInstagram{
public:
    ofxInstagram();
    ofTexture& applyFilter(ofTexture& src);
    
    bool fishEye;
    bool brcosa;
    bool bloom;
    bool lut;
    bool xpro;
    bool toaster;
    bool early;
    
private:
    ofShader fishEyeShader;
    float fishEyeLensradius,fishEyeSigncurvature;
    
    ofShader brcosaShader;
    float brcosaContrast,brcosaBrightness,brcosaSaturation,brcosaAlpha;
    
    ofShader bloomShader;
    float bloomBloom;
    
    ofShader lutShader;
    ofTexture lutTex;
    
    ofShader xproShader;
    ofTexture xproMapTex;
    ofTexture xproVigTex;
    
    ofShader toasterShader;
    ofTexture toasterMetal;
    ofTexture toasterSoftlight;
    ofTexture toasterCurves;
    ofTexture toasterOverlayMapWarm;
    ofTexture toasterColorshift;
    
    ofShader earlyShader;
    ofTexture earlyCurves;
    ofTexture earlyOverlay;
    ofTexture earlyVignette;
    ofTexture earlyBlowout;
    ofTexture earlyMap;

    void toggleFbo();
    ofFbo ping,pong;
    bool pingPong;
    ofFbo * now;
    ofFbo * last;
    
    float width,height;
};
