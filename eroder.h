#ifndef __ERODER_H__
#define __ERODER_H_

#include <algorithm>
#include <iostream>
#include <ostream>

#include "tabulka.h"
#include "vec.h"
#include "vec3.h"

struct Eroder{
    Table<double> H;
    Table<int> W;
    double inertia = 0.1;
    double solubility = 15;
    double sedimentation = 0.5;
    double erosion = 0.1; 
    double gravity = 5;
    double evaporation = -1.5;
    double min_incline = 0.1;
    static const int r = 2;
    double weights[2 * r + 1][2 * r + 1];
    ofstream MyFile;
    
    Eroder(Table<double> _H): H(_H.m, _H.n), W(_H.m, _H.n){
        MyFile = ofstream("log.txt");
        for (int i = 0; i < H.m; i++){
            for (int j = 0; j < H.n; j++){
                H(i, j) = _H(i, j);
                W(i, j) = 0;
            }
        }
        double sum = 0;
        for (int dy = 0; dy < 2 * r + 1; dy++)
            for (int dx = 0; dx < 2 * r + 1; dx++){
                double dist = sqrt((r - dx) * (r - dx) + (r - dy) * (r - dy)) / (r + 1);
                double w = 0;
                if (dist < 1) w = (1 - dist);
                weights[dx][dy] = w;
                sum += w;
            }

        //MyFile << "sum: " << sum << "\n";
        
        for (int dy = 0; dy < 2 * r + 1; dy++)
            for (int dx = 0; dx < 2 * r + 1; dx++) weights[dx][dy] /= sum;
        
    }

    void kvap(Vec& pos, double s){
        int lifetime = 500;
        Vec dir{0, 0};
        double water = 1;
        double mud = 0;
        double speed = 5;

        for (int t = 0; t < lifetime; t++){
            int o[2] = {(int) pos.x, (int) pos.y};

            // MyFile << "o: " << o[0] << " " << o[1] << "\n";

            W(o[0], o[1]) = 1; 
            double old_height = bilerp(H(o[0], o[1]), H(o[0] + 1, o[1]), 
                                       H(o[0], o[1] + 1), H(o[0] + 1, o[1] + 1), 
                                       pos.x - o[0], pos.y - o[1]);

            // MyFile <<"H: " << H(o[0], o[1]) << " " << H(o[0] + 1, o[1]) << " " << H(o[0], o[1] + 1) << " " << H(o[0] + 1, o[1] + 1) << "\n";
            Vec3 g = bilerp(normala(H, o[0], o[1], s), 
                            normala(H, o[0] + 1, o[1], s),
                            normala(H, o[0], o[1] + 1, s), 
                            normala(H, o[0] + 1, o[1] + 1, s), 
                            pos.x - o[0], pos.y - o[1]);
            
            // MyFile << "g: " << g << "\n";

            Vec movement{g.x, g.y};
            movement.normalize();

            dir = lerp(movement, dir, inertia);
            if (dir.length() < 1e-25) {
                // MyFile << "broken\n";
                break;
            }
            dir.normalize();

            // MyFile << "dir: " << dir << "\n";

            pos += dir;
            o[0] = (int) pos.x;
            o[1] = (int) pos.y;

            double new_height = bilerp(H(o[0], o[1]), H(o[0] + 1, o[1]), 
                                       H(o[0], o[1] + 1), H(o[0] + 1, o[1] + 1), 
                                       pos.x - o[0], pos.y - o[1]);
            
            double delta = old_height - new_height;
            double can_sediment = std::max(-delta, min_incline) * 
                                  speed * water * solubility;

            double sediment = 0;
            if (delta > 0){
                sediment = std::min(delta, mud);
            } else if (mud > can_sediment) {
                sediment = (mud - can_sediment) * sedimentation;
            }

            mud -= sediment;

            int ix = pos.x, iy = pos.y;
            Vec p{pos.x - ix, pos.y - iy};
                
            // MyFile << "sediment: " << sediment << "\n";
            // MyFile << "p.x: " << p.x << "\n";
            // MyFile << "p.y: " << p.y << "\n";

            for (int dy = 0; dy < 2; dy++)
                for (int dx = 0; dx < 2; dx++){
                    H(ix + dx, iy + dy) += sediment * (dx * p.x + (1 - dx) * (1 - p.x)) *
                                                      (dy * p.y + (1 - dy) * (1 - p.y));
                    // MyFile << sediment * (dx * p.x + (1 - dx) * (1 - p.x)) * (dy * p.y + (1 - dy) * (1 - p.y)) << "\n";
                }
            
            if (sediment == 0){
                double erode = std::min((can_sediment - mud) * erosion, -delta);
                // MyFile << "erode: " << erode << "\n";
                for (int dy = 0; dy < 2 * r + 1; dy++)
                    for (int dx = 0; dx < 2 * r + 1; dx++){
                        int i = ix - r + dx, j = iy - r + dy;
                        double piece = erode * weights[dx][dy];
                        // MyFile << "weights: " << weights[dx][dy] << "\n";
                        // MyFile << "piece: " << piece << "\n";
                        H(i, j) -= piece;
                        mud += piece;
                    }
            }

            if (delta < 0) delta *= -1;
            speed = sqrt(speed * speed + delta * gravity);
            water *= (1 - evaporation);

            // MyFile <<"H: " << H(o[0], o[1]) << " " << H(o[0] + 1, o[1]) << " " << H(o[0], o[1] + 1) << " " << H(o[0] + 1, o[1] + 1) << "\n";
            
        }
    }
};

#endif