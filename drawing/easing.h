//
// Created by Ivan Galakhov on 2019-05-23.
//

#ifndef WORK_01_LINE_EASING_H
#define WORK_01_LINE_EASING_H

#include <math.h>

#ifndef PI
#define PI  3.14159265
#endif

class Bounce {

public:

    static float easeIn(float t,float b , float c, float d);
    static float easeOut(float t,float b , float c, float d);
    static float easeInOut(float t,float b , float c, float d);
};

class Sine {

public:

    static float easeIn(float t,float b , float c, float d);
    static float easeOut(float t,float b , float c, float d);
    static float easeInOut(float t,float b , float c, float d);

};

#define BOUNCE_IN 1
#define BOUNCE_OUT 2
#define BOUNCE_IN_OUT 3
#define SINE_IN 4
#define SINE_OUT 5
#define SINE_IN_OUT 6

#endif //WORK_01_LINE_EASING_H
