#ifndef __PERLIN_H__
#define __PERLIN_H__

#include <cmath>
#include <random>

#include "tabulka.h"
#include "vec.h"

using namespace std;

mt19937 rnd(random_device{}());
uniform_real_distribution<> dis(0.0, 1.0);

struct Perlin{
    int n, d, m;
    Table<double> H;
    Table<Vec> G;

    Perlin(int _n, int _d): n{_n}, d{_d}, m{1 + n/d},
                            H(n, n), G(m, m){
        for (int i = 0; i < m; i++){
            for (int j = 0; j < m; j++){
                double theta = dis(rnd) * M_PI * 2.0;
                G(i, j) = {cos(theta), sin(theta)};
            }
        }
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                Vec p {(double) i / (double) d, (double) j / (double) d};
                int o[2] = {(int) p.x, (int) p.y};

                double v[4];
                for (int dy = 0; dy < 2; dy++){
                    for (int dx = 0; dx < 2; dx++){
                        v[2 * dy + dx] = G(o[0] + dx, o[1] + dy) * (p - Vec{(double)o[0] + dx, (double)o[1] + dy});
                    }
                }
                
                double r = fade(p.x - o[0]), s = fade(p.y - o[1]);
                double vc = lerp(v[0], v[1], r), vd = lerp(v[2], v[3], r);
                H(i, j) = lerp(vc, vd, s);
            }
        }
    }

    private:
        double fade(double t){
            return t * t * t * (10 + t * (6 * t - 15));
        }   
};

#endif