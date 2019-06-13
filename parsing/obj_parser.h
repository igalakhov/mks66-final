//
// Created by Ivan Galakhov on 2019-02-28.
//

#ifndef WORK_01_LINE_OBJ_PARSER_H
#define WORK_01_LINE_OBJ_PARSER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

#include "../settings.h"
#include "../matrix/edge_matrix.h"
#include "../matrix/triangle_matrix.h"
#include "../drawing/drawer.h"
#include <cstring>
#include <string.h>

class OBJFileParser {
public:
    // currently supports
    // ambient, diffuse, and specular coefficients, and the specular exponent
    // everything else is ignored
    static void draw_file(const char *, Drawer *, std::vector<double **> &, floating_color *, constants *, TransformationMatrix *);
    static std::unordered_map<std::string, constants *> get_materials(const char *);
    static int get_first_int(std::string);
    static constants * new_material();


    OBJFileParser(std::string);
    ~OBJFileParser();

    EdgeMatrix * get_edge_matrix();
    TriangleMatrix * get_triangle_matrix();


private:
    void parse_file();
    std::string file_name;
    std::vector<float_mat *> vertex_list;
    std::vector<int *> face_list;

};


#endif //WORK_01_LINE_OBJ_PARSER_H
