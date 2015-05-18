//
//  DoubleEndedPositioner.h
//  lowResPreview
//
//  Created by Isaac Gierard on 5/12/15.
//
//

#ifndef __lowResPreview__DoubleEndedPositioner__
#define __lowResPreview__DoubleEndedPositioner__
#include "ofMain.h"

class EndPoint{
public:
    EndPoint();
    ~EndPoint();
    ofVec2f pos;
    void draw();
    bool isDraging;
    void onMouseDown(ofMouseEventArgs &e);
    void onMouseUp(ofMouseEventArgs &e);
    void onMouseMove(ofMouseEventArgs &e);
};

class DoubleEndedPositioner{
public:
    DoubleEndedPositioner();
    EndPoint p1;
    EndPoint p2;
    void draw();
    float getLength();
    void onMouseMove(ofMouseEventArgs &e);
};

#endif /* defined(__lowResPreview__DoubleEndedPositioner__) */
