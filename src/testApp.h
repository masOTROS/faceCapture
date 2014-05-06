#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

#define CAPTURE_WIDTH 320
#define CAPTURE_HEIGHT 240

#define CAPTURE_FRAMES 20
#define CAPTURE_SKIP 5

#define THUMB_WIDTH CAPTURE_WIDTH/CAPTURE_FRAMES

#define SCALE 2

enum FaceCaptureState{
    IDLE=0,
    STARTING,
    CAPTURING,
    FINDING,
    UNIFYING,
    SELECTING,
    CROPPING,
    FINISHING,
    STATES
};

typedef struct{
    ofImage image;
    ofRectangle bounds;
    int frame;
}Face;

typedef struct{
    ofImage image;
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

		FaceCaptureState state;

		float startingTimeout;

        ofImage frames[CAPTURE_FRAMES];
        unsigned int captureSkip;
        unsigned int capturedFrames;

        float findingTimeout;
        unsigned int findingFrames;
        vector<ofRectangle> findings[CAPTURE_FRAMES];

        float unifyingTimeout;
        unsigned int unifyingFrames;
        vector< vector<Face> > faces;
        vector<ofPoint> facesPosition;
        vector<ofVec2f> facesDimension;

        float selectingTimeout;
        unsigned int selectingFaces;
        vector<int> selections;

        vector<Portrait> portraits;

		ofxCvHaarFinder finder;
		ofVideoGrabber camera;
};
