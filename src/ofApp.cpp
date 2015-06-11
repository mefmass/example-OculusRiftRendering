#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup(){
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    leap.open();
    
    //cam.setOrientation(ofPoint(-20, 0, 0));
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    ofBackground(0);
    
    showOverlay = false;
    predictive = false;
    
    ofHideCursor();
    
    oculusRift.baseCamera = &cam;
    oculusRift.setup();
    
    for(int i = 0; i < 20; i++){
        DemoSphere d;
        
        d.color = ofColor(180, 150, 80);
        
        d.pos = ofVec3f(ofRandom(-500, 500),0,ofRandom(-500,500));
        
        d.floatPos.x = d.pos.x;
        d.floatPos.z = d.pos.z;
        
        d.radius = ofRandom(10, 100);
        
        d.bMouseOver = false;
        d.bGazeOver  = false;
        
        demos.push_back(d);
    }
    
    //enable mouse;
    cam.begin();
    cam.end();
}


//--------------------------------------------------------------
void ofApp::update(){
    
    //-------------------------------------------------
    // Leap Motion Update
    
    fingersFound.clear();
    
    simpleHands = leap.getSimpleHands();
    
    if(leap.isFrameNew() && simpleHands.size()){
        
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
        leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);
        
        fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
        
        for(int i = 0; i < simpleHands.size(); i++){
            for (int f=0; f<5; f++) {
                int id = simpleHands[i].fingers[ fingerTypes[f] ].id;
                ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp; // metacarpal
                ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip; // proximal
                ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip; // distal
                ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip; // fingertip
                if(simpleHands[i].fingers[INDEX].isExtended){
                    
                }
                
                fingersFound.push_back(id);
            }
        }
    }
    
    leap.markFrameAsOld();
    
    //-------------------------------------------------
    // Oculus Rift Update
    
    for(int i = 0; i < demos.size(); i++){
        demos[i].floatPos.y = ofSignedNoise(ofGetElapsedTimef()/10.0,
                                            demos[i].pos.x/100.0,
                                            demos[i].pos.z/100.0,
                                            demos[i].radius*100.0) * demos[i].radius*20.;
    }
    
    if(oculusRift.isSetup()){
        ofRectangle viewport = oculusRift.getOculusViewport();
        for(int i = 0; i < demos.size(); i++){
            // mouse selection
            float mouseDist = oculusRift.distanceFromMouse(demos[i].floatPos);
            demos[i].bMouseOver = (mouseDist < 50);
            
            // gaze selection
            ofVec3f screenPos = oculusRift.worldToScreen(demos[i].floatPos, true);
            float gazeDist = ofDist(screenPos.x, screenPos.y,
                                    viewport.getCenter().x, viewport.getCenter().y);
            demos[i].bGazeOver = (gazeDist < 25);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(oculusRift.isSetup()){
        
        if(showOverlay){
            
            oculusRift.beginOverlay(-230, 320,240);
            ofRectangle overlayRect = oculusRift.getOverlayRectangle();
            
            ofPushStyle();
            ofEnableAlphaBlending();
            ofFill();
            ofSetColor(255, 40, 10, 200);
            
            ofRect(overlayRect);
            
            ofSetColor(255,255);
            ofFill();
            ofDrawBitmapString("ofxOculusRift by\nAndreas Muller\nJames George\nJason Walters\nElie Zananiri\nFPS:"+ofToString(ofGetFrameRate())+"\nPredictive Tracking " + (oculusRift.getUsePredictiveOrientation() ? "YES" : "NO"), 40, 40);
            
            ofSetColor(0, 255, 0);
            ofNoFill();
            ofCircle(overlayRect.getCenter(), 20);
            
            ofPopStyle();
            oculusRift.endOverlay();
        }
        
        ofSetColor(255);
        glEnable(GL_DEPTH_TEST);
        
//        oculusRift.beginBackground();
//        ofPushMatrix();
//        ofTranslate(ofGetMouseX(), ofGetMouseY());
//        ofBackgroundGradient(ofColor(30), ofColor(255), OF_GRADIENT_LINEAR);
//        //ofBackground(200);
//        ofPopMatrix();
//        oculusRift.endBackground();
        
        oculusRift.beginLeftEye();
        drawScene();
        oculusRift.endLeftEye();
        
        oculusRift.beginRightEye();
        drawScene();
        oculusRift.endRightEye();
        
        oculusRift.draw();
        
        glDisable(GL_DEPTH_TEST);
    }
    else{
        cam.begin();
        drawHands();
        cam.end();
    }
}

void ofApp::drawScene()
{
    drawHands();
    
    ofPushMatrix();
    ofRotate(90, 0, 0, -1);
    ofDrawGridPlane(5000.0f, 10.0f, false );
    ofPopMatrix();
    
    ofPushStyle();
    ofNoFill();
    for(int i = 0; i < demos.size(); i++){
        ofPushMatrix();
        //		ofRotate(ofGetElapsedTimef()*(50-demos[i].radius), 0, 1, 0);
        ofTranslate(demos[i].floatPos);
        //		ofRotate(ofGetElapsedTimef()*4*(50-demos[i].radius), 0, 1, 0);
        
        if (demos[i].bMouseOver)
            ofSetColor(ofColor::white.getLerped(ofColor(200, 50, 0), sin(ofGetElapsedTimef()*10.0)*.5+.5));
        else if (demos[i].bGazeOver)
            ofSetColor(ofColor::white.getLerped(ofColor(50, 200, 0), sin(ofGetElapsedTimef()*10.0)*.5+.5));
        else
            ofSetColor(demos[i].color);
        
        ofSphere(demos[i].radius);
        ofPopMatrix();
    }
    
    //billboard and draw the mouse
    if(oculusRift.isSetup()){
        
        ofPushMatrix();
        oculusRift.multBillboardMatrix();
        ofSetColor(255, 0, 0);
        ofCircle(0,0,.5);
        ofPopMatrix();
    }
    ofPopStyle();
}

void ofApp::drawHands(){
    
    ofPushMatrix();
    ofRotateX(90);
    ofRotateZ(180);

    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};

    for(int i = 0; i < simpleHands.size(); i++){
        
//        simpleHands[i].debugDraw();
        bool isLeft        = simpleHands[i].isLeft;
        ofPoint handPos    = simpleHands[i].handPos;
        ofPoint handNormal = simpleHands[i].handNormal;

        ofSetColor(50, 180, 200);
        ofDrawSphere(handPos.x, handPos.y, handPos.z, 10);
//        ofSetColor(200, 100, 0);
//        ofDrawArrow(handPos, handPos + 100*handNormal);

        for (int f=0; f<5; f++) {
            ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp;  // metacarpal
            ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip;  // proximal
            ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip;  // distal
            ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip;  // fingertip

            ofSetColor(50, 180, 200);
            ofDrawSphere(mcp.x, mcp.y, mcp.z, 7);
            ofDrawSphere(pip.x, pip.y, pip.z, 7);
            ofDrawSphere(dip.x, dip.y, dip.z, 7);
            ofDrawSphere(tip.x, tip.y, tip.z, 7);

            ofSetColor(10, 150, 250);
            ofSetLineWidth(15);
            ofLine(mcp.x, mcp.y, mcp.z, pip.x, pip.y, pip.z);
            ofLine(pip.x, pip.y, pip.z, dip.x, dip.y, dip.z);
            ofLine(dip.x, dip.y, dip.z, tip.x, tip.y, tip.z);
        }
    }
    ofPopMatrix();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch(key) {
        case 'f':
            ofToggleFullscreen();
            //post.init(ofGetWidth(), ofGetHeight());
            break;
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void ofApp::exit(){
    // let's close down Leap and kill the controller
    leap.close();
}
