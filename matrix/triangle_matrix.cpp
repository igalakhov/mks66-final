//
// Created by Ivan Galakhov on 2019-03-23.
//

#include "triangle_matrix.h"

TriangleMatrix::TriangleMatrix() : UnitMatrix(3) {}

void TriangleMatrix::add_triangle(float_mat x0, float_mat y0, float_mat z0,
                                  float_mat x1, float_mat y1, float_mat z1,
                                  float_mat x2, float_mat y2, float_mat z2) {
    add_unit({x0, y0, z0, x1, y1, z1, x2, y2, z2});

}



void TriangleMatrix::make_vertex_normals() {
    // clear the hash map
    vertex_normals.clear();

    int n = size();
    float_mat *s = start();
    float_mat * normal;

    while (n > 0) {

        float_mat A[] = {
                s[4] - s[0],
                s[5] - s[1],
                s[6] - s[2]
        };

        float_mat B[] = {
                s[8] - s[0],
                s[9] - s[1],
                s[10] - s[2]
        };

        normal = normalize(cross_product(A, B));

        auto key0 = getkey(s[0], s[1], s[2]);
        auto key1 =  getkey(s[4], s[5], s[6]);
        auto key2 =  getkey(s[8], s[9], s[10]);


        if(vertex_normals.count(key0) == 0)
            vertex_normals[key0] = empty_vector();

        add(vertex_normals[key0], normal);

        if(vertex_normals.count(key1) == 0)
            vertex_normals[key1] = empty_vector();

        add(vertex_normals[key1], normal);

        if(vertex_normals.count(key2) == 0)
            vertex_normals[key2] = empty_vector();

        add(vertex_normals[key2], normal);

        s += 12;
        n--;
    }

    // iterate and normalize everything
    for(const auto kv : vertex_normals)
        normalize_in_place(kv.second);
    //cur_triangles->vertex_normals = vertex_normals;

    for(const auto kv : vertex_normals){
        //printf("%f %f %f\n", kv.second[0], kv.second[1], kv.second[1]);
        //assert(kv.second[0] <= 1 and kv.second[1] <= 1 and kv.second[2] <= 1);
    }

}

// returns the vertex normal if the vertex exists
// else throws an error
float_mat * TriangleMatrix::get_vertex_normal(float_mat * s) {
    auto key = getkey(s[0], s[1], s[2]);

    if(vertex_normals.count(key) != 0)
        return vertex_normals[key];

    return s;

}

unsigned long TriangleMatrix::getkey(float_mat a, float_mat b, float_mat c){
    return (unsigned long) std::round(a*10000000 + b*1000000 + c*1000000000000000);
}