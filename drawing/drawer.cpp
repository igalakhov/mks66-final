//
// Created by Ivan Galakhov on 2019-02-06.
//

#include "drawer.h"


void Drawer::draw_points(PointMatrix *m) {
    int n = m->size();
    float_mat *s = m->start();

    while (n > 0) {
        set((int) s[0], (int) s[1], (int) s[2]);

        s += 4;

        n -= 1;
    }
}


// draw polygons with ambient and constants
void Drawer::draw_polygons(TriangleMatrix *m, std::vector<double **> &sources, struct floating_color *ambient,
                           struct constants *cons) {


    // only do this if using PHONG or GOURAUND, not FLAT
    if(shading_type == SHADING_PHONG or shading_type == SHADING_GOURAUD)
        m->make_vertex_normals();

    // isolates the exact point there is a problem

    int n = m->size();// - 1194;
    float_mat *s = m->start();// + 12*269;

    while (n > 0) {

        bool draw = true;
        float_mat *N;

        if (ENABLE_BACKFACE_CULLING) {

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

            N = normalize(cross_product(A, B));


            if (dot_product(N, view) <= 0)
                draw = false;
        }

        if (draw) {

            // random colors because no lighting or z buffering

            cur_color = *total_lighting(N, sources, ambient, cons);

//            cur_color.r = (unsigned char)(std::rand() % 255);
//            cur_color.g = (unsigned char)(std::rand() % 255);
//            cur_color.b = (unsigned char)(std::rand() % 255);


            if(shading_type == SHADING_FLAT)
                scan_line(s + 0, s + 4, s + 8);

            if(shading_type == SHADING_PHONG)
                scan_line_phong(m, sources, ambient, cons, s + 0, s + 4, s + 8);

            if(shading_type == SHADING_GOURAUD)
                scan_line_gouraund(m, sources, ambient, cons, s + 0, s + 4, s + 8);

//            draw_line_wu(s[0], s[1], s[2],  s[4],  s[5], s[6]);
//            draw_line_wu( s[4],  s[5], s[6],  s[8],  s[9], s[10]);
//            draw_line_wu( s[0],  s[1], s[2],  s[8],  s[9], s[10]);

        }

        s += 12;

        n -= 1;
    }
}

void Drawer::set_shading(int t) {
    switch(t){
        case SHADING_FLAT:
        case SHADING_GOURAUD:
        case SHADING_PHONG: {
            shading_type = t;
            break;
        }

        default:
            break;
    }
}

void Drawer::draw_line_vector(float_mat x0, int y0, double z0, float_mat * n0,
                              double x1, int y1, double z1, float_mat * n1,
                              std::vector<double **> &sources, struct floating_color *ambient,
                              struct constants *cons){

    assert(y0 == y1);

    if(x1 < x0)
        return draw_line_vector(x1, y1, z1, n1, x0, y0, z0, n0, sources, ambient, cons);


    int x0i = (int) std::round(x0);
    int x1i = (int) std::round(x1);


    if (x0i != x1i) {

        float_mat curz = z0;
        float_mat dz = (z1 - z0) / (x1i - x0i);

        float_mat * cv = empty_vector();
        cv[0] = n0[0];
        cv[1] = n0[1];
        cv[2] = n0[2];

        float_mat * dv = empty_vector();

        dv[0] = (n1[0] - n0[0])/(x1i - x0i);
        dv[1] = (n1[1] - n0[1])/(x1i - x0i);
        dv[2] = (n1[2] - n0[2])/(x1i - x0i);

        for (int x = x0i; x <= x1i; x++) {

            cur_color = *total_lighting(cv, sources, ambient, cons);

            set(x, y0, curz);

            if (x != x1i) {
                add(cv, dv);
                curz += dz;
            }

            //printf("%f, %f, %f\n", cc.r, cc.g, cc.b);

        }

//        cc.r -= dc.r;
//        cc.g -= dc.g;
//        cc.b -= dc.b;

//        printf("cur: %f, %f, %f\n", cc.r, cc.g, cc.b);
//        printf("target: %f, %f, %f\n", c1.r, c1.g, c1.b);
//        printf("delta: %f, %f, %f\n", dc.r, dc.g, dc.b);

    }





    // set endpoints
    cur_color = *total_lighting(n0, sources, ambient, cons);
    set(x0, y0, z0);
    cur_color = *total_lighting(n1, sources, ambient, cons);
    set(x1, y1, z1);


}

