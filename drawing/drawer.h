//
// Created by Ivan Galakhov on 2019-02-06.
//
#include "display.h"
#include "../matrix/edge_matrix.h"
#include "../compiler/symbol_table.h"

#ifndef WORK_01_LINE_DRAWER_H
#define WORK_01_LINE_DRAWER_H

#include <cassert>
#include "display.h"
#include <cmath>
#include "../matrix/point_matrix.h"
#include "../matrix/edge_matrix.h"
#include "../matrix/triangle_matrix.h"
#include "../matrix/utils/vector_utils.h"
#include "lighting/lighting.h"

#define SHADING_FLAT 1337
#define SHADING_GOURAUD 2019
#define SHADING_PHONG 2024

class Drawer : public Display {
public:
    Drawer(); // constructor
    ~Drawer(); // destructor

    // drawing basics
    void draw_points(PointMatrix *);
    void draw_edges(EdgeMatrix * );
    void draw_polygons(TriangleMatrix *, std::vector<double **> &, floating_color *, constants *);

    void draw_line(int, int, float_mat, int, int, float_mat);
    void draw_line_new(int, int, float_mat, int, int, float_mat);
    void draw_line_simon(double, double, double, double, double, double);
    void draw_line_steep(int, int, float_mat, int, int, float_mat, int, int, int);
    void draw_line_shallow(int, int, float_mat, int, int, float_mat, int, int, int);
    void scan_line(float_mat *, float_mat *, float_mat *);
    void scan_line_gouraund(TriangleMatrix *, std::vector<double **> &, floating_color *, constants *,
            float_mat *, float_mat *, float_mat *);
    void draw_line_color(float_mat, int, float_mat, floating_color &,
                        float_mat, int, float_mat, floating_color &);

    void draw_line_vector(float_mat, int, float_mat, float_mat *,
                          float_mat, int, float_mat, float_mat *,
                          std::vector<double **> &, floating_color *, constants *);

    void scan_line_phong(TriangleMatrix *, std::vector<double **> &, floating_color *, constants *,
                         float_mat *, float_mat *, float_mat *);
    void change_color(unsigned char, unsigned char, unsigned char);
    void set(float_mat, float_mat, float_mat); // set a specific point to the current color

    void set_shading(int);


    // anti aliasing algo
    void draw_line_wu(float_mat, float_mat, float_mat, float_mat, float_mat, float_mat);
    int ipart(float_mat);
    int round(float_mat);
    float_mat fpart(float_mat);
    float_mat rfpart(float_mat);





protected:

private:
    struct color cur_color; // current drawing color

    float_mat * z_buffer;

    int shading_type;
};


#endif //WORK_01_LINE_DRAWER_H
