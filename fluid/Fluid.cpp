#include "Fluid.h"

#define IDX(x, y) x + y *N

#include <stdio.h>

Fluid::Fluid(int N, int diffusion, int viscosity, float _dt)
{
    size = N;
    dt = _dt;
    diff = diffusion;
    visc = viscosity;

    s = new float[N * N]{0};
    density = new float[N * N]{0};

    Vx = new float[N * N]{0};
    Vy = new float[N * N]{0};

    Vx0 = new float[N * N]{0};
    Vy0 = new float[N * N]{0};
}

void Fluid::addDensity(int x, int y, float amount)
{
    int N = size;
    // density[IDX(x, y)] += amount;
    density[0] += amount;
    density[1] += amount;
    density[2] += amount;
    density[3] += amount;
}

void Fluid::addVelocity(int x, int y, float amountX, float amountY)
{
    int N = size;
    int index = IDX(x, y);

    Vx[index] += amountX;
    Vy[index] += amountY;
}

void Fluid::step()
{
    int N = size;

    printf("diffuse1\n");
    diffuse(1, Vx0, Vx, visc, dt, 4, N);
    printf("diffuse2\n");
    diffuse(2, Vy0, Vy, visc, dt, 4, N);

    printf("project1\n");
    project(Vx0, Vy0, Vx, Vy, 4, N);

    printf("advect1\n");
    advect(1, Vx, Vx0, Vx0, Vy0, dt, N);
    printf("advect2\n");
    advect(2, Vy, Vy0, Vx0, Vy0, dt, N);

    printf("project2\n");
    project(Vx, Vy, Vx0, Vy0, 4, N);

    printf("diffuse3\n");
    diffuse(0, s, density, diff, dt, 4, N);
    printf("advect3\n");
    advect(0, density, s, Vx, Vy, dt, N);
    printf("DONE\n");
}

void diffuse(int b, float *x, float *x0, float diff, float dt, int iter, int N)
{
    float a = dt * diff * (N - 2) * (N - 2);
    linSolve(b, x, x0, a, 1 + 6 * a, iter, N);
}

void project(float *velocX, float *velocY, float *p, float *div, int iter, int N)
{
    for (int j = 1; j < N - 1; j++)
    {
        for (int i = 1; i < N - 1; i++)
        {
            div[IDX(i, j)] = -0.5f * (velocX[IDX(i + 1, j)] - velocX[IDX(i - 1, j)] + velocY[IDX(i, j + 1)] - velocY[IDX(i, j - 1)]) / N;
            p[IDX(i, j)] = 0;
        }
    }
    setBnd(0, div, N);
    setBnd(0, p, N);
    linSolve(0, p, div, 1, 6, iter, N);

    for (int j = 1; j < N - 1; j++)
    {
        for (int i = 1; i < N - 1; i++)
        {
            velocX[IDX(i, j)] -= 0.5f * (p[IDX(i + 1, j)] - p[IDX(i - 1, j)]) * N;
            velocY[IDX(i, j)] -= 0.5f * (p[IDX(i, j + 1)] - p[IDX(i, j - 1)]) * N;
        }
    }
    setBnd(1, velocX, N);
    setBnd(2, velocY, N);
}

void linSolve(int b, float *x, float *x0, float a, float c, int iter, int N)
{
    float cRecip = 1.0 / c;
    for (int k = 0; k < iter; k++)
    {
        for (int j = 1; j < N - 1; j++)
        {
            for (int i = 1; i < N - 1; i++)
            {
                x[IDX(i, j)] =
                    (x0[IDX(i, j)] + a * (x[IDX(i + 1, j)] + x[IDX(i - 1, j)] + x[IDX(i, j + 1)] + x[IDX(i, j - 1)])) * cRecip;
            }
        }
        setBnd(b, x, N);
    }
}

void setBnd(int b, float *x, int N)
{
    for (int i = 1; i < N - 1; i++)
    {
        x[IDX(i, 0)] = b == 2 ? -x[IDX(i, 1)] : x[IDX(i, 1)];
        x[IDX(i, N - 1)] = b == 2 ? -x[IDX(i, N - 2)] : x[IDX(i, N - 2)];
    }
    for (int j = 1; j < N - 1; j++)
    {
        x[IDX(0, j)] = b == 1 ? -x[IDX(1, j)] : x[IDX(1, j)];
        x[IDX(N - 1, j)] = b == 1 ? -x[IDX(N - 2, j)] : x[IDX(N - 2, j)];
    }

    x[IDX(0, 0)] = 0.5f * (x[IDX(1, 0)] + x[IDX(0, 1)]);
    x[IDX(0, N - 1)] = 0.5f * (x[IDX(1, N - 1)] + x[IDX(0, N - 2)]);
    x[IDX(N - 1, 0)] = 0.5f * (x[IDX(N - 2, 0)] + x[IDX(N - 1, 1)]);
    x[IDX(N - 1, N - 1)] = 0.5f * (x[IDX(N - 2, N - 1)] + x[IDX(N - 1, N - 2)]);
}

void advect(int b, float *d, float *d0, float *velocX, float *velocY, float dt, int N)
{
    float i0, i1, j0, j1;

    float dtx = dt * (N - 2);
    float dty = dt * (N - 2);

    float s0, s1, t0, t1;
    float tmp1, tmp2, x, y;

    float Nfloat = N;
    float ifloat, jfloat;
    int i, j;

    for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++)
    {
        for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++)
        {
            tmp1 = dtx * velocX[IDX(i, j)];
            tmp2 = dty * velocY[IDX(i, j)];
            x = ifloat - tmp1;
            y = jfloat - tmp2;

            if (x < 0.5f)
                x = 0.5f;
            if (x > Nfloat + 0.5f)
                x = Nfloat + 0.5f;
            i0 = floorf(x);
            i1 = i0 + 1.0f;
            if (y < 0.5f)
                y = 0.5f;
            if (y > Nfloat + 0.5f)
                y = Nfloat + 0.5f;
            j0 = floorf(y);
            j1 = j0 + 1.0f;

            s1 = x - i0;
            s0 = 1.0f - s1;
            t1 = y - j0;
            t0 = 1.0f - t1;

            int i0i = i0;
            int i1i = i1;
            int j0i = j0;
            int j1i = j1;

            d[IDX(i, j)] =
                s0 * (t0 * d0[IDX(i0i, j0i)]) + t1 * d0[IDX(i0i, j1i)] +
                s1 * (t0 * d0[IDX(i1i, j0i)]) + t1 * d0[IDX(i1i, j1i)];
        }
    }

    setBnd(b, d, N);
}