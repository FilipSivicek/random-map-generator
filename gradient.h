#ifndef __GRADIENT_H__
#define __GRADIENT_H__

#include <cmath>
#include <vector>
#include <fstream>

#include "vec3.h"

struct Stop {
    double t;
    Vec3 rgb;
};

struct Gradient {
  std::vector<Stop> stops;

  Gradient(const std::vector<Stop>& _stops = std::vector<Stop>{}) {
    stops = _stops;
  }

  Gradient(std::string fname) {
    using namespace std;
    ifstream f(fname);
    string line;
    while (getline(f, line)) {
      if (line[0] == '#') continue;
      stringstream ss(line);
      Stop s;
      ss >> s.t;
      double r, g, b;
      ss >> r >> g >> b;
      s.rgb = Vec3{r, g, b};
      stops.push_back(s);
    }
  }

  Vec3 operator()(double t) const {
    using namespace std;
    t = fmod(t, 1);
    auto it = lower_bound(stops.begin(), stops.end(), Stop{t, {}},
                          [](Stop a, Stop b) { return a.t < b.t; });

    if (it == stops.begin()) return stops[0].rgb;
    Vec3 a = (it)->rgb, b = (it - 1)->rgb;
    t = (t - (it - 1)->t) / (it->t - (it - 1)->t);

    return lerp(b, a, t);
  }
};

#endif