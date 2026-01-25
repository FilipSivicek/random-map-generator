#ifndef __VEC_H__
#define __VEC_H__

#include <cmath>
#include <ostream>

struct Vec{
    double x, y;

    Vec(double _x, double _y){
        x = _x;
        y = _y;
    }

    Vec(){
        x = 0;
        y = 0;
    }

    double length(){
        return sqrt(x * x + y * y);
    }

    Vec& normalize(){
        double l = length();
        if (l > 1e-30){
            x = x / l;
            y = y / l;
        }
        return *this;
    }

    Vec& operator+=(const Vec& v){
        x += v.x;
        y += v.y;
        return *this;
    }

    Vec& operator-=(const Vec& v){
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vec& operator*=(const double v){
        x *= v;
        y *= v;
        return *this;
    }

    void clip(double lo = 0, double hi = 1) {
        if (x < lo) x = lo;
        if (x > hi) x = hi;
        if (y < lo) y = lo;
        if (y > hi) y = hi;
    }
};

bool operator==(const Vec& v0, const Vec& v1){
    return v0.x == v1.x && v0.y == v1.y;
}

Vec operator+(const Vec& v0, const Vec& v1 ){
    return Vec{v0.x + v1.x, v0.y + v1.y};
}

Vec operator-(const Vec& v0, const Vec& v1){
    return Vec{v0.x - v1.x, v0.y - v1.y};
}

Vec operator*(const double r, const Vec& v0){
    return Vec{v0.x * r, v0.y * r};
}

double operator*(const Vec& v0, const Vec& v1){
    return v0.x * v1.x + v0.y * v1.y;
}

std::ostream &operator<<(std::ostream &s, const Vec &v){
    s << "[" << v.x << ", " << v.y << "]";
    return s;
}

template <typename T>
T lerp(const T& v0, const T& v1, double t){
    return (1 - t) * v0 + t * v1;
}

template <typename T>
T bilerp(const T& v00, const T& v01, const T& v10, const T& v11, double x, double y){
    T h1 = lerp(v00, v01, x);
    T h2 = lerp(v10, v11, x);
    return lerp(h1, h2, y);
}

#endif