#include <iostream>
#include <thread>
#include <semaphore>

std::counting_semaphore<2> sem(2); // max 2, начальное значение 2

void f(int id) {
    sem.acquire();
    std::cout << "Thread " << id << " in semaphore\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sem.release();
}

int main() {
    std::thread threads[4];
    for (int i = 0; i < 4; ++i) {
        threads[i] = std::thread(f, i);
    }
    for (auto& t : threads) t.join();
    return 0;
}
