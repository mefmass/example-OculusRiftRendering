#pragma once

#include "ofMain.h"
#include "ofxLeapMotion2.h"
#include "ofxOculusDK2.h"

typedef struct{
    ofColor color;
    ofVec3f pos;
    ofVec3f floatPos;
    float radius;
    bool bMouseOver;
    bool bGazeOver;
} DemoSphere;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void drawScene();
    void drawHands();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();
    
    ofxLeapMotion leap;
    vector <ofxLeapMotionSimpleHand> simpleHands;
    
    vector <int> fingersFound;
    ofEasyCam cam;
    
    ofxOculusDK2 oculusRift;
    
    ofLight	light;
    bool showOverlay;
    bool predictive;
    vector<DemoSphere> demos;
    
    ofVec3f cursor2D;
    ofVec3f cursor3D;
    
    ofVec3f cursorRift;
    ofVec3f demoRift;
    
    ofVec3f cursorGaze;
};
