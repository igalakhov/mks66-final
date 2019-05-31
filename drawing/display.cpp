//
// Created by Ivan Galakhov on 2019-02-06.
//

#include "display.h"
#include <fstream>
#include <sstream>
#include <cmath>

bool cmpf(double A, double B, double epsilon = 0.001f)
{
    return (std::fabs(A - B) < epsilon);
}

void Display::clear(){
    unsigned char *cur = values;

    // fill in values
    for (int i = 0; i < NUM_PIXELS * 3; i++) {
        switch (i % 3) {
            case 0:
                *cur = DEFAULT_COLOR_R;
                break;
            case 1:
                *cur = DEFAULT_COLOR_G;
                break;
            case 2:
                *cur = DEFAULT_COLOR_B;
                break;
            default: // never gets run anyways
                break;
        }

        cur++;
    }


    for(int i = 0; i < NUM_PIXELS; i++)
        z_buffer[i] = Z_BUFFER_MIN;

}

void Display::display(){
    FILE * f;

    f = popen("display", "w");

    fprintf(f, "P3\n%d %d\n%d\n", IMAGE_WIDTH, IMAGE_HEIGHT, MAX_COLOR);

    unsigned char * cur = values;

    for(int h = NUM_PIXELS * 3; h >= 0; h -= IMAGE_WIDTH * 3){
        for(int i = 0; i < IMAGE_WIDTH * 3; i++){
            fprintf(f, "%d ", (int) values[h + i]);
        }
    }

    for(int i = 0; i < NUM_PIXELS*3; i++, cur++)
        fprintf(f, "%d ", (int) * cur);

    pclose(f);

}

float_mat Display::constrain(float_mat val, float_mat b, float_mat u){
    if(val > u) return u;
    if(val < b) return b;
    return val;
}

void Display::set(float_mat float_x, float_mat float_y, float_mat z, float_mat opacity, struct color * to_set){

    int x = (int) std::round(float_x);
    int y = (int) std::round(float_y);

//    std::printf("%f, %f, %f, %f, %u, %u, %u, %u, %u, %u\n", float_x, float_y, z, opacity,
//            to_set->r, to_set->g, to_set->b,
//                (unsigned char) std::round(to_set->r * opacity),
//                (unsigned char) std::round(to_set->g * opacity),
//                (unsigned char) std::round(to_set->b * opacity));


    if(x < 0 or x > IMAGE_WIDTH - 1 or y < 0 or y > IMAGE_HEIGHT - 1)
        return;

    if(z_buffer[y*IMAGE_HEIGHT + x] - z <= 0.0001f) {
        unsigned char *cur = values + ((y * IMAGE_WIDTH) + x) * 3;



        if(touch_buffer[y*IMAGE_HEIGHT + x]){
            *cur = (unsigned char) constrain(*cur + std::round(to_set->r * opacity), 0, 255);
            cur++;
            *cur = (unsigned char) constrain(*cur + std::round(to_set->g * opacity), 0, 255);
            cur++;
            *cur = (unsigned char) constrain(*cur + std::round(to_set->b * opacity), 0, 255);
        } else {
            touch_buffer[y*IMAGE_HEIGHT + x] = true;
            *cur = (unsigned char) std::round(to_set->r * opacity);
            cur++;
            *cur = (unsigned char) std::round(to_set->g * opacity);
            cur++;
            *cur = (unsigned char) std::round(to_set->b * opacity);
        }

        // "efficiency"
//        *cur = to_set->r;
//        cur++;
//        *cur = to_set->g;
//        cur++;
//        *cur = to_set->b;

        z_buffer[y*IMAGE_HEIGHT + x] = z;

    }
}

void Display::set(float_mat float_x, float_mat float_y, float_mat z, struct color * to_set){

    //std::printf("%f, %f, %f\n", float_x, float_y, z);

    int x = (int) std::round(float_x);
    int y = (int) std::round(float_y);


    if(x < 0 or x > IMAGE_WIDTH - 1 or y < 0 or y > IMAGE_HEIGHT - 1)
        return;

    if(z_buffer[y*IMAGE_HEIGHT + x] - z <= 0.0001f) {
        unsigned char *cur = values + ((y * IMAGE_WIDTH) + x) * 3;

        // "efficiency"
        *cur = to_set->r;
        cur++;
        *cur = to_set->g;
        cur++;
        *cur = to_set->b;

        z_buffer[y*IMAGE_HEIGHT + x] = z;

    }
//    if (x == 360 and y == 128){
//        std::cout << z << std::endl;
//        std::cout << z_buffer[130 * IMAGE_HEIGHT + 260] << std::endl;
//    }


}

// default saves as ppm
// change extention to save as
void Display::save(std::string file_name, std::string extention){


    std::string no_extention = file_name.substr(0, file_name.rfind('.') + 1);

    std::string from = no_extention + "ppm";
    std::string to = no_extention + file_name.substr(file_name.rfind('.') + 1);


    char line[256];

    std::sprintf(line, "convert - %s", to.c_str());

    FILE * f = popen(line, "w");

    fprintf(f, "P3\n%d %d\n%d\n", IMAGE_WIDTH, IMAGE_HEIGHT, MAX_COLOR);

    for(int h = NUM_PIXELS * 3; h >= 0; h -= IMAGE_WIDTH * 3){
        for(int i = 0; i < IMAGE_WIDTH * 3; i++){
            fprintf(f, " %d ", (int) values[h + i]);
            //img_file << std::to_string((int) values[h + i]) << " ";
        }
    }

    pclose(f);


//    std::cout << from << std::endl;
//    std::cout << to << std::endl;

    // write values in display to file
//    std::ofstream img_file;
//    img_file.open(from, std::ofstream::trunc);
//
//    img_file << "P3\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n" << MAX_COLOR << "\n";
//
//    unsigned char *cur = values;
//
//    for(int h = NUM_PIXELS * 3; h >= 0; h -= IMAGE_WIDTH * 3){
//        for(int i = 0; i < IMAGE_WIDTH * 3; i++){
//            img_file << std::to_string((int) values[h + i]) << " ";
//        }
//    }
//
//    img_file.close();
//
//    std::system(("convert " + from + " " + to).c_str());
//    std::system(("rm " + from).c_str());


}

// constructor
Display::Display() {
    values = new unsigned char[NUM_PIXELS * 3];
    z_buffer = new float_mat[NUM_PIXELS];
    touch_buffer = new bool[NUM_PIXELS];


    for(int i = 0; i < NUM_PIXELS; i++) {
        z_buffer[i] = Z_BUFFER_MIN;
        touch_buffer[i] = false;
    }

    unsigned char *cur = values;

    // fill in values
    for (int i = 0; i < NUM_PIXELS * 3; i++) {
        switch (i % 3) {
            case 0:
                *cur = DEFAULT_COLOR_R;
                break;
            case 1:
                *cur = DEFAULT_COLOR_G;
                break;
            case 2:
                *cur = DEFAULT_COLOR_B;
                break;
            default: // never gets run anyways
                break;
        }

        cur++;
    }
}

// deconstructor
Display::~Display() {
    delete values;
    delete z_buffer;
}
