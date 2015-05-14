#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxSyphon.h"
#include "DoubleEndedPositioner.h"

enum AppState {
    Viewing,
    Settings,
    Calibratig
};

template<typename T, size_t N>
T * end(T (&ra)[N]) {
    return ra + N;
}

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
        void exit();
		
        void guiEvent(ofxUIEventArgs &e);
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    ofxUISuperCanvas *settingCanvas;
    ofxUISuperCanvas *calibrateCanvas;
    AppState state;
    AppState lastState;
    static const char *calibrationSizeOptionsNames[];
    static const float calibrationSizeOptionsValues[];
    float outPixelPerMM;
    float currentCalibratorTolLengthInMM;
    float desideredPixelWidth;
    ofImage testImg;
    DoubleEndedPositioner calibratorTool;
    
    
    void serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg);
    void serverRetired(ofxSyphonServerDirectoryEventArgs &arg);
    int syphonServerIndex;
    ofxSyphonServerDescription syphonServerDesc;
    ofxSyphonServerDirectory syphonDir;
    ofxSyphonClient syphonClient;
    
    void rebuildSyphonServerList();
};
