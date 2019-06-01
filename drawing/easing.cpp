// easing functions

#include "easing.h"

#pragma GCC diagnostic ignored "-Wunsequenced" // lol

// bounce
float Bounce::easeIn(float t, float b, float c, float d) {
    return c - easeOut(d - t, 0, c, d) + b;
}

float Bounce::easeOut(float t, float b, float c, float d) {
    if ((t /= d) < (1 / 2.75f)) {
        return c * (7.5625f * t * t) + b;
    } else if (t < (2 / 2.75f)) {
        float postFix = t -= (1.5f / 2.75f);
        return c * (7.5625f * (postFix) * t + .75f) + b;
    } else if (t < (2.5 / 2.75)) {
        float postFix = t -= (2.25f / 2.75f);
        return c * (7.5625f * (postFix) * t + .9375f) + b;
    } else {
        float postFix = t -= (2.625f / 2.75f);
        return c * (7.5625f * (postFix) * t + .984375f) + b;
    }
}

float Bounce::easeInOut(float t, float b, float c, float d) {
    if (t < d / 2) return easeIn(t * 2, 0, c, d) * .5f + b;
    else return easeOut(t * 2 - d, 0, c, d) * .5f + c * .5f + b;
}


// Sine
float Sine::easeIn(float t, float b, float c, float d) {
    return -c * cos(t / d * (PI / 2)) + c + b;
}

float Sine::easeOut(float t, float b, float c, float d) {
    return c * sin(t / d * (PI / 2)) + b;
}

float Sine::easeInOut(float t, float b, float c, float d) {
    return -c / 2 * (cos(PI * t / d) - 1) + b;
}


float Back::easeIn(float t, float b, float c, float d) {
    float s = 1.70158f;
    float postFix = t /= d;
    return c * (postFix) * t * ((s + 1) * t - s) + b;
}

float Back::easeOut(float t, float b, float c, float d) {
    float s = 1.70158f;
    return c * ((t = t / d - 1) * t * ((s + 1) * t + s) + 1) + b;
}

float Back::easeInOut(float t, float b, float c, float d) {
    float s = 1.70158f;
    if ((t /= d / 2) < 1) return c / 2 * (t * t * (((s *= (1.525f)) + 1) * t - s)) + b;
    float postFix = t -= 2;
    return c / 2 * ((postFix) * t * (((s *= (1.525f)) + 1) * t + s) + 2) + b;
}

float Circ::easeIn(float t, float b, float c, float d) {
    return -c * (sqrt(1 - (t /= d) * t) - 1) + b;
}

float Circ::easeOut(float t, float b, float c, float d) {
    return c * sqrt(1 - (t = t / d - 1) * t) + b;
}

float Circ::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return -c / 2 * (sqrt(1 - t * t) - 1) + b;
    return c / 2 * (sqrt(1 - t * (t -= 2)) + 1) + b;
}

float Cubic::easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t * t + b;
}

float Cubic::easeOut(float t, float b, float c, float d) {
    return c * ((t = t / d - 1) * t * t + 1) + b;
}

float Cubic::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return c / 2 * t * t * t + b;
    return c / 2 * ((t -= 2) * t * t + 2) + b;
}

float Elastic::easeIn(float t, float b, float c, float d) {
    if (t == 0) return b;
    if ((t /= d) == 1) return b + c;
    float p = d * .3f;
    float a = c;
    float s = p / 4;
    float postFix = a * pow(2, 10 * (t -= 1)); // this is a fix, again, with post-increment operators
    return -(postFix * sin((t * d - s) * (2 * PI) / p)) + b;
}

float Elastic::easeOut(float t, float b, float c, float d) {
    if (t == 0) return b;
    if ((t /= d) == 1) return b + c;
    float p = d * .3f;
    float a = c;
    float s = p / 4;
    return (a * pow(2, -10 * t) * sin((t * d - s) * (2 * PI) / p) + c + b);
}

float Elastic::easeInOut(float t, float b, float c, float d) {
    if (t == 0) return b;
    if ((t /= d / 2) == 2) return b + c;
    float p = d * (.3f * 1.5f);
    float a = c;
    float s = p / 4;

    if (t < 1) {
        float postFix = a * pow(2, 10 * (t -= 1)); // postIncrement is evil
        return -.5f * (postFix * sin((t * d - s) * (2 * PI) / p)) + b;
    }
    float postFix = a * pow(2, -10 * (t -= 1)); // postIncrement is evil
    return postFix * sin((t * d - s) * (2 * PI) / p) * .5f + c + b;
}

