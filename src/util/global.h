/*=====================================================================================
                                global.h
    Description:  Global Constants and Functions
        
    Created by Chen Chen on 09/28/2015
=====================================================================================*/

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define VERTEX_POSITION 0
#define VERTEX_NORMAL   1
#define VERTEX_COLOR    2
#define VERTEX_TEXTURE  3
#define VERTEX_TEMP1    4
#define VERTEX_TEMP2    5

#include <cstdlib>
#include <cmath> 
#include <chrono> 
#include <limits> 

const float math_pi = 3.1415926536f;
const float math_radians = math_pi / 180.0f;
const float math_degrees = 180.0f / math_pi;
const float math_epsilon = 1e-5f;
const float math_maxfloat = (std::numeric_limits<float>::max)();
const float math_minfloat = -(std::numeric_limits<float>::max)();

template <class T> T fract(const T& x) { return x - floor(x); }
template <class T> T round(const T& x) { return floor(x + T(0.5)); }
template <class TA, class TB, class TC> TA clamp(const TA &x, const TB &minv, const TC &maxv) { if (x < (TA)minv) return (TA)minv; else if (x >(TA)maxv) return (TA)maxv; return x; }
template <class T> T sqr(const T &x) { return x*x; }
template <class T> T cube(const T &x) { return x*x*x; }
template <class T> T sign(const T &x) { if (x > 0) return T(1); else if (x < 0) return T(-1); return T(0); }
template <class T> T deg2rad(const T &x) { return x*T(math_pi / 180.0); }
template <class T> T rad2deg(const T &x) { return x*T(180.0 / math_pi); }
template <class T> T g_min(const T &a, const T & b) { return (a < b ? a : b); }
template <class T> T g_min(const T &a, const T & b, const T & c) { return min(min(a, b), c); };
template <class T> T g_min(const T &a, const T & b, const T & c, const T & d) { return min(min(a, b), min(c, d)); };
template <class T> T g_max(const T &a, const T & b) { return (a > b ? a : b); }
template <class T> T g_max(const T &a, const T & b, const T & c) { return max(max(a, b), c); }
template <class T> T g_max(const T &a, const T & b, const T & c, const T & d) { return max(max(a, b), max(c, d)); }
template <class T> T smoothStep(const T &l, const T &u, const T &x) { T t = clamp((x - l) / (u - l), (T)0, (T)1); return t*t*(3 - 2 * t); }
template <class T> T lerp(const T &a, const T &b, const float &s) { T t = b * s + (1 - s) * a; return t; }
template <class T> void loop(T &a, const T &low, const T &high, const T &inc = 1.0) { if (a >= high) a = low; else a += inc; }
template <class T> T rand(const T &low, const T &high) { return rand() / (static_cast<float>(RAND_MAX)+1.0) * (high - low) + low; }

class HPTimer{
private:
    std::chrono::time_point<std::chrono::system_clock> t0;
    std::chrono::time_point<std::chrono::system_clock> t1;

    double et;

public:
    HPTimer()
    {
        reset();
    }

    void reset()
    {
        t0 = std::chrono::system_clock::now();
        et = 0.0;
    }

    double time()
    {
        t1 = std::chrono::system_clock::now();

        et = std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();

        return et;
        
    }
};

template <class T> class SP{
private:
    T *data;
    int *count;

public:
    SP() : data(NULL), count(NULL)
    {
        count = new int(1);
        (*count)++;
    }

    SP(T *ptr) : data(NULL), count(NULL)
    {
        count = new int(1);
        (*count)++;
    }

    SP(const SP<T> &sp) : data(sp.data), count(sp.count)
    {
        (*count)++;
    }

    ~SP()
    {
        if (--(*count) == 0)
        {
            delete data;
            delete count;
        }
    }

    T &operator*()
    {
        return *data;
    }

    T* operator ->()
    {
        return data;
    }

    SP<T> &operator=(const SP<T> &sp)
    {
        if (this != &sp)
        {
            if (--(*count) == 0)
            {
                delete data;
                delete count;
            }

            data = sp.data;
            count = sp.count;
            (*count)++;
        }

        return *this;
    }
};

//template <class T>
//void timeout(const T& callback, unsigned int timeInMs)
//{
//    concurrency::task_completion_event<void> tce;
//    auto call = new concurrency::call<int>(
//        [callback, tce](int)
//        {
//            callback();
//            tce.set();
//        });

//    auto timer = new concurrency::timer<int>(timeInMs, 0, call, true);
//    //concurrency::task<void> event_set(tce);
//    //event_set.then([timer, call]()
//    //{
//    //    delete call;
//    //    delete timer;
//    //});

//    timer->start();
//}

#endif /* end of include guard: GLOBAL_H_ */
