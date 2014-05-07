//
//  ofxInstagram.cpp
//  faceCapture
//
//  Created by Zaira on 06/05/14.
//
//

#include "ofxInstagram.h"

ofxInstagram::ofxInstagram(){
    pingPong=false;
    width=height=0;
    
    fishEye=false;
    fishEyeShader.load("shaders/fisheye/FishEye_GLSL");
    fishEyeLensradius=2;
    fishEyeSigncurvature=4;
    
    brcosa=false;
    brcosaShader.load("shaders/brcosa/brcosa_GLSL");
    brcosaContrast=1.;
    brcosaBrightness=1.;
    brcosaSaturation=1.;
    brcosaAlpha=1.;
    
    bloom=false;
    bloomShader.load("shaders/bloom/bloom_GLSL");
    bloomBloom=1.;
    
    lut=false;
    lutShader.load("shaders/LUT/LUT_GLSL");
    ofLoadImage(lutTex,"shaders/LUT/lookup_miss_etikate.png");
    
    xpro=false;
    xproShader.load("shaders/Xpro/Xpro_GLSL");
    ofLoadImage(xproMapTex,"shaders/Xpro/xproMap.png");
    ofLoadImage(xproVigTex,"shaders/vignetteMap.png");
    
    toaster=false;
    toasterShader.load("shaders/toaster/toaster_GLSL");
    ofLoadImage(toasterMetal,"shaders/toaster/toasterMetal.png");
    ofLoadImage(toasterSoftlight,"shaders/toaster/toasterSoftlight.png");
    ofLoadImage(toasterCurves,"shaders/toaster/toasterCurves.png");
    ofLoadImage(toasterOverlayMapWarm,"shaders/toaster/toasterOverlayMapWarm.png");
    ofLoadImage(toasterColorshift,"shaders/toaster/toasterColorshift.png");
    
    early=false;
    earlyShader.load("shaders/earlybird/earlybird_GLSL");
    ofLoadImage(earlyCurves,"shaders/earlybird/earlybirdCurves.png");
    ofLoadImage(earlyOverlay,"shaders/earlybird/earlybirdOverlayMap.png");
    ofLoadImage(earlyVignette,"shaders/vignetteMap.png");
    ofLoadImage(earlyBlowout,"shaders/earlybird/earlybirdBlowout.png");
    ofLoadImage(earlyMap,"shaders/earlybird/earlybirdMap.png");   
}

