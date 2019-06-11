//
// Created by Ivan Galakhov on 2019-02-28.
//

#include "obj_parser.h"

// strategy:
// first pass: get all triangles and materials: construct a vertex normal list
// second pass: actually draw everything to the drawer object

void OBJFileParser::draw_file(const char *file_name,
                              Drawer *d,
                              std::vector<double **> &sources,
                              floating_color *ambient,
                              constants *default_material,
                              TransformationMatrix *trans) {

    if (!std::ifstream(file_name).good()) {
        printf("Error parsing materials: file \"%s\" doesn't exist\n", file_name);
        return; // no materials parsed
    }

    // first: construct the triangle matrix and material hashmap
    std::vector<float_mat *> vertex_list;
    auto triangles = new TriangleMatrix();
    std::unordered_map<std::string, constants *> materials;

    std::ifstream file(file_name);
    std::string str;

    while (std::getline(file, str)) {
        std::string buf;
        std::stringstream ss(str);

        std::vector<std::string> tokens;

        while (ss >> buf)
            tokens.push_back(buf);

        if (tokens.empty())
            continue;

        // new face
        if (tokens.at(0) == "f") {
            // add faces to matrix
            for (int i = 3; i < tokens.size(); i++) {
                triangles->add_triangle(
                        vertex_list.at(get_first_int(tokens.at(1)) - 1)[0],
                        vertex_list.at(get_first_int(tokens.at(1)) - 1)[1],
                        vertex_list.at(get_first_int(tokens.at(1)) - 1)[2],
                        vertex_list.at(get_first_int(tokens.at(i - 1)) - 1)[0],
                        vertex_list.at(get_first_int(tokens.at(i - 1)) - 1)[1],
                        vertex_list.at(get_first_int(tokens.at(i - 1)) - 1)[2],
                        vertex_list.at(get_first_int(tokens.at(i)) - 1)[0],
                        vertex_list.at(get_first_int(tokens.at(i)) - 1)[1],
                        vertex_list.at(get_first_int(tokens.at(i)) - 1)[2]
                );
            }
        }

        // new vertex
        if (tokens.at(0) == "v") {
            vertex_list.push_back(
                    new float_mat[3]{
                            std::stof(tokens.at(1)),
                            std::stof(tokens.at(2)),
                            std::stof(tokens.at(3))
                    });
        }

        // new material
        if (tokens.at(0) == "mtllib") {
            std::unordered_map<std::string, constants *> new_material = get_materials(tokens.at(1).c_str());
            materials.insert(new_material.begin(), new_material.end());
        }

    }

    // early termination in case there are no materials
    if (materials.empty()) {
        triangles->apply_transformation(trans);
        d->draw_polygons(triangles, sources, ambient, default_material, true);
        return;
    }

    // middle step:
    triangles->apply_transformation(trans); // necessarry
    triangles->make_vertex_normals();
    std::unordered_map<unsigned long, float_mat *> vertex_normals = triangles->vertex_normals; // use this later

    //std::cout << vertex_normals << std::endl;


    // second step: iterate again with colors
    file.clear();
    file.seekg(0, std::ios::beg);
    constants *cur_mat = default_material;
    auto cur_triangles = new TriangleMatrix();

    std::string mat_name = "mat_default";

    while (std::getline(file, str)) {
        std::string buf;
        std::stringstream ss(str);

        std::vector<std::string> tokens;

        while (ss >> buf)
            tokens.push_back(buf);

        if (tokens.empty())
            continue;

        //printf("%s\n", tokens.at(0).c_str());

        if (tokens.at(0) == "usemtl") {
            // draw the old triangles on screen
            cur_triangles->apply_transformation(trans);
            cur_triangles->vertex_normals = vertex_normals;
            //cur_triangles->make_vertex_normals();
            d->draw_polygons(cur_triangles, sources, ambient, cur_mat, false);

            // set up for new material
            cur_triangles->clear();

            //std::cout << tokens.at(1) << std::endl;

            mat_name = tokens.at(1);

            if (materials.count(tokens.at(1)) != 0) {
                //printf("material?\n");
                cur_mat = materials[tokens.at(1)];
            } else {
                printf("Unrecognized material \"%s\". Reverting to default.\n", tokens.at(1).c_str());
                cur_mat = default_material;
            }

        }

        // new face
        if (tokens.at(0) == "f") {
            // add faces to matrix
            for (int i = 3; i < tokens.size(); i++) {
                cur_triangles->add_triangle(
                        vertex_list.at(get_first_int(tokens.at(1)) - 1)[0],
                        vertex_list.at(get_first_int(tokens.at(1)) - 1)[1],
                        vertex_list.at(get_first_int(tokens.at(1)) - 1)[2],
                        vertex_list.at(get_first_int(tokens.at(i - 1)) - 1)[0],
                        vertex_list.at(get_first_int(tokens.at(i - 1)) - 1)[1],
                        vertex_list.at(get_first_int(tokens.at(i - 1)) - 1)[2],
                        vertex_list.at(get_first_int(tokens.at(i)) - 1)[0],
                        vertex_list.at(get_first_int(tokens.at(i)) - 1)[1],
                        vertex_list.at(get_first_int(tokens.at(i)) - 1)[2]
                );
            }
        }
    }


    cur_triangles->apply_transformation(trans);
    cur_triangles->vertex_normals = vertex_normals;
    //cur_triangles->make_vertex_normals();
    d->draw_polygons(cur_triangles, sources, ambient, cur_mat, false);
}

