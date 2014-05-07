#include "testApp.h"

string stateName[]={"Idle","Starting","Capturing","Finding","Unifying","Selecting","Cropping","Finishing","Waiting","Saving"};

#define FINDINGS_MIN_SCALE 0.05
#define FINDINGS_MAX_SCALE 0.50

#define FACES_DIST_DIFF 100
#define FACES_DIM_DIFF 50

#define BRIGTHNESS_MIN 100

#define PORTRAIT_WIDTH 300
#define PORTRAIT_HEIGHT 400

//--------------------------------------------------------------
void testApp::setup(){
    camera.initGrabber(CAPTURE_WIDTH,CAPTURE_HEIGHT);
    
    instagram.lut=true;
    instagram.xpro=true;
    
    finder.setup(ofToDataPath("haarcascade_frontalface_alt2.xml"));
	finder.setPreset(ofxCv::ObjectFinder::Accurate);
    finder.setMinSizeScale(FINDINGS_MIN_SCALE);
    finder.setMaxSizeScale(FINDINGS_MAX_SCALE);
    finder.setCannyPruning(true);
    
    delay=0;
    
    state=IDLE;
    
    automatic=1.0;

	time=ofGetElapsedTimef();
}

//--------------------------------------------------------------
void testApp::update(){
    float t=ofGetElapsedTimef();
    float dt=t-time;
    time=t;

    if(!delay){
        switch(state){
            case IDLE:
                if(automatic){
                    automatic-=dt;
                    if(automatic<=0){
                        automatic=1.0;
                        nextState();
                    }
                }
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
                finder.update(frames[findingFrames]);
                for(int i=0; i<finder.size(); i++) {
                    findings[findingFrames].push_back(finder.getObject(i));
                }
                findingFrames++;
                if(findingFrames>=CAPTURE_FRAMES)
                    nextState();
                else
                    delay=0.1;
                break;
            case UNIFYING:
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
                unifyingFrames++;
                if(unifyingFrames>=CAPTURE_FRAMES)
                    nextState();
                else
                    delay=0.1;
                break;
            case SELECTING:
                selectionSharpness=0;
                if(faces.size()){
                    for(int i=0; i<faces[selectingFaces].size(); i++){
                        ofPixels gray=faces[selectingFaces][i].image;
                        gray.setImageType(OF_IMAGE_GRAYSCALE);
                        ofPixels sobelX;
                        sobelX.allocate(gray.getWidth(),gray.getHeight(),OF_IMAGE_GRAYSCALE);
                        ofPixels sobelY;
                        sobelY.allocate(gray.getWidth(),gray.getHeight(),OF_IMAGE_GRAYSCALE);
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
                        if(sharpness>selectionSharpness){
                            selectionSharpness=sharpness;
                            selections[selectingFaces]=i;
                        }
                    }
                }
                selectingFaces++;
                if(selectingFaces>=faces.size())
                    nextState();
                else
                    delay=0.1;
                break;
            case CROPPING:
                if(faces.size()){
                    if(faces[croppingFaces].size()>CAPTURE_FRAMES/3){
                        portraits.push_back(Portrait());
                        portraits.back().rating=faces[croppingFaces].size();
                        int frame=faces[croppingFaces][selections[croppingFaces]].frame;
                        ofPoint center=faces[croppingFaces][selections[croppingFaces]].bounds.getCenter();
                        float ampX=1.2;
                        float ampY=1.5;
                        ofVec2f dim=ofVec2f(ampX*faces[croppingFaces][selections[croppingFaces]].bounds.width,ampY*faces[croppingFaces][selections[croppingFaces]].bounds.height);
                        ofRectangle crop(center.x-dim.x*0.5,center.y-dim.y*0.5,dim.x,dim.y);
                        portraits.back().image.cropFrom(frames[frame],crop.x,crop.y,crop.width,crop.height);
                    }
                }
                croppingFaces++;
                if(croppingFaces>=faces.size())
                    nextState();
                else
                    delay=0.5;
                break;
            case FINISHING:
                if(portraits.size()){
                    portraits[finishingPortraits].imageRefined=portraits[finishingPortraits].image;
                    if((portraits[finishingPortraits].image.getWidth()/portraits[finishingPortraits].image.getHeight())>(PORTRAIT_WIDTH/PORTRAIT_HEIGHT)){
                        int newWidth=portraits[finishingPortraits].image.getHeight()*PORTRAIT_WIDTH/PORTRAIT_HEIGHT;
                        portraits[finishingPortraits].imageRefined.crop((portraits[finishingPortraits].image.getWidth()-newWidth)*0.5,0,newWidth,portraits[finishingPortraits].image.getHeight());
                    }
                    else if((portraits[finishingPortraits].image.getWidth()/portraits[finishingPortraits].image.getHeight())<(PORTRAIT_WIDTH/PORTRAIT_HEIGHT)){
                        int newHeight=portraits[finishingPortraits].image.getWidth()*PORTRAIT_HEIGHT/PORTRAIT_WIDTH;
                        portraits[finishingPortraits].imageRefined.crop(0,(portraits[finishingPortraits].image.getHeight()-newHeight)*0.5,portraits[finishingPortraits].image.getWidth(),newHeight);
                    }
                    portraits[finishingPortraits].imageRefined.resize(PORTRAIT_WIDTH,PORTRAIT_HEIGHT);
                    instagram.applyFilter(portraits[finishingPortraits].imageRefined.getTextureReference()).readToPixels(portraits[finishingPortraits].imageRefined.getPixelsRef());
                    portraits[finishingPortraits].imageRefined.update();
                }
                finishingPortraits++;
                if(finishingPortraits>=portraits.size())
                    nextState();
                else
                    delay=0.1;
                break;
            case WAITING:
                if(automatic){
                    automatic-=dt;
                    if(automatic<=0){
                        automatic=1.0;
                        nextState();
                    }
                }
                break;
            case SAVING:
                for(int i=0;i<portraits.size();i++){
                    ofSaveImage(portraits[i].imageRefined,"portraits/"+ofGetTimestampString()+"_"+ofToString(i)+"_"+ofToString(portraits[i].rating)+".png",OF_IMAGE_QUALITY_BEST);
                    ofSaveImage(portraits[i].image,"portraits/"+ofGetTimestampString()+"_"+ofToString(i)+"_"+ofToString(portraits[i].rating)+"_original.png",OF_IMAGE_QUALITY_BEST);
                }
                nextState();
                break;
        }
    }
    else{
        delay-=dt;
        if(delay<=0)
            delay=0;
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
        ofDrawBitmapString("Starting Timeout: " + ofToString(startingTimeout,1),10/SCALE,60/SCALE);
        break;
    case CAPTURING:
        ofSetColor(255);
        camera.draw(0,0);
        ofDrawBitmapString("Captured Frames: " + ofToString(capturedFrames),10/SCALE,60/SCALE);
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
        ofDrawBitmapString("Finding Frames: " + ofToString(findingFrames),10/SCALE,60/SCALE);
        break;
    case UNIFYING:
        ofSetColor(255);
        for(int i=0; i<faces.size(); i++){
            for(int j=0; j<faces[i].size(); j++){
                faces[i][j].image.draw(j*THUMB_WIDTH,1.5*i*THUMB_WIDTH,THUMB_WIDTH,THUMB_WIDTH);
            }
        }
        ofDrawBitmapString("Unifying Frames: " + ofToString(unifyingFrames),10/SCALE,60/SCALE);
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
        ofDrawBitmapString("Selecting Frames: " + ofToString(selectingFaces),10/SCALE,60/SCALE);
        break;
    case CROPPING:
        ofSetColor(255);
        ofPushMatrix();
        for(int i=0; i<portraits.size(); i++){
            portraits[i].image.draw(0,60/SCALE);
            ofTranslate(portraits[i].image.getWidth()+10/SCALE,0);
        }
        ofPopMatrix();
        break;
    case FINISHING:
    case WAITING:
    case SAVING:
        ofSetColor(255);
        ofPushMatrix();
        for(int i=0; i<portraits.size() && i<finishingPortraits; i++){
            portraits[i].imageRefined.draw(0,60/SCALE);
            ofTranslate(portraits[i].imageRefined.getWidth()+10/SCALE,0);
        }
        ofPopMatrix();
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
        findingFrames=0;
        break;
    case UNIFYING:
        for(int i=0; i<faces.size(); i++) {
            faces[i].clear();
        }
        faces.clear();
        facesPosition.clear();
        facesDimension.clear();
        unifyingFrames=0;
        break;
    case SELECTING:
        selections.clear();
        selections.assign(faces.size(),-1);
        selectingFaces=0;
        break;
    case CROPPING:
        croppingFaces=0;
        portraits.clear();
        break;
    case FINISHING:
        finishingPortraits=0;
        break;
    case WAITING:
        break;
    case SAVING:
        break;
    default:
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