void Drawer::scan_line_phong(TriangleMatrix *m, std::vector<double **> &sources, struct floating_color *ambient,
                             struct constants *cons, float_mat *p0, float_mat *p1, float_mat *p2) {

    // swap so they're in the right oder

    // sort by y value
    if (p1[1] < p0[1])
        std::swap(p0, p1);

    if (p2[1] < p1[1])
        std::swap(p2, p1);

    if (p1[1] < p0[1])
        std::swap(p0, p1);

    if (p0[1] == p1[1] and p0[0] > p1[0])
        std::swap(p0, p1);

    if (p1[1] == p2[1] and p1[0] > p2[0])
        std::swap(p1, p2);

    bool flip = false;

    float_mat x0 = p0[0];
    float_mat z0 = p0[2];
    float_mat x1 = x0;
    float_mat z1 = z0;
    int y = (int) std::round(p0[1]);

    float_mat * p0v = empty_vector();
    float_mat * v0t = m->get_vertex_normal(p0);
    p0v[0] = v0t[0];
    p0v[1] = v0t[1];
    p0v[2] = v0t[2];

    float_mat * p1v = empty_vector();
    float_mat * v1t = m->get_vertex_normal(p1);
    p1v[0] = v1t[0];
    p1v[1] = v1t[1];
    p1v[2] = v1t[2];

    float_mat * p2v = empty_vector();
    float_mat * v2t = m->get_vertex_normal(p2);
    p2v[0] = v2t[0];
    p2v[1] = v2t[1];
    p2v[2] = v2t[2];

    float_mat d0 = (int) std::round(p2[1]) - y;
    float_mat d1 = (int) std::round(p1[1]) - y;
    float_mat d2 = (int) std::round(p2[1]) - (int) std::round(p1[1]);

    float_mat dx0 = 0, dx1 = 0, dz0 = 0, dz1 = 0;
    float_mat * dv0 = empty_vector();
    float_mat * dv1 = empty_vector();

    if (d0 != 0) {
        dx0 = (p2[0] - p0[0]) / d0;
        dz0 = (p2[2] - p0[2]) / d0;
        dv0[0] = (p2v[0] - p0v[0])/d0;
        dv0[1] = (p2v[1] - p0v[1])/d0;
        dv0[2] = (p2v[2] - p0v[2])/d0;
    }
    if (d1 != 0) {
        dx1 = (p1[0] - p0[0]) / d1;
        dz1 = (p1[2] - p0[2]) / d1;
        dv1[0] = (p1v[0] - p0v[0])/d1;
        dv1[1] = (p1v[1] - p0v[1])/d1;
        dv1[2] = (p1v[2] - p0v[2])/d1;
    }

    float_mat cur_z;
    float_mat dz;

    float_mat * v0 = empty_vector();
    float_mat * v1 = empty_vector();
    v0[0] = v1[0] = p0v[0];
    v0[1] = v1[1] = p0v[1];
    v0[2] = v1[2] = p0v[2];

    while (y <= (int) std::round(p2[1])) {
        cur_z = z0;
        dz = (z0 - z1) / (x0 - x1);

        draw_line_vector(x0, y, z0, v0, x1, y, z1, v1, sources, ambient, cons);

        //cur_color = *total_lighting(p0v, sources, ambient, cons);

        //draw_line_simon((int) std::round(x0), y, z0, (int) std::round(x1), y, z1);
        //draw_line_wu(x0, y, z0, x1, y, z1);

        x0 += dx0;
        // color here
        z0 += dz0;
        x1 += dx1;
        // color here
        z1 += dz1;
        y += 1;

        add(v0, dv0);
        add(v1, dv1);

        if (!flip and y >= (int) std::round(p1[1])) {
            flip = true;

            dx1 = 0, dz1 = 0;

            if (d2 != 0) {
                dx1 = (p2[0] - p1[0]) / d2;

                dv1[0] = (p2v[0] - p1v[0])/d2;
                dv1[1] = (p2v[1] - p1v[1])/d2;
                dv1[2] = (p2v[2] - p1v[2])/d2;

                v1[0] = p1v[0];
                v1[1] = p1v[1];
                v1[2] = p1v[2];

                // color here

                dz1 = (p2[2] - p1[2]) / d2;
            }

            x1 = p1[0];
            z1 = p1[2];
        }

    }

}

