//
// Created by Ivan Galakhov on 2019-02-06.
//

#include "drawer.h"


void Drawer::draw_points(PointMatrix * m){
    int n = m->size();
    float_mat * s = m->start();

    while(n > 0){
        set((int) s[0], (int) s[1], (int) s[2]);

        s += 4;

        n -= 1;
    }
}

// draw polygons with ambient and constants
void Drawer::draw_polygons(TriangleMatrix * m, std::vector<double **> &sources, struct floating_color * ambient, struct constants * cons){
    int n = m->size();
    float_mat * s = m->start();

    while(n > 0){

        bool draw = true;
        float_mat * N;

        if(ENABLE_BACKFACE_CULLING){

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


            if(dot_product(N, view) <= 0)
                draw = false;
        }

        if(draw) {

            // random colors because no lighting or z buffering

            cur_color = *total_lighting(N, sources, ambient, cons);

//            cur_color.r = (unsigned char)(std::rand() % 255);
//            cur_color.g = (unsigned char)(std::rand() % 255);
//            cur_color.b = (unsigned char)(std::rand() % 255);

            scan_line(s + 0, s + 4, s + 8);

//            draw_line_wu(s[0], s[1], s[2],  s[4],  s[5], s[6]);
//            draw_line_wu( s[4],  s[5], s[6],  s[8],  s[9], s[10]);
//            draw_line_wu( s[0],  s[1], s[2],  s[8],  s[9], s[10]);

        }

        s += 12;

        n -= 1;
    }
}

void Drawer::scan_line(float_mat * p0, float_mat * p1, float_mat * p2) {
    // swap so they're in the right oder

    // sort by y value
    if(p1[1] < p0[1])
        std::swap(p0, p1);

    if(p2[1] < p1[1])
        std::swap(p2, p1);

    if(p1[1] < p0[1])
        std::swap(p0, p1);

    if(p0[1] == p1[1] and p0[0] > p1[0])
        std::swap(p0, p1);

    if(p1[1] == p2[1] and p1[0] > p2[0])
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

    if(d0 != 0) {
        dx0 = (p2[0] - p0[0]) / d0;
        dz0 = (p2[2] - p0[2]) / d0;
    }
    if(d1 != 0) {
        dx1 = (p1[0] - p0[0])/d1;
        dz1 = (p1[2] - p0[2])/d1;
    }

    float_mat cur_z;
    float_mat dz;

    while(y <= (int) std::round(p2[1])){
        cur_z = z0;
        dz = (z0 - z1)/(x0 - x1);

        //draw_line_simon((int) std::round(x0), y, z0, (int) std::round(x1), y, z1);
        draw_line_wu(x0, y, z0, x1, y, z1);

        x0 += dx0;
        z0 += dz0;
        x1 += dx1;
        z1 += dz1;
        y += 1;

        if(!flip and y >= (int) std::round(p1[1])) {
            flip = true;

            dx1 = 0, dz1 = 0;

            if(d2 != 0) {
                dx1 = (p2[0] - p1[0]) / d2;
                dz1 = (p2[2] - p1[2]) / d2;
            }

            x1 = p1[0];
            z1 = p1[2];
        }

    }
}

int Drawer::ipart(float_mat x) {return (int) x;}
int Drawer::round(float_mat x) {return ipart(x + 0.5);}
float_mat Drawer::fpart(float_mat x) {return x - (int) x;}
float_mat Drawer::rfpart(float_mat x) {return 1 - fpart(x);}


