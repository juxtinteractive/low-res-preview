#include "ofApp.h"

const char *ofApp::calibrationSizeOptionsNames[] = {"Letter Long Edge","Letter Short Edge","Business Card Long Edge","10cm"};
const float ofApp::calibrationSizeOptionsValues[] = {279.4,215.9,88.9,100.0};

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetWindowTitle("LowResPreview");
    desideredPixelWidthLarge = 1;
    desideredPixelWidth = 0.0;
    lastState = state = Settings;
    outPixelPerMM = 3.0;
    edgeSizePct = 1.0;
    edgeSoft = 0.1;
    mode = 0;
    
    mainShader.load("shaders/default");
    
    
    syphonDir.setup();
    syphonClient.setup();
    syphonServerIndex = -1;
    ofAddListener(syphonDir.events.serverAnnounced, this, &ofApp::serverAnnounced);
    ofAddListener(syphonDir.events.serverRetired, this, &ofApp::serverRetired);
    
    settingCanvas = new ofxUISuperCanvas("Settings");
    settingCanvas->setDimensions(300, 400);
    settingCanvas->addLabelButton("Calibrate Menu", false)->setTriggerType(OFX_UI_TRIGGER_END);
    settingCanvas->addLabel("pixel width (mm)");
    ofxUISlider_<int> *pwl = new ofxUISlider_<int>("pixelWidthLarge", 0, 100, &desideredPixelWidthLarge,292,settingCanvas->getGlobalSliderHeight(),0,0);
    pwl->setLabelVisible(false);

    settingCanvas->addWidgetDown(pwl);
    settingCanvas->addSlider("pixelWidth", 0.0, 0.9999, &desideredPixelWidth)->setLabelVisible(false);
    ofxUITextInput *pwTi =  settingCanvas->addTextInput("pixelWidthText", ofToString(desideredPixelWidth));
    pwTi->setOnlyNumericInput(true);
    settingCanvas->addSpacer();
    const char *modes[] = {"Round","Square"};
    settingCanvas->addRadio("Mode", vector<string>(modes, end(modes)),OFX_UI_ORIENTATION_HORIZONTAL)->activateToggle("Round");
    settingCanvas->addSpacer();
    settingCanvas->addSlider("Edge Size %", 0.001, 1.0, &edgeSizePct);
    settingCanvas->addSlider("Edge Softness", 0.001, 0.5, &edgeSoft);
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
//    syphonClient.draw(0, 0, syphonClient.getWidth() * outPixelPerMM * desideredPixelWidth, syphonClient.getHeight() * outPixelPerMM * desideredPixelWidth);
    plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
    plane.mapTexCoords(0, 0, 1, 1);
    
    syphonClient.bind();
    ofTexture tex0 = syphonClient.getTextureRef();


    // mapTexCoordsFromTexture(tex0);
    mainShader.begin();
    mainShader.setUniform2f("windowSize", ofGetWindowWidth(), ofGetWindowHeight());
    mainShader.setUniform2f("texSize", syphonClient.getWidth(), syphonClient.getHeight());
    mainShader.setUniform1f("texScale", outPixelPerMM * (float(desideredPixelWidthLarge)+desideredPixelWidth));
    mainShader.setUniform1f("circleOffset",edgeSizePct);
    mainShader.setUniform1f("circleEdge",edgeSoft);
    mainShader.setUniform1i("mode",mode);
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    plane.draw();
    ofPopMatrix();
    mainShader.end();
    syphonClient.unbind();
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
    if(name == "pixelWidth" || name == "pixelWidthLarge"){
        ((ofxUITextInput*)settingCanvas->getWidget("pixelWidthText"))->setTextString(ofToString(float(desideredPixelWidthLarge)+desideredPixelWidth));
    }else if(name == "pixelWidthText"){
        string val = ((ofxUITextInput*)e.widget)->getTextString();
        float fval = ofToFloat(val);
        desideredPixelWidth = fmodf(fval,1.0);
        desideredPixelWidthLarge = floor(fval);
        
        
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
    }else if(name == "Mode"){
        ofxUIRadio *mode_list = (ofxUIRadio*)e.widget;
        if(mode_list->getActiveName() == "Round"){
            mode = 0;
        }else {
            mode = 1;
        }
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