void Drawer::draw_line_color(float_mat x0, int y0, double z0, floating_color &c0,
                             double x1, int y1, double z1, floating_color &c1) {

    assert(y0 == y1);

    if (x1 < x0) {
        return draw_line_color(x1, y1, z1, c1, x0, y0, z0, c0);
    }

//    printf("%f, %d, %f --> %f, %d, %f\n", x0, y0, z0, x1, y1, z1);
//    printf("%f, %f, %f --> %f, %f, %f\n", c0.r, c0.g, c0.b, c1.r, c1.g, c1.b);

    int x0i = (int) std::round(x0);
    int x1i = (int) std::round(x1);

    cur_color = {255, 255, 255};

    cur_color = *to_normal_color(c0);

    floating_color cc = {c0.r, c0.g, c0.b};

    floating_color dc = {
            (c1.r - c0.r) / (x1i - x0i),
            (c1.g - c0.g) / (x1i - x0i),
            (c1.b - c0.b) / (x1i - x0i)
    };

    if (x0i != x1i) {

        float_mat curz = z0;
        float_mat dz = (z1 - z0) / (x1i - x0i);

        for (int x = x0i; x <= x1i; x++) {


            cur_color = *to_normal_color(cc);

            set(x, y0, curz);

            if (x != x1i) {
                cc.r += dc.r;
                cc.g += dc.g;
                cc.b += dc.b;

                curz += dz;
            }


            //printf("%f, %f, %f\n", cc.r, cc.g, cc.b);

            assert(cc.r < 255 and cc.g < 255 and cc.b < 255);
            assert(cc.r > 0 and cc.g > 0 and cc.b > 0);
        }

//        cc.r -= dc.r;
//        cc.g -= dc.g;
//        cc.b -= dc.b;

//        printf("cur: %f, %f, %f\n", cc.r, cc.g, cc.b);
//        printf("target: %f, %f, %f\n", c1.r, c1.g, c1.b);
//        printf("delta: %f, %f, %f\n", dc.r, dc.g, dc.b);

        assert(cc.r - c1.r < 1);
    }


    // endpoints
    cur_color = *to_normal_color(c0);
    set(x0, y0, z0);
    cur_color = *to_normal_color(c1);
    set(x1, y1, z1);

}