void Drawer::draw_line_wu(float_mat x0, float_mat y0, float_mat z0, float_mat x1, float_mat y1, float_mat z1){
    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);

    //std::printf("(%f, %f, %f) --> (%f, %f, %f)\n", x0, y0, z0, x1, y1, z1);

    if(steep){
        //std::printf("swap 1\n");
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if(x0 > x1){
        //std::printf("swap 2\n");
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    float_mat dx = x1 - x0;
    float_mat dy = y1 - y0;
    float_mat grad = dy/dx;
    if(dx == 0)
        grad = 1;

    if(y0 != y1)
        std::printf("(%f, %f, %f) --> (%f, %f, %f)\n", x0, y0, z0, x1, y1, z1);
//    std::printf("grad: %f\n", grad);

    // first endpoint
    int xend = round(x0);
    float_mat yend = y0 + grad*(xend - x0);
    float_mat xgap = rfpart(x0 + 0.5f);
    float_mat xpxl1 = xend;
    float_mat ypxl1 = ipart(yend);

    //std::printf("yend: %f, xgap: %f, xpxl1: %f, ypxl1: %f\n", yend, xgap, xpxl1, ypxl1);
    if(steep){
        Display::set(ypxl1, xpxl1, 0, rfpart(yend)*xgap, &cur_color);
        Display::set(ypxl1 + 1, xpxl1, 0, fpart(yend)*xgap, &cur_color);
    } else {
        Display::set(xpxl1, ypxl1, 0, rfpart(yend)*xgap, &cur_color);
        Display::set(xpxl1, ypxl1 + 1, 0, fpart(yend)*xgap, &cur_color);
    }

    float_mat intery = yend + grad;

    xend = round(x1);
    yend = y1 + grad * (xend - x1);
    xgap = fpart(x1 + 0.5);
    float_mat xpxl2 = xend;
    float_mat ypxl2 = ipart(yend);

    //std::printf("yend: %f, xgap: %f, xpxl2: %f, ypxl2: %f\n", yend, xgap, xpxl2, ypxl2);

    if(steep) {
        Display::set(ypxl2, xpxl2, 0, rfpart(yend) * xgap, &cur_color);
        Display::set(ypxl2 + 1, xpxl2, 0, fpart(yend) * xgap, &cur_color);
    } else {
        Display::set(xpxl2, ypxl2, 0, rfpart(yend) * xgap, &cur_color);
        Display::set(xpxl2, ypxl2 + 1, 0, fpart(yend) * xgap, &cur_color);
    }

    if(steep){
        for(int x = (int) xpxl1 + 1; x < xpxl2; x++){
            Display::set(ipart(intery), x, 0, rfpart(intery), &cur_color);
            Display::set(ipart(intery)+1, x, 0, fpart(intery), &cur_color);
            intery += grad;
            //std::printf("%f\n", intery);
        }
    } else {
        for(int x = (int) xpxl1 + 1; x < xpxl2; x++){
            //printf("%d, %d\n ", x, ipart(intery));
            Display::set(x, ipart(intery), 0, rfpart(intery), &cur_color);
            Display::set(x, ipart(intery)+1, 0, fpart(intery), &cur_color);
            intery += grad;
        }
    }

    // second endpoint

    //std::printf("(%f, %f, %f) --> (%f, %f, %f)\n", x0, y0, z0, x1, y1, z1);



}

// draw edges
void Drawer::draw_edges(EdgeMatrix * m){
    int max = m->size();
    float_mat * start = m->start();

    while(max > 0){
        draw_line_new((int) start[0], (int) start[1], start[2], (int) start[4], (int) start[5], start[6]);

        start += 8;

        max -= 1;
    }
}

void Drawer::draw_line_new(int x1, int y1, float_mat z1, int x2, int y2, float_mat z2){
    if(x1 > x2)
        return draw_line_new(x2, y2, z2, x1, y1, z1);

    int dx = x2 - x1;
    int dy = y2 - y1;

    if(dy >= dx) // octant 1
        draw_line_steep(x1, y1, z1, x2, y2, z2, dx, dy, 1);
    else if(dy >= 0) // octant 2
        draw_line_shallow(x1, y1, z1, x2, y2, z1, dx, dy, 1);
    else if (dx + dy > 0) // octant 8
        draw_line_shallow(x1, y1, z1, x2, y2, z1, dx, dy, -1);
    else // octant 7
        draw_line_steep(x1, y1, z1, x2, y2, z2, dx, dy, -1);
}

void Drawer::draw_line_shallow(int x1, int y1, float_mat z1, int x2, int y2, float_mat z2, int dx, int dy, int increment) {
    int error = 2*dy - dx, x = x1, y = y1;
    float_mat z = z1, dz = ((float_mat) (z2 - z1))/((float_mat) (x2 - x1));

    while(x <= x2){
        set(x, y, z);

        if(error > 0) {
            y += increment;
            error += -2 * dx;
        }

        x += 1;
        z += dz;
        error += 2*dy;
    }

}

void Drawer::draw_line_steep(int x1, int y1, float_mat z1, int x2, int y2, float_mat z2, int dx, int dy, int increment){
    int error = 2*dx - dy, x = x1, y = y1;
    float_mat z = z1, dz = ((float_mat) (z2 - z1))/((float_mat) (y2 - y1));

    while(y <= y2){
        set(x, y, z);

//        printf("(%d, %d, %f)\n", x, y, z);
//        printf("(%d, %d, %f)\n", dx, dy, dz);
//        printf("(%d, %d)\n", x1, x2);
//        printf("(%d, %d)\n", y1, y2);
//        printf("(%f, %f)\n", z1, z2);

        if(error > 0){
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
    if (x1 > x2){
        draw_line(x2, y2, z1, x1, y1, z2);
        return;
    }

    // draws a line from (x1, x2) to (y1, y2)

    // get constants
    int dy = y2 - y1;
    int dx = x2 - x1;
    int a = dy;
    int b = -1*dx;

    //printf("dy: %d, dx: %d, a: %d, b: %d\n", dy, dx, a, b);

    // figure out the octet (amazing code right there)
    char octet = -1;
    if(dy == 0) octet = 1;
    else if(dx == 0)
        if(y2 > y1) octet = 2;
        else octet = 7;
    else if(dy > 0)
        if(dy > dx) octet = 2;
        else octet = 1;
    else
        if(abs(dy) > dx) octet = 7;
        else octet = 8;

    assert(octet != -1); // octet should be set

    // more values
    int x = x1;
    int y = y1;
    int d = 2*a + b;

    double z = z1;
    double dz = 0;

    // (260, 130)

    switch(octet){
        case 1:

            dz = ((double) z2 - z1)/(x2 - x1);

            while(x <= x2){
                set((int) std::round(x), (int) std::round(y), z);
                if(d > 0){
                    y++;
                    d += 2*b;
                }
                x++;
                z += dz;
                d += 2*a;
            }
            break;
        case 2:

            dz = ((double) z2 - z1)/(y2 - y1);

            while(y <= y2){
                set((int) std::round(x), (int) std::round(y), z);
                if(d < 0){
                    x++;
                    d += 2*a;
                }
                y++;
                z += dz;
                d += 2*b;
            }
            break;
        case 8:

            dz = ((double) z2 - z1)/(x2 - x1);

            while(x <= x2){
                set((int) std::round(x), (int) std::round(y), z);
                if(d < 0){
                    y--;
                    d -= 2*b;
                }
                x++;
                z += dz;
                d += 2*a;
            }
            break;
        case 7:

            dz = ((double) z2 - z1)/(y2 - y1);

            while(y >= y2){
                set((int) std::round(x), (int) std::round(y), z);
                if(d > 0){
                    x++;
                    d += 2*a;
                }
                y--;
                z += dz;
                d -= 2*b;
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
    if(x < IMAGE_WIDTH and x > 0 and y < IMAGE_HEIGHT and y > 0){ // only set if value is valid
        Display::set(x, y, z, &cur_color);
    }
}

void Drawer::change_color(unsigned char r, unsigned char g, unsigned char b){
    cur_color.r = r;
    cur_color.g = g;
    cur_color.b = b;
}

Drawer::Drawer() {
    cur_color.r = 0;
    cur_color.g = 255;
    cur_color.b = 102;

    z_buffer = new float_mat[NUM_PIXELS];
}

Drawer::~Drawer() {

    delete z_buffer;
}

