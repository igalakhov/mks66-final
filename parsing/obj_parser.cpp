//
// Created by Ivan Galakhov on 2019-02-28.
//

#include "obj_parser.h"

// strategy:
// first pass: get all triangles and materials: construct a vertex normal list
// second pass: actually draw everything to the drawer object

void OBJFileParser::draw_file(const char * name,
        Drawer * d,
        std::vector<double **> & sources,
        floating_color * ambient,
        constants * default_material) {




}

std::unordered_map<std::string, constants *> OBJFileParser::get_materials(const char * file_name) {
    std::unordered_map<std::string, constants *> ret;
    if(!std::ifstream(file_name).good()) {
        printf("Error parsing materials: file \"%s\" doesn't exist\n", file_name);
        return ret; // no materials parsed
    }

    std::ifstream file(file_name);
    std::string str;

    constants * cur_material = new_material();
    std::string cur_material_name;
    bool first_material = true; // so we don't reset multiple times

    while(std::getline(file, str)){
        std::string buf;
        std::stringstream ss(str);

        std::vector<std::string> tokens;

        while (ss >> buf)
            tokens.push_back(buf);

        if (tokens.empty())
            continue;

        if(tokens.at(0) == "newmtl"){
            printf("New material %s\n", tokens.at(1).c_str());
            if(!first_material) {
                ret[cur_material_name] = cur_material;
                cur_material = new_material();
                cur_material_name = tokens.at(1);
            } else {
                first_material = false;
            }
        }

        // parse other stuff
        if(tokens.at(0) == "Ka"){
            cur_material->r[AMBIENT_R] = std::stof(tokens.at(1));
            cur_material->g[AMBIENT_R] = std::stof(tokens.at(2));
            cur_material->b[AMBIENT_R] = std::stof(tokens.at(3));
        }

        if(tokens.at(0) == "Ks"){
            cur_material->r[SPECULAR_R] = std::stof(tokens.at(1));
            cur_material->g[SPECULAR_R] = std::stof(tokens.at(2));
            cur_material->b[SPECULAR_R] = std::stof(tokens.at(3));
        }

        if(tokens.at(0) == "Kd"){
            cur_material->r[DIFFUSE_R] = std::stof(tokens.at(1));
            cur_material->g[DIFFUSE_R] = std::stof(tokens.at(2));
            cur_material->b[DIFFUSE_R] = std::stof(tokens.at(3));
        }






        for(const std::string & i : tokens){
            //std::cout << i << std::endl;
        }
    }

    // add the last material
    ret[cur_material_name] = cur_material;


    return ret;
}

constants * OBJFileParser::new_material() {
    auto ret = (constants *) std::malloc(sizeof(constants));

    std::memset(ret, 0, sizeof(constants));
//    ret->r[0] = ret->r[1] = ret->r[2] = ret->r[3] = 0;
//    ret->g[0] = ret->g[1] = ret->g[2] = ret->g[3] = 0;
//    ret->b[0] = ret->b[1] = ret->b[2] = ret->b[3] = 0;
//
//    ret->red = ret->green = ret->blue;


    return ret;
}

// parse


TriangleMatrix * OBJFileParser::get_triangle_matrix(){
//    auto ret = new TriangleMatrix();
//
//    for(int * i : face_list){
//        float_mat * v0 = vertex_list.at(i[0] - 1);
//        float_mat * v1 = vertex_list.at(i[1] - 1);
//        float_mat * v2 = vertex_list.at(i[2] - 1);
//
//        ret->add_triangle(
//                v0[0], v0[1], v0[2],
//                v1[0], v1[1], v1[2],
//                v2[0], v2[1], v2[2]);
//    }
//
//    return ret;
    return nullptr;
}

EdgeMatrix * OBJFileParser::get_edge_matrix(){
//    auto ret = new EdgeMatrix();
//
//    for(int * i : face_list){
//        float_mat * v0 = vertex_list.at(i[0] - 1);
//        float_mat * v1 = vertex_list.at(i[1] - 1);
//        float_mat * v2 = vertex_list.at(i[2] - 1);
//
//        ret->add_edge(v0[0], v0[1], v0[2], v1[0], v1[1], v1[2]);
//        ret->add_edge(v0[0], v0[1], v0[2], v2[0], v2[1], v2[2]);
//        ret->add_edge(v2[0], v2[1], v2[2], v1[0], v1[1], v1[2]);
//    }
//
//    return ret;
    return nullptr;

}

// constructor
OBJFileParser::OBJFileParser(std::string file_name){
    std::ifstream file(file_name);
    std::string str;

    std::vector<float_mat *> vertex_list;
    std::vector<int *> face_list;


    // iterate line by line, splitting each line by space
    while (std::getline(file, str)){
        std::string buf;
        std::stringstream ss(str);

        std::vector<std::string> tokens;

//        while (ss >> buf)
//            tokens.push_back(buf);
//
//        if(tokens.empty())
//            continue;
//
//        // only look at vertex and faces now
//        if(tokens.at(0) == "v") {
//            vertex_list.push_back(new float_mat[3]{std::stof(tokens.at(1)), std::stof(tokens.at(2)), std::stof(tokens.at(3))});
//        }
//        if(tokens.at(0) == "f"){
//            face_list.push_back(new int[3]{std::stoi(tokens.at(1)), std::stoi(tokens.at(2)), std::stoi(tokens.at(3))});
//        }
    }

    // assign values
//    this->vertex_list = vertex_list;
//    this->face_list = face_list;
}

// destructor
OBJFileParser::~OBJFileParser() {
    // nothing here yet I guess
}