void Drawer::scan_line_gouraund(TriangleMatrix *m, std::vector<double **> &sources, struct floating_color *ambient,
                                struct constants *cons, float_mat *p0, float_mat *p1, float_mat *p2) {

    // swap so they're in the right oder

    // sort by y value
    if (p1[1] < p0[1])
        std::swap(p0, p1);

    if (p2[1] < p1[1])
        std::swap(p2, p1);

    if (p1[1] < p0[1])
        std::swap(p0, p1);

    if (p0[1] == p1[1] and p0[0] > p1[0])
        std::swap(p0, p1);

    if (p1[1] == p2[1] and p1[0] > p2[0])
        std::swap(p1, p2);

    // calculate colors at each vertex

//    printf("(%f, %f %f)\n", m->get_vertex_normal(p0)[0],
//           m->get_vertex_normal(p0)[1],
//           m->get_vertex_normal(p0)[2]);

    floating_color p0c = *total_lighting_no_round(m->get_vertex_normal(p0), sources, ambient, cons);
    floating_color p1c = *total_lighting_no_round(m->get_vertex_normal(p1), sources, ambient, cons);
    floating_color p2c = *total_lighting_no_round(m->get_vertex_normal(p2), sources, ambient, cons);

//    printf("\ncolor 0: %f, %f, %f\n", p0c.r, p0c.g, p0c.b);
//    printf("color 1: %f, %f, %f\n", p1c.r, p1c.g, p1c.b);
//    printf("color 2: %f, %f, %f\n", p2c.r, p2c.g, p2c.b);


    // scanline stuff

    bool flip = false;

    float_mat x0 = p0[0];
    float_mat z0 = p0[2];
    float_mat x1 = x0;
    float_mat z1 = z0;
    int y = (int) std::round(p0[1]);

    // set color
    floating_color c0 = {p0c.r, p0c.g, p0c.b};
    floating_color c1 = {c0.r, c0.g, c0.b};

//    printf("c0: %f, %f, %f\n", c0.r, c0.g, c0.b);
//    printf("c1: %f, %f, %f\n", c1.r, c1.g, c1.b);


    float_mat d0 = (int) std::round(p2[1]) - y;
    float_mat d1 = (int) std::round(p1[1]) - y;
    float_mat d2 = (int) std::round(p2[1]) - (int) std::round(p1[1]);

    float_mat dx0 = 0, dx1 = 0, dz0 = 0, dz1 = 0;

    floating_color dc0 = {0, 0, 0};
    floating_color dc1 = {0, 0, 0};

    if (d0 != 0) {
        dx0 = (p2[0] - p0[0]) / d0;
        dz0 = (p2[2] - p0[2]) / d0;
        dc0 = {(p2c.r - p0c.r) / d0, (p2c.g - p0c.g) / d0, (p2c.b - p0c.b) / d0};
    }
    if (d1 != 0) {
        dx1 = (p1[0] - p0[0]) / d1;
        dz1 = (p1[2] - p0[2]) / d1;
        dc1 = {(p1c.r - p0c.r) / d1, (p1c.g - p0c.g) / d1, (p1c.b - p0c.b) / d1};

    }

//    printf("dc0: %f, %f, %f\n", dc0.r, dc0.g, dc0.b);
//    printf("dc1: %f, %f, %f\n", dc1.r, dc1.g, dc1.b);

    float_mat cur_z;
    float_mat dz;


    while (y <= (int) std::round(p2[1])) {
        cur_z = z0;
        dz = (z0 - z1) / (x0 - x1);

        //draw_line_simon((int) std::round(x0), y, z0, (int) std::round(x1), y, z1);

        //printf("%f, %d, %f --> %f, %d, %f\n", std::round(x0), y, z0, std::round(x1), y, z1);


//        if (x0 < x1) {
//            float_mat curz = z0;
//            float_mat curdz = (z1 - z0) / (std::round(x1) - std::round(x0));
//
//            floating_color curc = {c0.r, c0.g, c0.b};
//            floating_color curdc = {(c1.r - c0.r) / (std::round(x1) - std::round(x0)),
//                                    (c1.g - c0.g) / (std::round(x1) - std::round(x0)),
//                                    (c1.b - c0.b) / (std::round(x1) - std::round(x0))};
//            for (int curx = (int) std::round(x0); curx < (int) std::round(x1); curx++) {
//                cur_color = *to_normal_color(curc);
//                //printf("%d, %f\n", curx, curz);
//                //set(curx, y, curz);
//                curz += curdz;
//                curc.r += curdc.r;
//                curc.g += curdc.g;
//                curc.b += curdc.b;
//
//                //printf("curc: %f, %f, %f\n", curc.r, curc.g, curc.b);
//
//            }
//        } else {
//            float_mat curz = z1;
//            float_mat curdz = -1*((z0 - z1) / (std::round(x0) - std::round(x1)));
//
//            floating_color curc = {c1.r, c1.g, c1.b};
//            floating_color curdc = {(c0.r - c1.r) / (std::round(x0) - std::round(x1)),
//                                    (c0.g - c1.g) / (std::round(x0) - std::round(x1)),
//                                    (c0.b - c1.b) / (std::round(x0) - std::round(x1))};
//            for (int curx = (int) std::round(x1); curx < (int) std::round(x0); curx++) {
//                cur_color = *to_normal_color(curc);
//                //printf("%d, %f\n", curx, curz);
//                //set(curx, y, curz);
//                curz += curdz;
//                curc.r += curdc.r;
//                curc.g += curdc.g;
//                curc.b += curdc.b;
//
//                if(!(curc.r < 255 and curc.g < 255 and curc.b < 255)){
//                    printf("%d, %d\n", (int) std::round(x0), (int) std::round(x1));
//                   printf("%f, %f, %f\n", curc.r, curc.g, curc.b);
//                }
//                assert(curc.r < 255 and curc.g < 255 and curc.b < 255);
//            }
//
//        }

        //printf("%f\n", (z1 - z0)/(std::round(x1) - std::round(x0)));


        // set endpoints separately

//        cur_color = temp1;
//        set((int) std::round(x0), y, z0 + 10000);
//        cur_color = temp2;
//        set((int) std::round(x1), y, z1 + 10000);

        //draw_line_wu(x0, y, z0, x1, y, z1);

        draw_line_color(x0, y, z0, c0, x1, y, z1, c1);

        x0 += dx0;
        // color here
        c0.r += dc0.r;
        c0.g += dc0.g;
        c0.b += dc0.b;

        z0 += dz0;
        x1 += dx1;
        // color here
        c1.r += dc1.r;
        c1.g += dc1.g;
        c1.b += dc1.b;

        z1 += dz1;
        y += 1;

        if (!flip and y >= (int) std::round(p1[1])) {
            flip = true;

            dx1 = 0, dz1 = 0;

            dc1 = {0, 0, 0};

            if (d2 != 0) {
                dx1 = (p2[0] - p1[0]) / d2;
                // color here
                c1 = {p1c.r, p1c.g, p1c.b}; // reset delta

                dc1 = {(p2c.r - p1c.r) / d2, (p2c.g - p1c.g) / d2, (p2c.b - p1c.b) / d2};

                dz1 = (p2[2] - p1[2]) / d2;
            }

            x1 = p1[0];
            z1 = p1[2];
        }

    }

}