float Expo::easeIn(float t, float b, float c, float d) {
    return (t == 0) ? b : c * pow(2, 10 * (t / d - 1)) + b;
}

float Expo::easeOut(float t, float b, float c, float d) {
    return (t == d) ? b + c : c * (-pow(2, -10 * t / d) + 1) + b;
}

float Expo::easeInOut(float t, float b, float c, float d) {
    if (t == 0) return b;
    if (t == d) return b + c;
    if ((t /= d / 2) < 1) return c / 2 * pow(2, 10 * (t - 1)) + b;
    return c / 2 * (-pow(2, -10 * --t) + 2) + b;
}

float Linear::easeNone(float t, float b, float c, float d) {
    return c * t / d + b;
}

float Linear::easeIn(float t, float b, float c, float d) {
    return c * t / d + b;
}

float Linear::easeOut(float t, float b, float c, float d) {
    return c * t / d + b;
}

float Linear::easeInOut(float t, float b, float c, float d) {
    return c * t / d + b;
}

float Quad::easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t + b;
}

float Quad::easeOut(float t, float b, float c, float d) {
    return -c * (t /= d) * (t - 2) + b;
}

float Quad::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return ((c / 2) * (t * t)) + b;
    return -c / 2 * (((t - 2) * (--t)) - 1) + b;
    /*
    originally return -c/2 * (((t-2)*(--t)) - 1) + b;

    I've had to swap (--t)*(t-2) due to diffence in behaviour in
    pre-increment operators between java and c++, after hours
    of joy
    */

}

float Quart::easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t * t * t + b;
}

float Quart::easeOut(float t, float b, float c, float d) {
    return -c * ((t = t / d - 1) * t * t * t - 1) + b;
}

float Quart::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return c / 2 * t * t * t * t + b;
    return -c / 2 * ((t -= 2) * t * t * t - 2) + b;
}

float Quint::easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t * t * t * t + b;
}

float Quint::easeOut(float t, float b, float c, float d) {
    return c * ((t = t / d - 1) * t * t * t * t + 1) + b;
}

float Quint::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return c / 2 * t * t * t * t * t + b;
    return c / 2 * ((t -= 2) * t * t * t * t + 2) + b;
}

float get_easing(float t, float b, float c, float d, int f){
    switch(f){
        case BACK_IN: {
            return Back::easeIn(t, b, c, d);
        }

        case BACK_OUT: {
            return Back::easeOut(t, b, c, d);
        }

        case BACK_INOUT: {
            return Back::easeInOut(t, b, c, d);
        }

        case BOUNCE_IN: {
            return Bounce::easeIn(t, b, c, d);
        }

        case BOUNCE_OUT: {
            return Bounce::easeOut(t, b, c, d);
        }

        case BOUNCE_INOUT: {
            return Bounce::easeInOut(t, b, c, d);
        }

        case CIRC_IN: {
            return Circ::easeIn(t, b, c, d);
        }

        case CIRC_OUT: {
            return Circ::easeOut(t, b, c, d);
        }

        case CIRC_INOUT: {
            return Circ::easeInOut(t, b, c, d);
        }

        case CUBIC_IN: {
            return Cubic::easeIn(t, b, c, d);
        }

        case CUBIC_OUT: {
            return Cubic::easeOut(t, b, c, d);
        }

        case CUBIC_INOUT: {
            return Cubic::easeInOut(t, b, c, d);
        }

        case ELASTIC_IN: {
            return Elastic::easeIn(t, b, c, d);
        }

        case ELASTIC_OUT: {
            return Elastic::easeOut(t, b, c, d);
        }

        case ELASTIC_INOUT: {
            return Elastic::easeInOut(t, b, c, d);
        }

        case EXPO_IN: {
            return Expo::easeIn(t, b, c, d);
        }

        case EXPO_OUT: {
            return Expo::easeOut(t, b, c, d);
        }

        case EXPO_INOUT: {
            return Expo::easeInOut(t, b, c, d);
        }

        case QUAD_IN: {
            return Quad::easeIn(t, b, c, d);
        }

        case QUAD_OUT: {
            return Quad::easeOut(t, b, c, d);
        }

        case QUAD_INOUT: {
            return Quad::easeInOut(t, b, c, d);
        }


        case QUART_IN: {
            return Quart::easeIn(t, b, c, d);
        }

        case QUART_OUT: {
            return Quart::easeOut(t, b, c, d);
        }

        case QUART_INOUT: {
            return Quart::easeInOut(t, b, c, d);
        }

        case QUINT_IN: {
            return Quint::easeIn(t, b, c, d);
        }

        case QUINT_OUT: {
            return Quint::easeOut(t, b, c, d);
        }

        case QUINT_INOUT: {
            return Quint::easeInOut(t, b, c, d);
        }

        case SINE_IN: {
            return Sine::easeIn(t, b, c, d);
        }

        case SINE_OUT: {
            return Sine::easeOut(t, b, c, d);
        }

        case SINE_INOUT: {
            return Sine::easeInOut(t, b, c, d);
        }




        default: {
            return Linear::easeInOut(t, b, c, d);
            break;
        }
    }
}

