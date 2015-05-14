#include "ofApp.h"

const char *ofApp::calibrationSizeOptionsNames[] = {"Letter Long Edge","Letter Short Edge","Business Card Long Edge","10cm"};
const float ofApp::calibrationSizeOptionsValues[] = {279.4,215.9,88.9,100.0};

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetWindowTitle("LowResPreview");
    
    desideredPixelWidth = 1.0;
    lastState = state = Settings;
    outPixelPerMM = 3.0;
    
    syphonDir.setup();
    syphonClient.setup();
    syphonServerIndex = -1;
    ofAddListener(syphonDir.events.serverAnnounced, this, &ofApp::serverAnnounced);
    ofAddListener(syphonDir.events.serverRetired, this, &ofApp::serverRetired);
    
    testImg.load("rainbow.png");
    testImg.load("noise.jpg");
    testImg.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    settingCanvas = new ofxUISuperCanvas("Settings");
    settingCanvas->setDimensions(300, 400);
    settingCanvas->addLabelButton("Calibrate Menu", false)->setTriggerType(OFX_UI_TRIGGER_END);
    settingCanvas->addLabel("pixel width (mm)");
    settingCanvas->addSlider("pixelWidth", 0.0, 10.0, &desideredPixelWidth)->setLabelVisible(false);
    ofxUITextInput *pwTi =  settingCanvas->addTextInput("pixelWidthText", ofToString(desideredPixelWidth));
    pwTi->setOnlyNumericInput(true);
    settingCanvas->addSpacer();
    settingCanvas->addRadio("Syphon Servers", vector<string>());

    ofAddListener(settingCanvas->newGUIEvent,this,&ofApp::guiEvent);
    
    
    calibrateCanvas = new ofxUISuperCanvas("Calibrate");
    calibrateCanvas->setVisible(false);
    calibrateCanvas->setDimensions(300,400);
    calibrateCanvas->addLabelButton("Settings Menu", false)->setTriggerType(OFX_UI_TRIGGER_END);
    calibrateCanvas->addSpacer();
    calibrateCanvas->addLabelButton("Read From Marker", false)->setTriggerType(OFX_UI_TRIGGER_END);
    calibrateCanvas->addSpacer();
    
    vector<string> calibrationSizeOptions(calibrationSizeOptionsNames,end(calibrationSizeOptionsNames));
    int defautlt_index = 0;
    
    ofxUILabel *calLabel = calibrateCanvas->addLabel("selected_calibration");
    calLabel->setLabel(calibrationSizeOptionsNames[defautlt_index]);
    
    ofxUIDropDownList* cal_size_widget = calibrateCanvas->addDropDownList("Calibration Size", calibrationSizeOptions);
    vector<int> &indecies = cal_size_widget->getSelectedIndeces();
    indecies.push_back(defautlt_index);
    
    currentCalibratorTolLengthInMM = calibrationSizeOptionsValues[defautlt_index];

    ofAddListener(calibrateCanvas->newGUIEvent,this,&ofApp::guiEvent);
    
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0);
    ofSetColor(ofColor::white);
//    testImg.draw(0, 0, testImg.getWidth() * outPixelPerMM * desideredPixelWidth, testImg.getHeight() * outPixelPerMM * desideredPixelWidth);

    syphonClient.draw(0, 0, syphonClient.getWidth() * outPixelPerMM * desideredPixelWidth, syphonClient.getHeight() * outPixelPerMM * desideredPixelWidth);

    switch (state) {
        case Viewing:
            break;
        case Settings:
            break;
        case Calibratig:
            calibratorTool.draw();
        default:
            break;
    }

}

//--------------------------------------------------------------
void ofApp::exit(){
    delete settingCanvas;
    delete calibrateCanvas;
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
    if(name == "pixelWidth"){
        ((ofxUITextInput*)settingCanvas->getWidget("pixelWidthText"))->setTextString(ofToString(desideredPixelWidth));
    }else if(name == "pixelWidthText"){
        string val = ((ofxUITextInput*)e.widget)->getTextString();
        desideredPixelWidth = ofToFloat(val);

    }else if(name == "Calibrate Menu"){
        lastState = state;
        state = Calibratig;
        settingCanvas->setVisible(false);
        calibrateCanvas->setVisible(true);
        
    }else if(name == "Settings Menu"){
        lastState = state;
        state = Settings;
        settingCanvas->setVisible(true);
        calibrateCanvas->setVisible(false);
        
    }else if(name == "Calibration Size"){
        ofxUIDropDownList* w = ((ofxUIDropDownList*)e.widget);
        vector<int> &indecies = w->getSelectedIndeces();
        ((ofxUILabel*)calibrateCanvas->getWidget("selected_calibration"))->setLabel(calibrationSizeOptionsNames[indecies[0]]);
        currentCalibratorTolLengthInMM = calibrationSizeOptionsValues[indecies[0]];
        
    }else if(name == "Read From Marker"){
        outPixelPerMM = calibratorTool.getLength() / currentCalibratorTolLengthInMM;
        
    }else if(name == "Syphon Servers"){
        ofxUIRadio *syphon_list = (ofxUIRadio*)e.widget;
        string active = syphon_list->getActiveName();
        vector<string> parts = ofSplitString(active, "::");
        syphonServerDesc = ofxSyphonServerDescription(parts[0],parts[1]);
        syphonClient.set(syphonServerDesc);
    }
}

//these are our directory's callbacks
void ofApp::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Announced")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    rebuildSyphonServerList();
}

void ofApp::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Retired")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
        if(syphonServerDesc == dir){
            syphonServerDesc = ofxSyphonServerDescription();
        }
    }
    rebuildSyphonServerList();
}

void ofApp::rebuildSyphonServerList(){
    settingCanvas->removeWidget("Syphon Servers");
    vector<string> server_names;
    for( auto& server : syphonDir.getServerList()){
        server_names.push_back(server.serverName+"::"+server.appName);
    }
    ofxUIRadio *radio_list =  settingCanvas->addRadio("Syphon Servers", server_names);
    if(syphonServerDesc.serverName == "null" && syphonServerDesc.appName == "null" && syphonDir.size()){
        syphonServerDesc = syphonDir.getServerList()[0];
        syphonClient.set(syphonServerDesc);
    }
    if(syphonDir.size()){
        radio_list->activateToggle(syphonServerDesc.serverName+"::"+syphonServerDesc.appName);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'm':
            
            if(state != Viewing){
                lastState = state;
                state = Viewing;
                settingCanvas->setVisible(false);
                calibrateCanvas->setVisible(false);
            }else{
                AppState t = state;
                state = lastState;
                lastState = t;
                if(state == Calibratig){
                    settingCanvas->setVisible(false);
                    calibrateCanvas->setVisible(true);
                }else{
                    settingCanvas->setVisible(true);
                    calibrateCanvas->setVisible(false);
                }
            }
            break;
            
        default:
            break;
    }}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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