void Drawer::scan_line(float_mat *p0, float_mat *p1, float_mat *p2) {
    // swap so they're in the right oder

    // sort by y value
    if (p1[1] < p0[1])
        std::swap(p0, p1);

    if (p2[1] < p1[1])
        std::swap(p2, p1);

    if (p1[1] < p0[1])
        std::swap(p0, p1);

    if (p0[1] == p1[1] and p0[0] > p1[0])
        std::swap(p0, p1);

    if (p1[1] == p2[1] and p1[0] > p2[0])
        std::swap(p1, p2);

    bool flip = false;

    float_mat x0 = p0[0];
    float_mat z0 = p0[2];
    float_mat x1 = x0;
    float_mat z1 = z0;
    int y = (int) std::round(p0[1]);

    float_mat d0 = (int) std::round(p2[1]) - y;
    float_mat d1 = (int) std::round(p1[1]) - y;
    float_mat d2 = (int) std::round(p2[1]) - (int) std::round(p1[1]);

    float_mat dx0 = 0, dx1 = 0, dz0 = 0, dz1 = 0;

    if (d0 != 0) {
        dx0 = (p2[0] - p0[0]) / d0;
        dz0 = (p2[2] - p0[2]) / d0;
    }
    if (d1 != 0) {
        dx1 = (p1[0] - p0[0]) / d1;
        dz1 = (p1[2] - p0[2]) / d1;
    }

    float_mat cur_z;
    float_mat dz;

    while (y <= (int) std::round(p2[1])) {
        cur_z = z0;
        dz = (z0 - z1) / (x0 - x1);

        draw_line_simon((int) std::round(x0), y, z0, (int) std::round(x1), y, z1);
        //draw_line_wu(x0, y, z0, x1, y, z1);

        x0 += dx0;
        // color here
        z0 += dz0;
        x1 += dx1;
        // color here
        z1 += dz1;
        y += 1;

        if (!flip and y >= (int) std::round(p1[1])) {
            flip = true;

            dx1 = 0, dz1 = 0;

            if (d2 != 0) {
                dx1 = (p2[0] - p1[0]) / d2;
                // color here

                dz1 = (p2[2] - p1[2]) / d2;
            }

            x1 = p1[0];
            z1 = p1[2];
        }

    }
}

