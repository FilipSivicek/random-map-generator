#ifndef __VEC3_H__
#define __VEC3_H__

#include <cmath>
#include <ostream>

struct Vec3{
    double x, y, z;

    Vec3(double _x, double _y, double _z){
        x = _x;
        y = _y;
        z = _z;
    }

    Vec3(){
        x = 0;
        y = 0;
        z = 0;
    }

    double length(){
        return sqrt(x * x + y * y + z * z);
    }

    Vec3& normalize(){
        double l = length();
        if (l > 1e-30){
            x = x / l;
            y = y / l;
            z = z / l;
        }
        return *this;
    }

    Vec3& operator+=(const Vec3& v){
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& v){
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vec3& operator*=(const double v){
        x *= v;
        y *= v;
        z *= v;
        return *this;
    }

    void clip(double lo = 0, double hi = 1) {
        if (x < lo) x = lo;
        if (x > hi) x = hi;
        if (y < lo) y = lo;
        if (y > hi) y = hi;
        if (z < lo) z = lo;
        if (z > hi) z = hi;
    }
};

bool operator==(const Vec3& v0, const Vec3& v1){
    return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z;
}

Vec3 operator+(const Vec3& v0, const Vec3& v1){
    return Vec3{v0.x + v1.x, v0.y + v1.y, v0.z + v1.z};
}

Vec3 operator-(const Vec3& v0, const Vec3& v1){
    return Vec3{v0.x - v1.x, v0.y - v1.y, v0.z - v1.z};
}

Vec3 operator*(const double r, const Vec3& v0){
    return Vec3{v0.x * r, v0.y * r, v0.z * r};
}

double operator*(const Vec3& v0, const Vec3& v1){
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

Vec3 operator^(const Vec3& u, const Vec3& v){
    return Vec3{u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};
}

std::ostream &operator<<(std::ostream &s, const Vec3 &v){
    s << "[" << v.x << ", " << v.y << ", " << v.z << "]";
    return s;
}

Vec3 normala(Table<double>& H, int i, int j, double s = 1.0){
    Vec3 result{0, 0, 0};
    std::vector<Vec> dir{{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}};
    for (int k = 0; k < dir.size(); k++){
        result += Vec3{dir[k].x * s, dir[k].y * s, H(i + dir[k].x, j + dir[k].y)}^
                  Vec3{dir[(k + 1)%dir.size()].x * s, dir[(k + 1)%dir.size()].y * s,
                       H(i + dir[(k + 1)%dir.size()].x, j + dir[(k + 1) % dir.size()].y)};
    }

    return result.normalize();
}


#endif