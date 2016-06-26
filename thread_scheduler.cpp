#include <thread_scheduler.h>

Thread::Thread(uint16_t id, pthread_func_ptr func_ptr, T init_data=NULL)
{
    m_id = id;
    m_data = init_data;
    m_data_mutex = PTHREAD_MUTEX_INITIALIZER;
    m_data_cond = PTHREAD_COND_INITIALIZER;
    m_running = 0;
    m_detached = 0;
    pthread_create(&m_thread, NULL, func_ptr, init_data);
}

Thread::~Thread()
{
    int state = 0;
    if(!m_detached)
    {
        state = pthread_cancel(&m_thread);
    }
    else if(m_running)
    {
        state = pthread_cancel(&m_thread);
    }
    else
    {
        //do nothing -- thread will terminate on its own (hopefully)
    }
    pthread_mutex_destroy(&m_data_mutex);
    pthread_cond_destroy(&m_data_cond);
}

int Thread::updateData(T data)
{
    if(!m_running)
    {
        pthread_mutex_lock(&m_data_mutex);

        m_data = data;
        pthread_cond_signal(&m_data_cond);

        pthread_mutex_unlock(&m_data_mutex);
        return 0;
    }
    else
    {
        return 1;
    }
}
bool Thread::isRunning()
{
    pthread_mutex_lock(&m_data_mutex);
    bool is_running = m_running;
    pthread_mutex_unlock(&m_data_mutex);
    return is_running;
}

bool Thread::isDetached()
{
    pthread_mutex_lock(&m_data_mutex);
    bool is_detached = m_detached;
    pthread_mutex_unlock(&m_data_mutex);
    return is_detached;
}

int Thread::join()
{
     return pthread_join(&m_thread);
}

int Thread::detach()
{
    return pthread_detach(&m_thread);
}



ThreadScheduler::ThreadScheduler(uint16_t num_threads, std::queue<T> init_queue)
{
    m_thread_limit = num_threads;
    m_queue = init_queue;
    m_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    m_queue_cond = PTHREAD_COND_INITIALIZER;
    for(int i = 0; i < m_thread_limit; i++)
    {
	