int Drawer::ipart(float_mat x) { return (int) x; }

int Drawer::round(float_mat x) { return ipart(x + 0.5); }

float_mat Drawer::fpart(float_mat x) { return x - (int) x; }

float_mat Drawer::rfpart(float_mat x) { return 1 - fpart(x); }


void Drawer::draw_line_wu(float_mat x0, float_mat y0, float_mat z0, float_mat x1, float_mat y1, float_mat z1) {
    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);

    //std::printf("(%f, %f, %f) --> (%f, %f, %f)\n", x0, y0, z0, x1, y1, z1);

    if (steep) {
        //std::printf("swap 1\n");
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        //std::printf("swap 2\n");
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    float_mat dx = x1 - x0;
    float_mat dy = y1 - y0;
    float_mat grad = dy / dx;
    if (dx == 0)
        grad = 1;

    if (y0 != y1)
        std::printf("(%f, %f, %f) --> (%f, %f, %f)\n", x0, y0, z0, x1, y1, z1);
//    std::printf("grad: %f\n", grad);

    // first endpoint
    int xend = round(x0);
    float_mat yend = y0 + grad * (xend - x0);
    float_mat xgap = rfpart(x0 + 0.5f);
    float_mat xpxl1 = xend;
    float_mat ypxl1 = ipart(yend);

    //std::printf("yend: %f, xgap: %f, xpxl1: %f, ypxl1: %f\n", yend, xgap, xpxl1, ypxl1);
    if (steep) {
        Display::set(ypxl1, xpxl1, 0, rfpart(yend) * xgap, &cur_color);
        Display::set(ypxl1 + 1, xpxl1, 0, fpart(yend) * xgap, &cur_color);
    } else {
        Display::set(xpxl1, ypxl1, 0, rfpart(yend) * xgap, &cur_color);
        Display::set(xpxl1, ypxl1 + 1, 0, fpart(yend) * xgap, &cur_color);
    }

    float_mat intery = yend + grad;

    xend = round(x1);
    yend = y1 + grad * (xend - x1);
    xgap = fpart(x1 + 0.5);
    float_mat xpxl2 = xend;
    float_mat ypxl2 = ipart(yend);

    //std::printf("yend: %f, xgap: %f, xpxl2: %f, ypxl2: %f\n", yend, xgap, xpxl2, ypxl2);

    if (steep) {
        Display::set(ypxl2, xpxl2, 0, rfpart(yend) * xgap, &cur_color);
        Display::set(ypxl2 + 1, xpxl2, 0, fpart(yend) * xgap, &cur_color);
    } else {
        Display::set(xpxl2, ypxl2, 0, rfpart(yend) * xgap, &cur_color);
        Display::set(xpxl2, ypxl2 + 1, 0, fpart(yend) * xgap, &cur_color);
    }

    if (steep) {
        for (int x = (int) xpxl1 + 1; x < xpxl2; x++) {
            Display::set(ipart(intery), x, 0, rfpart(intery), &cur_color);
            Display::set(ipart(intery) + 1, x, 0, fpart(intery), &cur_color);
            intery += grad;
            //std::printf("%f\n", intery);
        }
    } else {
        for (int x = (int) xpxl1 + 1; x < xpxl2; x++) {
            //printf("%d, %d\n ", x, ipart(intery));
            Display::set(x, ipart(intery), 0, rfpart(intery), &cur_color);
            Display::set(x, ipart(intery) + 1, 0, fpart(intery), &cur_color);
            intery += grad;
        }
    }

    // second endpoint

    //std::printf("(%f, %f, %f) --> (%f, %f, %f)\n", x0, y0, z0, x1, y1, z1);



}

