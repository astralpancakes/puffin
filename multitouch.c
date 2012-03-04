//
//  multitouch.c
//  Puffin
//
//  Created by Peter Schulman on 3/4/12.
//  Copyright 2012 -. All rights reserved.
//

#include "multitouch.h"


//#import <Cocoa/Cocoa.h>
//#include <math.h>
#include <stddef.h>
//#include <stdio.h>
//#include <CoreFoundation/CoreFoundation.h>

typedef struct { float x,y; } pufMultitouchPoint;
typedef struct { pufMultitouchPoint pos,vel; } pufMultitouchReadout;

typedef struct {
    int frame;
    double timestamp;
    int identifier, state, foo3, foo4;
    pufMultitouchReadout normalized;
    float size;
    int zero1;
    float angle, majorAxis, minorAxis; // ellipsoid
    pufMultitouchReadout mm;
    int zero2[2];
    float unk2;
} pufMultitouchFinger;

void (*pufMultitouchCallback)(float x, float y, float xRel, float yRel) = NULL;

typedef void *MTDeviceRef;
typedef int (*MTContactCallbackFunction)(int,pufMultitouchFinger*,int,double,int);

MTDeviceRef MTDeviceCreateDefault();
void MTRegisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
void MTDeviceStart(MTDeviceRef, int);



int pufInternMultitouchCallback(int device, pufMultitouchFinger *data, int nFingers, double timestamp, int frame) {
    for (int i=0; i<nFingers; i++) {
        pufMultitouchFinger *f = &data[i];
        /*
        printf("Frame %7d: Angle %6.2f, ellipse %6.3f x%6.3f; "
               "position (%6.3f,%6.3f) vel (%6.3f,%6.3f) "
               "ID %d, state %d [%d %d?] size %6.3f, %6.3f?\n",
               f->frame,
               f->angle * 90 / atan2(1,0),
               f->majorAxis,
               f->minorAxis,
               f->normalized.pos.x,
               f->normalized.pos.y,
               f->normalized.vel.x,
               f->normalized.vel.y,
               f->identifier, f->state, f->foo3, f->foo4,
               f->size, f->unk2);
        */
        (*pufMultitouchCallback)(f->normalized.pos.x, f->normalized.pos.y, f->normalized.vel.x, f->normalized.vel.y);
    }
    //printf("\n");
    return 0;
}

void pufSetCallbackMultitouch(void (*func)(float x,float y,float xRel,float yRel))
{
    pufMultitouchCallback = func;
    MTDeviceRef dev = MTDeviceCreateDefault();
    MTRegisterContactFrameCallback(dev, pufInternMultitouchCallback);
    MTDeviceStart(dev, 0);
}