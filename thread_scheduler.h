#include <pthread.h>
#include <kmer_record.h>

typedef void* (*pthread_func_ptr) (void*)

template <typename T> class ThreadScheduler
{
    private:
        uint16_t m_num_active_threads;
        uint16_t m_thread_limit;
        uint64_t m_block_size;
        std::queue<T> m_queue;
        pthread_mutex_t m_queue_mutex;
        std::vector<Thread> m_threads;

    public:
        ThreadScheduler(uint16_t, uint64_t);
        ~ThreadScheduler();
        feedData(T, uint64_t);
        cancelAllThreads();
        joinAllThreads();
        queueSize();
};

template <typename T> class Thread
{
    private:
        uint16_t m_id;
        pthread_t m_thread;
        T m_data;
        pthread_mutex_t m_data_mutex;
        pthread_cond_t m_data_cond;
        bool m_running, m_detached;
    public:
        Thread(uint16_t,pthread_func_ptr, T);
        ~Thread();
        updateData(T);
        isRunning();
        isDetached();
        join();
        detach();
};
