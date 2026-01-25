#ifndef __TABULKA_H__
#define __TABULKA_H__

#include <ostream>

template <typename T>
struct Table{
    int m, n;
    T *data;
    T sentinel = T();

    Table(int _m, int _n){
        m = _m;
        n = _n;
        data = new T[m * n];  
    }

    T& operator()(int r, int s){
        if (r >= 0 && r < m && s >= 0 && s < n){
            return data[r * n + s];
        }
        return sentinel;
    }

    T operator()(int r, int s) const {
        if (r >= 0 && r < m && s >= 0 && s < n){
            return data[r * n + s];
        }
        else{
            return sentinel;
        }
    }

    Table& operator*=(const T& r){
        for (int i = 0; i < m * n; i++){
            data[i] *= r;
        }
        return *this;
    }

    Table& operator+=(const Table<T>& t){
        for (int i = 0; i < m * n; i++){
            data[i] += t.data[i];
        }
        return *this;
    }

};

template <typename F>
std::ostream &operator<<(std:: ostream &s, const Table<F> &T){
    s << T.m << " " << T.n << "\n";
    for (int i = 0; i < T.m; i++){
        for (int j = 0; j < T.n; j++){
            s << T(i, j) << " ";
        }
        s << "\n";
    }
    return s;
}

#endif