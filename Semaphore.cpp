#include <condition_variable>
#include <mutex>

class Semaphore
{
private:
    std::condition_variable go;
    std::mutex mutex;
    int count;
public:
    Semaphore(int count);
    ~Semaphore();
    void acquire();
    void release();
};

Semaphore::Semaphore(int count) : count(count){}

Semaphore::~Semaphore(){}

void Semaphore::acquire()
{
    {
        std::unique_lock<std::mutex> lk(mutex);
        go.wait(lk, [this]{return count > 0; });

        count--;
    }
}

void Semaphore::release()
{
    {
        std::unique_lock<std::mutex> lk(mutex);
        count++;
    }
    go.notify_one();
}