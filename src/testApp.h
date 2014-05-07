#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxInstagram.h"

#define CAPTURE_WIDTH 1920
#define CAPTURE_HEIGHT 1080

#define CAPTURE_FRAMES 20
#define CAPTURE_SKIP 5

#define THUMB_WIDTH CAPTURE_WIDTH/CAPTURE_FRAMES

#define SCALE 0.5

enum FaceCaptureState{
    IDLE=0,
    STARTING,
    CAPTURING,
    FINDING,
    UNIFYING,
    SELECTING,
    CROPPING,
    FINISHING,
    WAITING,
    SAVING,
    STATES
};

typedef struct{
    ofImage image;
    ofRectangle bounds;
    int frame;
}Face;

typedef struct{
    ofImage image;
    ofImage imageRefined;
    unsigned int rating;
}Portrait;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void nextState();

        float time;
        float delay;

		FaceCaptureState state;

		float startingTimeout;

        ofImage frames[CAPTURE_FRAMES];
        unsigned int captureSkip;
        unsigned int capturedFrames;

        unsigned int findingFrames;
        vector<ofRectangle> findings[CAPTURE_FRAMES];

        unsigned int unifyingFrames;
        vector< vector<Face> > faces;
        vector<ofPoint> facesPosition;
        vector<ofVec2f> facesDimension;

        unsigned int selectingFaces;
        float selectionSharpness;
        vector<int> selections;
        
        unsigned int croppingFaces;
        unsigned int finishingPortraits;
        vector<Portrait> portraits;
    
        ofxCv::ObjectFinder finder;
        ofVideoGrabber camera;
        ofxInstagram instagram;
    
    float automatic;
};