int get_easing_func(const char * name, const char * endpoints){
//    printf("%s\n", name);
//    printf("%s\n", endpoints);

    if(!(endpoints == nullptr or !strcmp(endpoints, "inout") or !strcmp(endpoints, "in") or !strcmp(endpoints, "out"))){
        if(endpoints != nullptr) {
            std::printf("Unrecognized easing endpoints option [%s]\n", endpoints);
            std::printf("Using [inout] instead\n");
        }
        endpoints = "inout";
    }

    if(name == nullptr or !strcmp(name, "linear"))
        return LINEAR_INOUT;

    if(!strcmp(name, "back")){
        if(endpoints == nullptr or !strcmp(endpoints, "inout"))
            return BACK_INOUT;
        if(!strcmp(endpoints, "in"))
            return BACK_IN;
        if(!strcmp(endpoints, "out"))
            return BACK_OUT;
    }

    if(!strcmp(name, "bounce")){
        if(endpoints == nullptr or !strcmp(endpoints, "inout"))
            return BOUNCE_INOUT;
        if(!strcmp(endpoints, "in"))
            return BOUNCE_IN;
        if(!strcmp(endpoints, "out"))
            return BOUNCE_OUT;
    }

    if(!strcmp(name, "circ")){
        if(endpoints == nullptr or !strcmp(endpoints, "inout"))
            return CIRC_INOUT;
        if(!strcmp(endpoints, "in"))
            return CIRC_IN;
        if(!strcmp(endpoints, "out"))
            return CIRC_OUT;
    }

    if(!strcmp(name, "cubic")){
        if(endpoints == nullptr or !strcmp(endpoints, "inout"))
            return CUBIC_INOUT;
        if(!strcmp(endpoints, "in"))
            return CUBIC_IN;
        if(!strcmp(endpoints, "out"))
            return CUBIC_OUT;
    }

    if(!strcmp(name, "elastic")){
        if(endpoints == nullptr or !strcmp(endpoints, "inout"))
            return ELASTIC_INOUT;
        if(!strcmp(endpoints, "in"))
            return ELASTIC_IN;
        if(!strcmp(endpoints, "out"))
            return ELASTIC_OUT;
    }

    if(!strcmp(name, "expo")){
        if(endpoints == nullptr or !strcmp(endpoints, "inout"))
            return EXPO_INOUT;
        if(!strcmp(endpoints, "in"))
            return EXPO_IN;
        if(!strcmp(endpoints, "out"))
            return EXPO_OUT;
    }

    if(!strcmp(name, "quad")){
        if(endpoints == nullptr or !strcmp(endpoints, "inout"))
            return QUAD_INOUT;
        if(!strcmp(endpoints, "in"))
            return QUAD_IN;
        if(!strcmp(endpoints, "out"))
            return QUAD_OUT;
    }

    if(!strcmp(name, "quart")){
        if(endpoints == nullptr or !strcmp(endpoints, "inout"))
            return QUART_INOUT;
        if(!strcmp(endpoints, "in"))
            return QUART_IN;
        if(!strcmp(endpoints, "out"))
            return QUART_OUT;
    }

    if(!strcmp(name, "quint")){
        if(endpoints == nullptr or !strcmp(endpoints, "inout"))
            return QUINT_INOUT;
        if(!strcmp(endpoints, "in"))
            return QUINT_IN;
        if(!strcmp(endpoints, "out"))
            return QUINT_OUT;
    }

    if(!strcmp(name, "sine")){
        if(endpoints == nullptr or !strcmp(endpoints, "inout"))
            return SINE_INOUT;
        if(!strcmp(endpoints, "in"))
            return SINE_IN;
        if(!strcmp(endpoints, "out"))
            return SINE_OUT;
    }


    // we didn't find anything
    std::printf("Unrecognized easing function [%s]\n", name);
    std::printf("Using [linear] instead\n");
    return LINEAR_INOUT;

}