// draw edges
void Drawer::draw_edges(EdgeMatrix *m) {
    int max = m->size();
    float_mat *start = m->start();

    while (max > 0) {
        draw_line_new((int) start[0], (int) start[1], start[2], (int) start[4], (int) start[5], start[6]);

        start += 8;

        max -= 1;
    }
}

void Drawer::draw_line_new(int x1, int y1, float_mat z1, int x2, int y2, float_mat z2) {
    if (x1 > x2)
        return draw_line_new(x2, y2, z2, x1, y1, z1);

    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dy >= dx) // octant 1
        draw_line_steep(x1, y1, z1, x2, y2, z2, dx, dy, 1);
    else if (dy >= 0) // octant 2
        draw_line_shallow(x1, y1, z1, x2, y2, z1, dx, dy, 1);
    else if (dx + dy > 0) // octant 8
        draw_line_shallow(x1, y1, z1, x2, y2, z1, dx, dy, -1);
    else // octant 7
        draw_line_steep(x1, y1, z1, x2, y2, z2, dx, dy, -1);
}

void
Drawer::draw_line_shallow(int x1, int y1, float_mat z1, int x2, int y2, float_mat z2, int dx, int dy, int increment) {
    int error = 2 * dy - dx, x = x1, y = y1;
    float_mat z = z1, dz = ((float_mat) (z2 - z1)) / ((float_mat) (x2 - x1));

    while (x <= x2) {
        set(x, y, z);

        if (error > 0) {
            y += increment;
            error += -2 * dx;
        }

        x += 1;
        z += dz;
        error += 2 * dy;
    }

}

void
Drawer::draw_line_steep(int x1, int y1, float_mat z1, int x2, int y2, float_mat z2, int dx, int dy, int increment) {
    int error = 2 * dx - dy, x = x1, y = y1;
    float_mat z = z1, dz = ((float_mat) (z2 - z1)) / ((float_mat) (y2 - y1));

    while (y <= y2) {
        set(x, y, z);

//        printf("(%d, %d, %f)\n", x, y, z);
//        printf("(%d, %d, %f)\n", dx, dy, dz);
//        printf("(%d, %d)\n", x1, x2);
//        printf("(%d, %d)\n", y1, y2);
//        printf("(%f, %f)\n", z1, z2);

        if (error > 0) {
            x += increment;
            error += -2 * dy;
        }

        y += 1;
        z += dz;
        error += 2 * dx;
    }
}

void Drawer::draw_line(int x1, int y1, float_mat z1, int x2, int y2, float_mat z2) {
    // make sure x1 <= x2
    if (x1 > x2) {
        draw_line(x2, y2, z1, x1, y1, z2);
        return;
    }

    // draws a line from (x1, x2) to (y1, y2)

    // get constants
    int dy = y2 - y1;
    int dx = x2 - x1;
    int a = dy;
    int b = -1 * dx;

    //printf("dy: %d, dx: %d, a: %d, b: %d\n", dy, dx, a, b);

    // figure out the octet (amazing code right there)
    char octet = -1;
    if (dy == 0) octet = 1;
    else if (dx == 0)
        if (y2 > y1) octet = 2;
        else octet = 7;
    else if (dy > 0)
        if (dy > dx) octet = 2;
        else octet = 1;
    else if (abs(dy) > dx) octet = 7;
    else octet = 8;

    assert(octet != -1); // octet should be set

    // more values
    int x = x1;
    int y = y1;
    int d = 2 * a + b;

    double z = z1;
    double dz = 0;

    // (260, 130)

    switch (octet) {
        case 1:

            dz = ((double) z2 - z1) / (x2 - x1);

            while (x <= x2) {
                set((int) std::round(x), (int) std::round(y), z);
                if (d > 0) {
                    y++;
                    d += 2 * b;
                }
                x++;
                z += dz;
                d += 2 * a;
            }
            break;
        case 2:

            dz = ((double) z2 - z1) / (y2 - y1);

            while (y <= y2) {
                set((int) std::round(x), (int) std::round(y), z);
                if (d < 0) {
                    x++;
                    d += 2 * a;
                }
                y++;
                z += dz;
                d += 2 * b;
            }
            break;
        case 8:

            dz = ((double) z2 - z1) / (x2 - x1);

            while (x <= x2) {
                set((int) std::round(x), (int) std::round(y), z);
                if (d < 0) {
                    y--;
                    d -= 2 * b;
                }
                x++;
                z += dz;
                d += 2 * a;
            }
            break;
        case 7:

            dz = ((double) z2 - z1) / (y2 - y1);

            while (y >= y2) {
                set((int) std::round(x), (int) std::round(y), z);
                if (d > 0) {
                    x++;
                    d += 2 * a;
                }
                y--;
                z += dz;
                d -= 2 * b;
            }
            break;
    }

}

