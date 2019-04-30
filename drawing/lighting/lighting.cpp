//
// Created by Ivan Galakhov on 2019-04-29.
//

#include "lighting.h"

color *total_lighting(float_mat *normal){
    float r = 0;
    float g = 0;
    float b = 0;
    floating_color * cur;
    for(int i = 0; i < 1; i++){
        cur = single_lighting(normal, light[i]);
        r += cur->r;
        g += cur->g;
        b += cur->b;
    }
    auto ret = (color *) malloc(sizeof(color));
    ret->r = (unsigned char) constrain(r, 0, 255);
    ret->g = (unsigned char) constrain(g, 0, 255);
    ret->b = (unsigned char) constrain(b, 0, 255);
    return ret;
}

floating_color *single_lighting(float_mat *normal, float_mat light[2][3]) {

    auto ret = (floating_color *) std::malloc(sizeof(floating_color)); // color we're returning


    floating_color *ambient = calculate_ambient();
    floating_color *diffuse = calculate_diffuse(normal, light);
    floating_color *specular = calculate_specular(normal, light); // calculate this last, as it modifies the normal


    // set colors
    ret->r = std::round(ambient->r + specular->r + diffuse->r);
    ret->g =  std::round(ambient->g + specular->g + diffuse->g);
    ret->b = std::round(ambient->b + specular->b + diffuse->b);

//    ret->r = (unsigned char) constrain(std::round(ambient->r), 0, 255);
//    ret->g = (unsigned char) constrain(std::round(ambient->g), 0, 255);
//    ret->b = (unsigned char) constrain(std::round(ambient->b), 0, 255);

//    ret->r = (unsigned char) constrain(std::round(ambient->r), 0, 255);
//    ret->g = (unsigned char) constrain(std::round(ambient->g), 0, 255);
//    ret->b = (unsigned char) constrain(std::round(ambient->b), 0, 255);

    // delete stuff we don't need
    delete ambient;
    delete diffuse;
    delete specular;

    return ret;
}

floating_color *calculate_diffuse(float_mat *normal, float_mat light[2][3]) {
    auto ret = (floating_color *) std::malloc(sizeof(floating_color));

    ret->r = light[1][0];
    ret->g = light[1][1];
    ret->b = light[1][2];

    float_mat dot = dot_product(normal, light[0]);
    //printf("%f\n", dot);
    multiply_constant(ret, dot);
    multiply_through(ret, diffuse_reflect);
    fix_color(ret);

    //printf("%f, %f, %f\n", ret->r, ret->g, ret->b);

    return ret;
}

floating_color *calculate_specular(float_mat *normal, float_mat light[2][3]) {
    auto ret = (floating_color *) std::malloc(sizeof(floating_color));

    ret->r = light[1][0];
    ret->g = light[1][1];
    ret->b = light[1][2];

    float_mat first_dot = dot_product(normal, light[0]);

    if (first_dot < 0) {
        ret->r = ret->g = ret->b = 0;
        return ret;
    }

    scalar_mult(normal, first_dot * 2);
    subtract(normal, light[0]);
    float_mat second_dot = dot_product(normal, view);
    multiply_through(ret, specular_reflect);
    multiply_constant(ret, std::pow(second_dot, SPECULAR_EXPONENT));
    fix_color(ret);


    return ret;
}

floating_color *calculate_ambient() {
    auto ret = (floating_color *) std::malloc(sizeof(floating_color));

    copy_color(ret, ambient);
    multiply_through(ret, ambient_reflect);
    fix_color(ret);

    return ret;

}

// utils
void copy_color(floating_color *to, float_mat *from) {
    to->r = from[0];
    to->g = from[1];
    to->b = from[2];
}

void multiply_through(floating_color *c, float_mat *m) {
    c->r = c->r * m[0];
    c->g = c->g * m[0];
    c->b = c->b * m[0];
};

void fix_color(floating_color *c) {
    c->r = constrain(c->r, 0, 255);
    c->g = constrain(c->g, 0, 255);
    c->b = constrain(c->b, 0, 255);
}

void multiply_constant(floating_color *c, float_mat val) {
    c->r *= val;
    c->g *= val;
    c->b *= val;
}

float_mat constrain(float_mat val, float_mat lo, float_mat hi) {
    if (val < lo)
        return lo;
    if (val > hi)
        return hi;
    return val;
}
