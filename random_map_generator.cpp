#include <algorithm>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include "obrazok.h"
#include "perlin.h"
#include "vec3.h"
#include "gradient.h"
#include "tabulka.h"
#include "eroder.h"

using namespace std;

uniform_real_distribution<> DIS_MID(0.3, 0.7);
uniform_real_distribution<> DIS_RAINDROP(0.8, 1.25);

void zapis(Table<int>& T, string fname) {
    vector<unsigned char> data(T.m * T.n * 4);
    for (int i = 0; i < T.m; i++)
        for (int j = 0; j < T.n; j++) {
            int offs = 4 * ((T.n - j - 1) * T.n + i);
            for (int k = 0; k < 3; k++)
                data[offs++] = min(255 * max(T(i, j), 0), 255);
            data[offs++] = 255;
        }

    zapis_rgba_png(T.n, T.m, data.data(), fname);
}

void zapis(Table<Vec3> &T, string fname){
    vector<unsigned char> data(T.m * T.n * 4);
    for (int i = 0; i < T.m; i++){
        for (int j = 0; j < T.n; j++){
            int offs = 4 * ((T.n - j - 1) * T.n + i);
            data[offs++] = min(255 * T(i, j).x, 255.0);
            data[offs++] = min(255 * T(i, j).y, 255.0);
            data[offs++] = min(255 * T(i, j).z, 255.0);
            data[offs++] = 255;
        }
    }
    zapis_rgba_png(T.n, T.m, data.data(), fname);
}

int main(int argc, char** argv) {
    string fname = "island.png";
    int n = 1024;
    double coeff = 0.7;

    if (argc > 1) {
        stringstream ss(argv[1]);
        ss >> fname;
    }

    if (argc > 2){
        stringstream ss(argv[2]);
        ss >> n;
    }

    if (argc > 3){
        stringstream ss(argv[3]);
        ss >> coeff;
    }

    if (argc > 4){
        stringstream ss(argv[3]);
        unsigned long seed;
        ss >> seed;
        rnd.seed(seed);
    }

    Table<double> H(n, n);
    for (int i = 0; i < H.m; i++)
        for (int j = 0; j < H.n; j++) H(i, j) = 0;
            for (int t = 1; t < n; t *= 2){
                H *= coeff;
                H += Perlin(n, t).H;  
            }

    int mid_x = DIS_MID(rnd) * n;
    int mid_y = DIS_MID(rnd) * n;
    Vec max_length{(double) max(mid_x, n - mid_x), (double)max(mid_y, n - mid_y)};

    for (int i = 0; i < H.m; i++){
        for (int j = 0; j < H.n; j++){
            H(i, j) -= sqrt((i - mid_x) * (i - mid_x) + (j - mid_y) * (j - mid_y)) / (0.5 * max_length.length());
        }
    }


    double wl = -0.24;
    double minl = 1e50, maxl = -1e50;
    for (int i = 0; i < H.m; i++){
        for (int j = 0; j < H.n; j++){
            if (H(i, j) < minl){
                minl = H(i, j);
            }
            if (H(i, j) > maxl){
                maxl = H(i, j);
            }
        }
    }

    Eroder erod{H};
    double s = 0.004;
    for (int i = 0; i < 1000; i++){
        Vec drop{DIS_RAINDROP(rnd) * mid_x, DIS_RAINDROP(rnd) * mid_y};
        erod.kvap(drop, s);
    }

    for (int i = 0; i < H.m; i++){
        for (int j = 0; j < H.n; j++){
            H(i, j) = erod.H(i, j);
        }
    }

    Gradient gWater("bath_112.gpf");
    Gradient gLand("europe_3.gpf");
    Table<Vec3> F(n, n);
    for (int i = 0; i < H.m; i++){
        for (int j = 0; j < H.n; j++){
            if (H(i, j) > wl){
                F(i, j) = gLand((H(i, j) - wl) / (maxl - wl));
            }
            else {
                F(i, j) = gWater((H(i, j) - minl) / (wl - minl));
            }
        }
    }

    Vec3 light{-1, 1, 12};
    light.normalize();

    Table<double> N(n, n);
    for (int i = 0; i < H.m; i++){
        for (int j = 0; j < H.n; j++){
            if (H(i, j) <= wl){
                N(i, j) = 1;
            }
            else {
                N(i, j) = light * normala(H, i, j, s);
            }
        }
    }

    for (int i = 0; i < F.m; i++){
        for (int j = 0; j < F.n; j++){
            F(i, j) *= (0.6 + 0.4 * max(0.0, N(i, j)));
        }
    }
    zapis(F, fname);
    zapis(erod.W, "water.png");
}