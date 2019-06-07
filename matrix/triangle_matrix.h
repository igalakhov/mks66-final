//
// Created by Ivan Galakhov on 2019-03-23.
//

#ifndef WORK_01_LINE_TRIANGLE_MATRIX_H
#define WORK_01_LINE_TRIANGLE_MATRIX_H

#include <iostream>
#include <thread>
#include <unordered_map>
#include "utils/vector_utils.h"
#include "math.h"

#include "unit_matrix.h"
#include "../settings.h"


class TriangleMatrix: public UnitMatrix {

public:
    TriangleMatrix();

    void add_triangle(float_mat, float_mat, float_mat, float_mat, float_mat, float_mat, float_mat, float_mat, float_mat);
    void make_vertex_normals();
    unsigned long getkey(float_mat, float_mat, float_mat);
    float_mat * get_vertex_normal(float_mat *);

    // list of normals
    std::unordered_map<unsigned long,float_mat *> vertex_normals;
};

#endif //WORK_01_LINE_TRIANGLE_MATRIX_H