//--------------------------------------------------------------
ofTexture& ofxInstagram::applyFilter(ofTexture& src){
    
    if(width!=src.getWidth() || height!=src.getHeight()){
        width = src.getWidth();
        height = src.getHeight();
        ping.allocate(width,height,GL_RGBA);
        pong.allocate(width,height,GL_RGBA);
    }
    
    toggleFbo();
    now->begin();
    ofClear(0,0);
    src.draw(0,0);
    now->end();
    
    ofEnableNormalizedTexCoords();
    
    if(brcosa){
        toggleFbo();
        now->begin();
        ofClear(0,0);
        brcosaShader.begin();
        brcosaShader.setUniformTexture("tex",last->getTextureReference(),0);
        brcosaShader.setUniform1f("saturation", brcosaSaturation);
        brcosaShader.setUniform1f("contrast", brcosaContrast);
        brcosaShader.setUniform1f("brightness", brcosaBrightness);
        brcosaShader.setUniform1f("alpha", brcosaAlpha);
        ofSetColor(255);
        last->getTextureReference().bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(0,0,0);
        glTexCoord2f(1,0); glVertex3f(width,0,0);
        glTexCoord2f(1,1); glVertex3f(width,height,0);
        glTexCoord2f(0,1); glVertex3f(0,height,0);
        glEnd();
        last->getTextureReference().unbind();
        brcosaShader.end();
        now->end();
    }
    
    if(bloom){
        toggleFbo();
        now->begin();
        ofClear(0,0);
        bloomShader.begin();
        bloomShader.setUniformTexture("tex0",last->getTextureReference(),0);
        bloomShader.setUniform1f("bloom",bloomBloom);
        ofSetColor(255);
        last->getTextureReference().bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(0,0,0);
        glTexCoord2f(1,0); glVertex3f(width,0,0);
        glTexCoord2f(1,1); glVertex3f(width,height,0);
        glTexCoord2f(0,1); glVertex3f(0,height,0);
        glEnd();
        last->getTextureReference().unbind();
        bloomShader.end();
        now->end();
    }
    
    if(lut){
        toggleFbo();
        now->begin();
        ofClear(0,0);
        lutShader.begin();
        lutShader.setUniformTexture("tex",last->getTextureReference(),0);
        lutShader.setUniformTexture("texLUT",lutTex,1);
        ofSetColor(255);
        last->getTextureReference().bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(0,0,0);
        glTexCoord2f(1,0); glVertex3f(width,0,0);
        glTexCoord2f(1,1); glVertex3f(width,height,0);
        glTexCoord2f(0,1); glVertex3f(0,height,0);
        glEnd();
        last->getTextureReference().unbind();
        lutShader.end();
        now->end();
    }
    
    if(xpro){
        toggleFbo();
        now->begin();
        ofClear(0,0);
        xproShader.begin();
        xproShader.setUniformTexture("tex",last->getTextureReference(),0);
        xproShader.setUniformTexture("texMap",xproMapTex,1);
        xproShader.setUniformTexture("texVig",xproVigTex,2);
        ofSetColor(255);
        last->getTextureReference().bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(0,0,0);
        glTexCoord2f(1,0); glVertex3f(width,0,0);
        glTexCoord2f(1,1); glVertex3f(width,height,0);
        glTexCoord2f(0,1); glVertex3f(0,height,0);
        glEnd();
        last->getTextureReference().unbind();
        xproShader.end();
        now->end();
    }
    
    if(toaster){
        toggleFbo();
        now->begin();
        ofClear(0,0);
        toasterShader.begin();
        toasterShader.setUniformTexture("inputImageTexture",last->getTextureReference(),0);
        toasterShader.setUniformTexture("inputImageTexture2",toasterMetal,1);
        toasterShader.setUniformTexture("inputImageTexture3",toasterSoftlight,2);
        toasterShader.setUniformTexture("inputImageTexture4",toasterCurves,3);
        toasterShader.setUniformTexture("inputImageTexture5",toasterOverlayMapWarm,4);
        toasterShader.setUniformTexture("inputImageTexture6",toasterColorshift,5);
        ofSetColor(255);
        last->getTextureReference().bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(0,0,0);
        glTexCoord2f(1,0); glVertex3f(width,0,0);
        glTexCoord2f(1,1); glVertex3f(width,height,0);
        glTexCoord2f(0,1); glVertex3f(0,height,0);
        glEnd();
        last->getTextureReference().unbind();
        toasterShader.end();
        now->end();
    }
    
    if(early){
        toggleFbo();
        now->begin();
        ofClear(0,0);
        earlyShader.begin();
        earlyShader.setUniformTexture("inputImageTexture",last->getTextureReference(),0);
        earlyShader.setUniformTexture("inputImageTexture2",earlyCurves,1);
        earlyShader.setUniformTexture("inputImageTexture3",earlyOverlay,2);
        earlyShader.setUniformTexture("inputImageTexture4",earlyVignette,3);
        earlyShader.setUniformTexture("inputImageTexture5",earlyBlowout,4);
        earlyShader.setUniformTexture("inputImageTexture6",earlyMap,5);
        ofSetColor(255);
        last->getTextureReference().bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(0,0,0);
        glTexCoord2f(1,0); glVertex3f(width,0,0);
        glTexCoord2f(1,1); glVertex3f(width,height,0);
        glTexCoord2f(0,1); glVertex3f(0,height,0);
        glEnd();
        last->getTextureReference().unbind();
        earlyShader.end();
        now->end();
    }
    
    if(fishEye){
        toggleFbo();
        now->begin();
        ofClear(0,0);
        fishEyeShader.begin();
        fishEyeShader.setUniformTexture("tex0",last->getTextureReference(),0);
        fishEyeShader.setUniform1f("lensradius",fishEyeLensradius);
        fishEyeShader.setUniform1f("signcurvature",fishEyeSigncurvature);
        ofSetColor(255);
        last->getTextureReference().bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(0,0,0);
        glTexCoord2f(1,0); glVertex3f(width,0,0);
        glTexCoord2f(1,1); glVertex3f(width,height,0);
        glTexCoord2f(0,1); glVertex3f(0,height,0);
        glEnd();
        last->getTextureReference().unbind();
        fishEyeShader.end();
        now->end();
    }
    
    ofDisableNormalizedTexCoords();
    
    return now->getTextureReference();
}

//--------------------------------------------------------------
void ofxInstagram::toggleFbo(){
    pingPong=!pingPong;
    if(pingPong){
        now=&ping;
        last=&pong;
    }
    else{
        now=&pong;
        last=&ping;
    }
}
