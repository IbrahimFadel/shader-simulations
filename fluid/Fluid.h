#ifndef FLUID_H
#define FLUID_H

#include <cmath>

struct Fluid
{
    int size;
    float dt;
    float diff;
    float visc;

    float *s;
    float *density;

    float *Vx;
    float *Vy;

    float *Vx0;
    float *Vy0;

    Fluid(int size, int diffusion, int viscosity, float dt);
    void addDensity(int x, int y, float amount);
    void addVelocity(int x, int y, float amountX, float amountY);

    void step();
};

void linSolve(int b, float *x, float *x0, float a, float c, int iter, int N);
void setBnd(int b, float *x, int N);

void diffuse(int b, float *x, float *x0, float diff, float dt, int iter, int N);
void project(float *velocX, float *velocY, float *p, float *div, int iter, int N);
void advect(int b, float *d, float *d0, float *velocX, float *velocY, float dt, int N);

#endif