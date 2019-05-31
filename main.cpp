#include <iostream>
#include <random>

#include "settings.h"
#include "compiler/parser.h"
#include "matrix/triangle_matrix.h"
#include "matrix/edge_matrix.h"
#include "drawing/drawer.h"
#include "build/mdl.tab.hh"
#include "compiler/symbol_table.h"
#include "parsing/mdl/coordinate_stack.h"
#include "drawing/3d/3d.h"
#include "parsing/obj_parser.h"
#include "drawing/easing.h"
// "globals"



// TODO: move this to parser.h
static struct vary_node ** knobs;
static std::string basename = "myanimation";
static int num_frames = 1;
static bool animate = 0;

void first_pass();

void second_pass();

void third_pass();

void render_frame(int);


void my_main() {

    auto d = new Drawer();

    //d->draw_line_wu(0, 0, 0, 300, 400, 0);
    //d->draw_line_wu(100, 400.5, 0, 100, 100.5, 0);

    //d->display();


    //return;
    first_pass();


    if(num_frames > 1) {
        std::printf("Generating animation with %d frames, with basename %s\n", num_frames, basename.c_str());
        std::system("mkdir -p anim");
        animate = true;
    } else
        std::printf("Generating single render\n");

//    std::cout << basename << std::endl;
//    std::cout << numframes << std::endl;

    // with that being said, do a second pass
    second_pass();

    third_pass();


    // print delta list
//    for(int i = 0; i < num_frames; i++){
//        std::printf("Frame %d: ", i);
//        struct vary_node * cur = knobs[i];
//        while(cur != nullptr){
//            std::printf("[%s (%f)]", cur->name, cur->value);
//            cur = cur->next;
//        }
//        std::printf("\n");
//    }


    // convert to a gif if necessary
    if(num_frames > 1){
        // frame conversion code here
        std::system(("convert -delay 1.7 anim/" + basename + "* " + basename + ".gif").c_str());
        std::system("rm -rf anim");
        std::system(("animate " + basename + ".gif").c_str());

    }

    //s->print();



}

void third_pass(){
    for(int frame = 0; frame < num_frames; frame++){
        std::printf("Generating Frame #%d\n", frame);

        // update symbol table
        struct vary_node * cur = knobs[frame];
        while(cur != nullptr){
//            printf("??\n");
            s->lookup_symbol(cur->name)->s.val = cur->value;
            cur = cur->next;
        }
        s->print_values();

        // actually render
        render_frame(frame);

    }
}

void add_value(vary_node **knobs, int frame, char *name, double delta){
    if(frame < 0 or frame >= num_frames) // don't do anything for invalid frames
        return;

    assert(0 <= frame and frame < num_frames); // so we don't get a weird null pointer
    vary_node * cur = knobs[frame];

    // new node
    auto new_node = (struct vary_node * ) malloc(sizeof(struct vary_node));

    new_node->name = name;
    new_node->next = nullptr;
    new_node->value = delta;

    if(cur == nullptr) {
        knobs[frame] = new_node;
    } else {
        while (cur->next != nullptr)
            cur = cur->next;
        cur->next = new_node;
    }

}

void second_pass(){
    knobs = (struct vary_node **)calloc((size_t) num_frames, sizeof(struct vary_node *));
    // set all knobs to null
    for(int i = 0; i < num_frames; i++)
        knobs[i] = nullptr;

    // look for vary commands
    for (int i = 0; i < lastop; i++) {

        //std::cout << "yes" << std::endl;

        //printf("%d: \n", i);
        struct command cur = op[i];
        switch (cur.opcode) {

            case VARY: {
//                std::printf("%s: frame %d to %d, vary %d to %d\n", cur.op.vary.p->name,
//                            (int) std::round(cur.op.vary.start_frame),
//                            (int) std::round(cur.op.vary.end_frame),
//                            (int) std::round(cur.op.vary.start_val),
//                            (int) std::round(cur.op.vary.end_val));

                assert(cur.op.vary.start_frame != cur.op.vary.end_frame); // no weird errors

                double delta = (cur.op.vary.end_val - cur.op.vary.start_val)/(cur.op.vary.end_frame - cur.op.vary.start_frame);

                int t = 0;
                double b = cur.op.vary.start_val;
                double c = cur.op.vary.end_val - cur.op.vary.start_val;
                double d = cur.op.vary.end_frame - cur.op.vary.start_frame;

//                std::cout << "HELLO" << std::endl;

                //std::printf("%d, %d\n", (int)std::round(cur.op.vary.start_frame) + 1, (int)std::round(cur.op.vary.end_frame));

                double prev = cur.op.vary.start_val;

                // figure out which function we need
                int easing_function = SINE_IN;

                for(int j = (int)std::round(cur.op.vary.start_frame); j <= (int)std::round(cur.op.vary.end_frame); j++){

                    //std::printf("%d ", j);
                    //add_value(knobs, j, cur.op.vary.p->name, delta);

                    //std::printf("t: %d, b: %f, c: %f, d: %f\n", t, b, c, d);


                    // calculate value

                    double val = Sine::easeInOut(t, b, c, d);

//                    std::printf("val: %f\n", val);
////                    std::printf("prev: %f\n", prev);
//                    std::printf("change: %f\n", prev - val);

                    add_value(knobs, j, cur.op.vary.p->name, val);

                    prev = val;

                    t++;
                }

                //std::printf("%f\n", delta);

                // set the knob default

                break;
            }

            default: {
                break;
            }
        }

    }



}