int OBJFileParser::get_first_int(std::string w) {
    return std::stoi(w.find('/') != std::string::npos ? w.substr(0, w.find('/')) : w);
}

std::unordered_map<std::string, constants *> OBJFileParser::get_materials(const char *file_name) {
    std::unordered_map<std::string, constants *> ret;
    if (!std::ifstream(file_name).good()) {
        printf("Error parsing materials: file \"%s\" doesn't exist\n", file_name);
        return ret; // no materials parsed
    }

    std::ifstream file(file_name);
    std::string str;

    constants *cur_material = new_material();
    std::string cur_material_name;
    bool first_material = true; // so we don't reset multiple times

    while (std::getline(file, str)) {
        std::string buf;
        std::stringstream ss(str);

        std::vector<std::string> tokens;

        while (ss >> buf)
            tokens.push_back(buf);

        if (tokens.empty())
            continue;

        if (tokens.at(0) == "newmtl") {
            if(!first_material) {
                ret[cur_material_name] = cur_material;
                cur_material = new_material();
                cur_material_name = tokens.at(1);
            } else {
                cur_material_name = tokens.at(1);
                first_material = false;
            }
        }

        // parse other stuff
        if (tokens.at(0) == "Ka") {
            cur_material->r[AMBIENT_R] = std::stof(tokens.at(1));
            cur_material->g[AMBIENT_R] = std::stof(tokens.at(2));
            cur_material->b[AMBIENT_R] = std::stof(tokens.at(3));
        }

        if (tokens.at(0) == "Ks") {
            cur_material->r[SPECULAR_R] = std::stof(tokens.at(1));
            cur_material->g[SPECULAR_R] = std::stof(tokens.at(2));
            cur_material->b[SPECULAR_R] = std::stof(tokens.at(3));
        }

        if (tokens.at(0) == "Kd") {
            cur_material->r[DIFFUSE_R] = std::stof(tokens.at(1));
            cur_material->g[DIFFUSE_R] = std::stof(tokens.at(2));
            cur_material->b[DIFFUSE_R] = std::stof(tokens.at(3));
        }

        if (tokens.at(0) == "Ns") {
            cur_material->exp = std::stof(tokens.at(1));
        }
    }

    // add the last material
    ret[cur_material_name] = cur_material;


//    for (auto const &i : ret) {
//        printf("%s\n", i.first.c_str());
//        printf("%f %f %f\n", i.second->r[DIFFUSE_R], i.second->g[DIFFUSE_R], i.second->b[DIFFUSE_R]);
//        printf("%f %f %f\n", i.second->r[SPECULAR_R], i.second->g[SPECULAR_R], i.second->b[SPECULAR_R]);
//        printf("%f %f %f\n", i.second->r[AMBIENT_R], i.second->g[AMBIENT_R], i.second->b[AMBIENT_R]);
//    }

    return ret;
}

constants *OBJFileParser::new_material() {
    auto ret = (constants *) std::malloc(sizeof(constants));

    std::memset(ret, 0, sizeof(constants));

    ret->exp = SPECULAR_EXPONENT;
//    ret->r[0] = ret->r[1] = ret->r[2] = ret->r[3] = 0;
//    ret->g[0] = ret->g[1] = ret->g[2] = ret->g[3] = 0;
//    ret->b[0] = ret->b[1] = ret->b[2] = ret->b[3] = 0;
//
//    ret->red = ret->green = ret->blue;


    return ret;
}

// parse


TriangleMatrix *OBJFileParser::get_triangle_matrix() {
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

EdgeMatrix *OBJFileParser::get_edge_matrix() {
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
OBJFileParser::OBJFileParser(std::string file_name) {
    std::ifstream file(file_name);
    std::string str;

    std::vector<float_mat *> vertex_list;
    std::vector<int *> face_list;


    // iterate line by line, splitting each line by space
    while (std::getline(file, str)) {
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