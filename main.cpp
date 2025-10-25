#include <iostream>
#include <vector>

#include <thread>   // std::thread, std::jthread
#include <mutex>    // std::mutex
#include <atomic>   // std::atomic
#include <future>   // std::promise & std::future , std::async
#include <condition_variable> // variables condicionada

using namespace std;

// Variable Global
mutex m;

void fun_1() {
    m.lock();
    cout<<"hilo 1"<<endl;
    m.unlock();
}

void fun_2() {
    m.lock();
    cout<<"hilo 2"<<endl;
    m.unlock();
}

void fun_3() {
    m.lock();
    cout<<"hilo 3"<<endl;
    m.unlock();
}

void ejercicio_1() {
    thread t1;
    thread t2;
    thread t3;

    // fork
    t1 = thread(fun_1);
    t2 = thread(fun_2);
    t3 = thread(fun_3);

    m.lock();
    std::cout << "Hello, World!" << std::endl;
    m.unlock();

    // join
    t1.join();
    t2.join();
    t3.join();
}

void fun_mensaje(const string& mensaje) {
    m.lock();
    cout << mensaje << endl;
    //--------------> ERROR
    m.unlock();
}

void ejercicio_2() {
    thread t1(fun_mensaje, "--Hilo 1");
    thread t2(fun_mensaje, "--Hilo 2");
    thread t3(fun_mensaje, "--Hilo 3");

    t1.join();
    t2.join();
    t3.join();
}

void ejercicio_3() {
    thread t1([]{ fun_mensaje("--Hilo 1"); });
    thread t2([]{ fun_mensaje("--Hilo 2"); });
    thread t3([]{ fun_mensaje("--Hilo 3"); });

    t1.join();
    t2.join();
    t3.join();
}

void fun_incrementar(int& valor, int incremento) {
    valor += incremento;
}


void fun_incrementar_concurrent(int& valor, int incremento) {
    lock_guard<mutex> lock(m);
    valor += incremento;
    // ... instrucciones
} // m.unlock

template <typename T>
class Guardia_Bloqueo {
    T& m_;
public:
    Guardia_Bloqueo(T& m) : m_(m) { m_.lock(); }
    ~Guardia_Bloqueo() { m_.unlock(); }
};

void fun_incrementar_concurrent_GB(int& valor, int incremento) {
    Guardia_Bloqueo<mutex> lock(m); // m.lock();
    valor += incremento;
} // m.unlock();

void fun_incrementar_concurrent_unique_lock(int& valor, int incremento) {
    unique_lock<mutex> lock(m);
    valor += incremento;
    lock.unlock();
    // ... instrucciones
}  // m.unlock()

template <typename T>
class Bloqueo_Unico {
    T& m_;
public:
    Bloqueo_Unico(T& m) : m_(m) { m_.lock(); }
    ~Bloqueo_Unico() { m_.unlock(); }

    void unlock() { m_.unlock(); }
};

void fun_incrementar_concurrent_unique_lock(int& valor, int incremento) {
    Bloqueo_Unico<mutex> lock(m);
    valor += incremento;
    lock.unlock();
    // ... instrucciones
}  // m.unlock()


void ejercicio_4() {
    constexpr int n = 1000;
    int x = 0;
    for (int i = 0; i < n; i++) {
        fun_incrementar(x, 1);
    }
    cout << x << endl;

    int y = 0;
    vector<thread> ts;
    ts.reserve(n);
    for (int i = 0; i < n; i++) {
        ts.emplace_back(fun_incrementar_concurrent, std::ref(y), 1);
    }

    for (int i = 0; i < n; i++) {
        ts[i].join();
    }
    cout << y << endl;
}

void ejercicio_5() {
    constexpr int n = 1000;
    int x = 0;
    for (int i = 0; i < n; i++) {
        fun_incrementar(x, 1);
    }
    cout << x << endl;

    int y = 0;
    vector<thread> ts;
    ts.reserve(n);
    for (int i = 0; i < n; i++) {
        ts.emplace_back([&y] {
            lock_guard<mutex> lock(m);
            y += 1;
        });
    }

    for (int i = 0; i < n; i++) {
        ts[i].join();
    }
    cout << y << endl;
}

void ejercicio_6() {
    constexpr int n = 1000;
    atomic<int> y = 0;
    vector<thread> ts;
    ts.reserve(n);
    for (int i = 0; i < n; i++) {
        ts.emplace_back([&y] { y += 1; });
    }

    for (int i = 0; i < n; i++) {
        ts[i].join();
    }
    cout << y << endl;
}


int main()
{
    // cout << std::thread::hardware_concurrency() << endl;
    // ejercicio_2();
    // ejercicio_3();
    // ejercicio_4();
    // ejercicio_5();
    ejercicio_6();
    return 0;
}