void first_pass() {
    // find the basename and number of frames commands
    for (int i = 0; i < lastop; i++) {

        //std::cout << "yes" << std::endl;

        //printf("%d: \n", i);
        struct command cur = op[i];
        switch (cur.opcode) {

            case BASENAME: {
                basename = cur.op.basename.p->name;
                break;
            }

            case FRAMES: {
                num_frames = (int) std::round(cur.op.frames.num_frames);
                break;
            }

            default: {
                break;
            }
        }

    }

}


void render_frame(int frame) {
    // set up
    auto cord_stack = new CoordinateStack();

    auto edge_matrix = new EdgeMatrix();
    auto triangle_matrix = new TriangleMatrix();
    auto drawer = new Drawer();

    // default light
    auto default_light = (double **) std::malloc(sizeof(double **) * 2);
    default_light[0] = (double *) std::malloc(sizeof(double *) * 3);
    default_light[1] = (double *) std::malloc(sizeof(double *) * 3);
    default_light[0][0] = 0.5;
    default_light[0][1] = 0.75;
    default_light[0][2] = 1;
    //normalize_in_place(default_light[0]);

    default_light[1][0] = 255;
    default_light[1][1] = 255;
    default_light[1][2] = 255;

    // default reflect
    struct constants white;
    white.r[AMBIENT_R] = 0.1;
    white.g[AMBIENT_R] = 0.1;
    white.b[AMBIENT_R] = 0.1;

    white.r[DIFFUSE_R] = 0.5;
    white.g[DIFFUSE_R] = 0.5;
    white.b[DIFFUSE_R] = 0.5;

    white.r[SPECULAR_R] = 0.5;
    white.g[SPECULAR_R] = 0.5;
    white.b[SPECULAR_R] = 0.5;

    struct constants *default_constants;
    default_constants = &white;

    // values
    struct floating_color ambient = {50, 50, 50};

    bool custom_light = false;
    std::vector<double **> light_sources; // array of light sources
    light_sources.push_back(default_light);


    // actually run the commands
    for (int i = 0; i < lastop; i++) {

        //std::cout << "yes" << std::endl;

        //printf("%d: \n", i);
        struct command cur = op[i];
        switch (cur.opcode) {

            case TORUS: {
                add_torus(triangle_matrix,
                          cur.op.torus.d[0],
                          cur.op.torus.d[1],
                          cur.op.torus.d[2],
                          cur.op.torus.r0,
                          cur.op.torus.r1
                );
                triangle_matrix->apply_transformation(cord_stack->peek());
                if (cur.op.torus.constants != nullptr)
                    drawer->draw_polygons(triangle_matrix, light_sources, &ambient, cur.op.torus.constants->s.c);
                else
                    drawer->draw_polygons(triangle_matrix, light_sources, &ambient, default_constants);
                triangle_matrix->clear();
                break;
            }

                // shapes and displaying
            case BOX: {
                add_box(triangle_matrix,
                        cur.op.box.d0[0],
                        cur.op.box.d0[1],
                        cur.op.box.d0[2],
                        cur.op.box.d1[0],
                        cur.op.box.d1[1],
                        cur.op.box.d1[2]
                );
                triangle_matrix->apply_transformation(cord_stack->peek());
                if (cur.op.box.constants != nullptr)
                    drawer->draw_polygons(triangle_matrix, light_sources, &ambient, cur.op.box.constants->s.c);
                else
                    drawer->draw_polygons(triangle_matrix, light_sources, &ambient, default_constants);
                triangle_matrix->clear();
                break;
            }
            case SPHERE: {
                add_sphere(triangle_matrix,
                           cur.op.sphere.d[0],
                           cur.op.sphere.d[1],
                           cur.op.sphere.d[2],
                           cur.op.sphere.r);
                if (cur.op.sphere.cs != nullptr) {
                    //s->lookup_symbol(cur.op.sphere.cs->name)->s.m->print_self();
                    triangle_matrix->apply_transformation(s->lookup_symbol(cur.op.sphere.cs->name)->s.m);
                } else
                    triangle_matrix->apply_transformation(cord_stack->peek());
                if (cur.op.sphere.constants != nullptr)
                    drawer->draw_polygons(triangle_matrix, light_sources, &ambient, cur.op.sphere.constants->s.c);
                else
                    drawer->draw_polygons(triangle_matrix, light_sources, &ambient, default_constants);
                triangle_matrix->clear();
                break;
            }
            case LINE: {
                auto temp = new EdgeMatrix();

                temp->add_edge(cur.op.line.p0[0],
                               cur.op.line.p0[1],
                               cur.op.line.p0[2],
                               cur.op.line.p1[0],
                               cur.op.line.p1[1],
                               cur.op.line.p1[2]
                );

                temp->apply_transformation(cord_stack->peek());

                drawer->draw_edges(edge_matrix);

                break;
            }


                // stacks and transformations
            case MOVE: {
                double delta = 1;
                if(cur.op.move.p != nullptr)
                    delta = cur.op.move.p->s.val;
                cord_stack->apply_transformation(TransformationMatrix::translation(
                        cur.op.move.d[0] * delta,
                        cur.op.move.d[1] * delta,
                        cur.op.move.d[2] * delta
                ));
                break;
            }

            case ROTATE: {
                double delta = 1;
                if(cur.op.rotate.p != nullptr)
                    delta = cur.op.rotate.p->s.val;

                switch ((int) std::round(cur.op.rotate.axis)) {
                    case 0:
                        cord_stack->apply_transformation(TransformationMatrix::rotationX(cur.op.rotate.degrees * delta));
                        break;
                    case 1:
                        cord_stack->apply_transformation(TransformationMatrix::rotationY(cur.op.rotate.degrees * delta));
                        break;
                    case 2:
                        cord_stack->apply_transformation(TransformationMatrix::rotationZ(cur.op.rotate.degrees * delta));
                        break;
                    default:
                        break;
                }
                break;
            }

            case SCALE: {
                double delta = 1;
                if(cur.op.scale.p != nullptr)
                    delta = cur.op.scale.p->s.val;

                cord_stack->apply_transformation(TransformationMatrix::dilation(
                        cur.op.scale.d[0] * delta,
                        cur.op.scale.d[1] * delta,
                        cur.op.scale.d[2] * delta
                ));
                break;
            }

            case POP: {
                cord_stack->pop();
                break;
            }

            case PUSH: {
                cord_stack->push();
                break;
            }

            case SAVE_COORDS: {
                //cord_stack->peek()->print_self();
                s->lookup_symbol(cur.op.save_coordinate_system.p->name)->s.m = cord_stack->peek();
                //s->add_symbol(cur.op.save_coordinate_system.p->name, SYM_MATRIX, cord_stack->peek());
                break;
            }

                // lighting

                // add a new light source to the current light source list
            case LIGHT: {
                //std::cout << "new light source" << std::endl;
                // remove the first light if this is a new light
                if (!custom_light) {
                    light_sources.erase(light_sources.begin()); // remove the default light
                    custom_light = true;
                }

                auto new_light = (double **) std::malloc(sizeof(double **) * 2);
                new_light[0] = (double *) std::malloc(sizeof(double *) * 3);
                new_light[1] = (double *) std::malloc(sizeof(double *) * 3);
                new_light[0][0] = cur.op.light.p->s.l->l[0];
                new_light[0][1] = cur.op.light.p->s.l->l[1];
                new_light[0][2] = cur.op.light.p->s.l->l[2];
                new_light[1][0] = cur.op.light.p->s.l->c[0];
                new_light[1][1] = cur.op.light.p->s.l->c[1];;
                new_light[1][2] = cur.op.light.p->s.l->c[2];;
                normalize_in_place(new_light[0]);

                light_sources.push_back(new_light);

                break;
            }

                // set new ambient color
            case AMBIENT: {
                ambient.r = cur.op.ambient.c[0];
                ambient.g = cur.op.ambient.c[1];
                ambient.b = cur.op.ambient.c[2];
                break;
            }

            case DISPLAY: {
                if(!animate)
                    drawer->display();
                break;
            }

            case SAVE: {
                drawer->save(cur.op.save.p->name, "dummy");
                break;
            }

                // stuff we don't need to do anything for since the parser and lexer do it for us
            case VARY:
            case BASENAME:
            case FRAMES:
            case CONSTANTS:
                break;

                // mesh
            case MESH: {
                auto parser = new OBJFileParser(cur.op.mesh.name);
                triangle_matrix = parser->get_triangle_matrix();
                triangle_matrix->apply_transformation(cord_stack->peek());
                if (cur.op.mesh.constants != nullptr)
                    drawer->draw_polygons(triangle_matrix, light_sources, &ambient, cur.op.mesh.constants->s.c);
                else
                    drawer->draw_polygons(triangle_matrix, light_sources, &ambient, default_constants);

                triangle_matrix->clear();

                //yeah
                break;
            }

                // default case (this means the command isn't implemented)
            default: {
                printf("Command with opcode [%d] is not yet implemented\n", cur.opcode);
                break;
            }
        }
    }

    // save the drawer after we ran all our commands
    char buff[100];
    snprintf(buff, sizeof(buff), "%06d", frame);
    std::string buffAsStdStr = buff;
    //std::cout << "anim/" + basename + "" + buffAsStdStr + ".png" << std::endl;

    drawer->save("anim/" + basename + "" + buffAsStdStr + ".png", "dummy");
}

