#include "testApp.h"

string stateName[]={"Idle","Starting","Capturing","Finding","Unifying","Selecting","Cropping","Finishing"};

#define FINDINGS_MIN_WIDTH 50
#define FINDINGS_MIN_HEIGHT 50

#define FACES_DIST_DIFF 40
#define FACES_DIM_DIFF 20

//--------------------------------------------------------------
void testApp::setup(){
    camera.initGrabber(CAPTURE_WIDTH,CAPTURE_HEIGHT);
	finder.setup("haarcascade_frontalface_default.xml");
	state=IDLE;

    startingTimeout=0.0;
	capturedFrames=0;
	captureSkip=0;

	findingTimeout=0.0;
	findingFrames=0;

	unifyingTimeout=0.0;
	unifyingFrames=0;

	selectingTimeout=0.0;
	selectingFaces=0;

	time=ofGetElapsedTimef();
}

//--------------------------------------------------------------
void testApp::update(){
    float t=ofGetElapsedTimef();
    float dt=t-time;
    time=t;

    switch(state){
    case IDLE:
        break;
    case STARTING:
        camera.update();
        startingTimeout-=dt;
        if(startingTimeout<=0){
            nextState();
        }
        break;
    case CAPTURING:
        camera.update();
        if(camera.isFrameNew()){
            captureSkip--;
            if(!captureSkip){
                frames[capturedFrames].setFromPixels(camera.getPixelsRef());
                frames[capturedFrames].update();
                captureSkip=CAPTURE_SKIP;
                capturedFrames++;
                if(capturedFrames>=CAPTURE_FRAMES){
                   nextState();
                }
            }
        }
        break;
    case FINDING:
        findingTimeout-=dt;
        if(findingTimeout<=0){
            finder.findHaarObjects(frames[findingFrames].getPixelsRef(),FINDINGS_MIN_WIDTH,FINDINGS_MIN_HEIGHT);
            for(int i=0; i<finder.blobs.size(); i++) {
                findings[findingFrames].push_back(finder.blobs[i].boundingRect);
            }
            findingTimeout=0.5;
            findingFrames++;
            if(findingFrames>=CAPTURE_FRAMES){
                nextState();
            }
        }
        break;
    case UNIFYING:
        unifyingTimeout-=dt;
        if(unifyingTimeout<=0){
            for(int i=0; i<findings[unifyingFrames].size(); i++){
                int exists=-1;
                Face newFace;
                newFace.bounds=findings[unifyingFrames][i];
                newFace.frame=unifyingFrames;
                newFace.image.cropFrom(frames[unifyingFrames],newFace.bounds.x,newFace.bounds.y,newFace.bounds.width,newFace.bounds.height);
                for(int j=0; j<facesPosition.size(); j++){
                    if(facesPosition[j].distance(newFace.bounds.getCenter())<FACES_DIST_DIFF){
                        if(abs(facesDimension[j].x-newFace.bounds.width)<FACES_DIM_DIFF && abs(facesDimension[j].y-newFace.bounds.height)<FACES_DIM_DIFF){
                            exists=j;
                            break;
                        }
                    }
                }
                if(exists==-1){
                    faces.push_back(vector<Face>());
                    facesPosition.push_back(ofPoint());
                    facesDimension.push_back(ofVec2f());
                    exists=faces.size()-1;
                }
                faces[exists].push_back(newFace);
                facesPosition[exists].set(0,0);
                facesDimension[exists].set(0,0);
                for(int j=0; j<faces[exists].size(); j++){
                    facesPosition[exists]+=(faces[exists][j].bounds.getCenter()/faces[exists].size());
                    facesDimension[exists]+=(ofVec2f(faces[exists][j].bounds.width,faces[exists][j].bounds.height)/faces[exists].size());
                }
            }
            unifyingTimeout=0.5;
            unifyingFrames++;
            if(unifyingFrames>=CAPTURE_FRAMES){
                nextState();
            }
        }
        break;
    case SELECTING:
        selectingTimeout-=dt;
        if(selectingTimeout<=0){
            int s=0;
            float maxSharpness=0;
            for(int i=0; i<faces[selectingFaces].size(); i++){
                ofxCvColorImage color;
                color.setFromPixels(faces[selectingFaces][i].image.getPixelsRef());
                ofxCvGrayscaleImage gray;
                gray = color;
                ofxCvGrayscaleImage sobelX;
                sobelX.allocate(gray.getWidth(),gray.getHeight());
                ofxCvGrayscaleImage sobelY;
                sobelY.allocate(gray.getWidth(),gray.getHeight());
                unsigned char * grayPixels=gray.getPixels();
                unsigned char * dx=sobelX.getPixels();
                for(int y=0;y<gray.getHeight();y++){
                    for(int x=1;x<gray.getWidth();x++){
                        int index=x+y*gray.getWidth();
                        int indexPrevious=(x-1)+y*gray.getWidth();
                        dx[index]=grayPixels[index]-grayPixels[indexPrevious];
                    }
                }
                unsigned char * dy=sobelY.getPixels();
                for(int y=1;y<gray.getHeight();y++){
                    for(int x=0;x<gray.getWidth();x++){
                        int index=x+y*gray.getWidth();
                        int indexPrevious=x+(y-1)*gray.getWidth();
                        dy[index]=grayPixels[index]-grayPixels[indexPrevious];
                    }
                }
                float sharpness=0;
                for(int j=0;j<(gray.getWidth()*gray.getHeight());j++){
                    sharpness+=(dx[j]*dx[j]+dy[j]*dy[j]);
                }
                if(sharpness>maxSharpness){
                    maxSharpness=sharpness;
                    s=i;
                }
            }
            selections[selectingFaces]=s;
            selectingTimeout=0.5;
            selectingFaces++;
            if(selectingFaces>=faces.size()){
                nextState();
            }
        }
        break;
    case CROPPING:
        for(int i=0; i<faces.size(); i++){
            if(faces[i].size()>CAPTURE_FRAMES/3){
                portraits.push_back(Portrait());
                portraits.back().rating=faces[i].size();
                int frame=faces[i][selections[i]].frame;
                ofPoint center=faces[i][selections[i]].bounds.getCenter();
                float amp=1.2;
                ofVec2f dim=amp*ofVec2f(faces[i][selections[i]].bounds.width,faces[i][selections[i]].bounds.height);
                ofRectangle crop(center.x-dim.x*0.5,center.y-dim.y*0.5,dim.x,dim.y);
                portraits.back().image.cropFrom(frames[frame],crop.x,crop.y,crop.width,crop.height);
                portraits.back().image.setAnchorPercent(0.5,0.);
            }
        }
        nextState();
        break;
    case FINISHING:
        break;
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    ofPushMatrix();
    ofScale(SCALE,SCALE);
    ofPushStyle();
    switch(state){
    case IDLE:
        break;
    case STARTING:
        ofSetColor(255);
        camera.draw(0,0);
        ofDrawBitmapString("Starting Timeout: " + ofToString(startingTimeout,1),5,30);
        break;
    case CAPTURING:
        ofSetColor(255);
        camera.draw(0,0);
        ofDrawBitmapString("Captured Frames: " + ofToString(capturedFrames),5,30);
        break;
    case FINDING:
        if(findingFrames){
            ofSetColor(255);
            frames[findingFrames-1].draw(0,0);
            ofSetColor(255,200);
            ofNoFill();
            for(int i=0; i<findings[findingFrames-1].size(); i++) {
                ofRectangle f = findings[findingFrames-1][i];
                ofRect(f.x, f.y, f.width, f.height);
            }
        }
        ofSetColor(255);
        ofDrawBitmapString("Finding Frames: " + ofToString(findingFrames),5,30);
        break;
    case UNIFYING:
        ofSetColor(255);
        for(int i=0; i<faces.size(); i++){
            for(int j=0; j<faces[i].size(); j++){
                faces[i][j].image.draw(j*THUMB_WIDTH,1.5*i*THUMB_WIDTH,THUMB_WIDTH,THUMB_WIDTH);
            }
        }
        ofDrawBitmapString("Unifying Frames: " + ofToString(unifyingFrames),5,30);
        break;
    case SELECTING:
        for(int i=0; i<faces.size(); i++){
            for(int j=0; j<faces[i].size(); j++){
                if(j==selections[i])
                    ofSetColor(255);
                else
                    ofSetColor(255,150);
                faces[i][j].image.draw(j*THUMB_WIDTH,1.5*i*THUMB_WIDTH,THUMB_WIDTH,THUMB_WIDTH);
            }
        }
        ofSetColor(255);
        ofDrawBitmapString("Selecting Frames: " + ofToString(selectingFaces),5,30);
        break;
    case CROPPING:
    case FINISHING:
        ofSetColor(255);
        int height=0;
        for(int i=0; i<portraits.size(); i++){
            portraits[i].image.draw(CAPTURE_WIDTH*0.5,height);
            height+=portraits[i].image.getHeight();
        }
        break;
     }
    ofPopStyle();
    ofPopMatrix();

	ofSetColor(255);
    ofDrawBitmapString(ofToString(ofGetFrameRate(),1),10,20);
	ofDrawBitmapString(stateName[state],10,40);
}

//--------------------------------------------------------------
void testApp::nextState(){
    state=(FaceCaptureState)((state+1)%STATES);
    switch(state){
    case IDLE:
        break;
    case STARTING:
        startingTimeout=3.0;
        break;
    case CAPTURING:
        for(int i=0; i<CAPTURE_FRAMES; i++) {
            frames[i].clear();
        }
        capturedFrames=0;
        captureSkip=CAPTURE_SKIP;
        break;
    case FINDING:
        for(int i=0; i<CAPTURE_FRAMES; i++) {
            findings[i].clear();
        }
        findingTimeout=0.0;
        findingFrames=0;
        break;
    case UNIFYING:
        faces.clear();
        facesPosition.clear();
        facesDimension.clear();
        unifyingTimeout=0.0;
        unifyingFrames=0;
        break;
    case SELECTING:
        selections.clear();
        selections.assign(faces.size(),-1);
        selectingTimeout=0.0;
        selectingFaces=0;
        break;
    case CROPPING:
        portraits.clear();
        break;
    case FINISHING:
        break;
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    nextState();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