void Drawer::draw_line_simon(double x0d, double y0d, double z0, double x1d, double y1d, double z1) {
    int x0 = std::round(x0d), y0 = std::round(y0d), x1 = std::round(x1d), y1 = std::round(y1d);
    int x, y, d, A, B;
    double z, dz;
    int dy_east, dy_northeast, dx_east, dx_northeast, d_east, d_northeast;
    int loop_start, loop_end;

    //swap points if going right -> left
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
        std::swap(z0, z1);
    }

    x = x0;
    y = y0;
    z = z0;
    A = 2 * (y1 - y0);
    B = -2 * (x1 - x0);
    int wide = 0;
    int tall = 0;
    //octants 1 and 8
    if (abs(x1 - x0) >= abs(y1 - y0)) { //octant 1/8
        wide = 1;
        loop_start = x;
        loop_end = x1;
        dx_east = dx_northeast = 1;
        dy_east = 0;
        d_east = A;
        if (A > 0) { //octant 1
            d = A + B / 2;
            dy_northeast = 1;
            d_northeast = A + B;
        } else { //octant 8
            d = A - B / 2;
            dy_northeast = -1;
            d_northeast = A - B;
        }
    }//end octant 1/8
    else { //octant 2/7
        tall = 1;
        dx_east = 0;
        dx_northeast = 1;
        if (A > 0) {     //octant 2
            d = A / 2 + B;
            dy_east = dy_northeast = 1;
            d_northeast = A + B;
            d_east = B;
            loop_start = y;
            loop_end = y1;
        } else {     //octant 7
            d = A / 2 - B;
            dy_east = dy_northeast = -1;
            d_northeast = A - B;
            d_east = -1 * B;
            loop_start = y1;
            loop_end = y;
        }
    }
    dz = loop_end == loop_start ? 0 : (z1 - z0) / (loop_end - loop_start);


    while (loop_start < loop_end) {

        set(x, y, z);
        if ((wide && ((A > 0 && d > 0) ||
                      (A < 0 && d < 0)))
            ||
            (tall && ((A > 0 && d < 0) ||
                      (A < 0 && d > 0)))) {
            y += dy_northeast;
            d += d_northeast;
            x += dx_northeast;
        } else {
            x += dx_east;
            y += dy_east;
            d += d_east;
        }
        z += dz;
        loop_start++;
    } //end drawing loop
    set(x, y, z);
}

void Drawer::set(float_mat x, float_mat y, float_mat z) {
    if (x < IMAGE_WIDTH and x > 0 and y < IMAGE_HEIGHT and y > 0) { // only set if value is valid
        Display::set(x, y, z, &cur_color);
    }
}

void Drawer::change_color(unsigned char r, unsigned char g, unsigned char b) {
    cur_color.r = r;
    cur_color.g = g;
    cur_color.b = b;
}

Drawer::Drawer() {
    cur_color.r = 0;
    cur_color.g = 255;
    cur_color.b = 102;

    z_buffer = new float_mat[NUM_PIXELS];

    shading_type = SHADING_FLAT;
}

Drawer::~Drawer() {

    delete z_buffer;
}

