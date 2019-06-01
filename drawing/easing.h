//
// Created by Ivan Galakhov on 2019-05-23.
//

#ifndef WORK_01_LINE_EASING_H
#define WORK_01_LINE_EASING_H

#include <math.h>
#include <cstring>
#include <iostream>

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

class Back {

public:

    static float easeIn(float t,float b , float c, float d);
    static float easeOut(float t,float b , float c, float d);
    static float easeInOut(float t,float b , float c, float d);

};

class Circ {

public:

    static float easeIn(float t,float b , float c, float d);
    static float easeOut(float t,float b , float c, float d);
    static float easeInOut(float t,float b , float c, float d);
};

class Cubic {

public:

    static float easeIn(float t,float b , float c, float d);
    static float easeOut(float t,float b , float c, float d);
    static float easeInOut(float t,float b , float c, float d);
};

class Elastic {

public:

    static float easeIn(float t,float b , float c, float d);
    static float easeOut(float t,float b , float c, float d);
    static float easeInOut(float t,float b , float c, float d);
};

class Expo {

public:

    static float easeIn(float t,float b , float c, float d);
    static float easeOut(float t,float b , float c, float d);
    static float easeInOut(float t,float b , float c, float d);
};

class Linear {

public:

    static float easeNone(float t,float b , float c, float d); // ??
    static float easeIn(float t,float b , float c, float d);
    static float easeOut(float t,float b , float c, float d);
    static float easeInOut(float t,float b , float c, float d);

};

class Quad {

public:

    static float easeIn(float t,float b , float c, float d);
    static float easeOut(float t,float b , float c, float d);
    static float easeInOut(float t,float b , float c, float d);
};

class Quart {

public:

    static float easeIn(float t,float b , float c, float d);
    static float easeOut(float t,float b , float c, float d);
    static float easeInOut(float t,float b , float c, float d);
};

class Quint {

public:

    static float easeIn(float t,float b , float c, float d);
    static float easeOut(float t,float b , float c, float d);
    static float easeInOut(float t,float b , float c, float d);
};

// make an easing function here

#define BACK_IN 0
#define BACK_OUT 1
#define BACK_INOUT 2

#define BOUNCE_IN 3
#define BOUNCE_OUT 4
#define BOUNCE_INOUT 5

#define CIRC_IN 6
#define CIRC_OUT 7
#define CIRC_INOUT 8

#define CUBIC_IN 9
#define CUBIC_OUT 10
#define CUBIC_INOUT 11

#define ELASTIC_IN 12
#define ELASTIC_OUT 13
#define ELASTIC_INOUT 14

#define EXPO_IN 15
#define EXPO_OUT 16
#define EXPO_INOUT 17

#define LINEAR_IN 18
#define LINEAR_OUT 19
#define LINEAR_INOUT 20

#define QUAD_IN 21
#define QUAD_OUT 22
#define QUAD_INOUT 23

#define QUART_IN 24
#define QUART_OUT 25
#define QUART_INOUT 26

#define QUINT_IN 27
#define QUINT_OUT 28
#define QUINT_INOUT 29

#define SINE_IN 30
#define SINE_OUT 31
#define SINE_INOUT 32

int get_easing_func(const char *, const char *);
float get_easing(float, float, float, float, int);


#endif //WORK_01_LINE_EASING_H
