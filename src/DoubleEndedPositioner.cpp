//
//  DoubleEndedPositioner.cpp
//  lowResPreview
//
//  Created by Isaac Gierard on 5/12/15.
//
//

#include "DoubleEndedPositioner.h"

EndPoint::EndPoint():
pos(0,0),
isDraging(false){
    ofAddListener(ofEvents().mousePressed,this,&EndPoint::onMouseDown);
    ofAddListener(ofEvents().mouseReleased,this,&EndPoint::onMouseUp);
    ofAddListener(ofEvents().mouseDragged,this,&EndPoint::onMouseMove);
}
EndPoint::~EndPoint(){
    ofRemoveListener(ofEvents().mousePressed,this,&EndPoint::onMouseDown);
    ofRemoveListener(ofEvents().mouseReleased,this,&EndPoint::onMouseUp);
    ofRemoveListener(ofEvents().mouseDragged,this,&EndPoint::onMouseMove);
}
void EndPoint::draw(){
    ofSetColor(255, 255, 0);
    int size = 10;
    ofDrawLine(pos.x - (size * 0.5), pos.y, pos.x + (size * 0.5), pos.y);
    ofDrawLine(pos.x, pos.y - (size * 0.5), pos.x, pos.y + (size * 0.5));
}

void EndPoint::onMouseDown(ofMouseEventArgs &e){
    if((e - pos).length() < 10.0){
        this->isDraging = true;
        pos.x = e.x;
        pos.y = e.y;
    }
}
void EndPoint::onMouseUp(ofMouseEventArgs &e){
    this->isDraging = false;
}
void EndPoint::onMouseMove(ofMouseEventArgs &e){

    if(this->isDraging){
        pos.x = e.x;
        pos.y = e.y;
    }

}

DoubleEndedPositioner::DoubleEndedPositioner(){
    p2.pos.x = 10;
    p2.pos.y = 10;
}

void DoubleEndedPositioner::draw(){
    p1.draw();
    p2.draw();
    if(p2.isDraging && p1.isDraging){
        p2.isDraging = false;
    }
    ofSetColor(255, 255, 0);
    ofDrawLine(p1.pos,p2.pos);
}

float DoubleEndedPositioner::getLength(){
    ofVec2f d = p1.pos - p2.pos;
    return d.length();
}