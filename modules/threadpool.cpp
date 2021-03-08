#include<threadpool.h>

ThreadPool::ThreadPool()
{
    isWorking_.store(false, MEM_ORD);
    fireOne_.store(false, MEM_ORD);
    allThreadsCount_.store(0, MEM_ORD);
    idleThreadsCount_.store(0, MEM_ORD);
    tasksCount_.store(0, MEM_ORD);
}

ThreadPool::~ThreadPool() { stop(); }

void ThreadPool::execute(Task task)
{
    std::lock_guard<std::mutex> lg(mutex_);
    tasks_.push(task);
    tasksCount_++;
    condition_.notify_one();
}

void ThreadPool::start()
{
    isWorking_.store(true, MEM_ORD);
    newThreads(0);
}

void ThreadPool::stop()
{
    if(isWorking_.exchange(false) == true)
        condition_.notify_all();
}

void ThreadPool::interrupt()
{
    stop();
}

void ThreadPool::wait()
{
    std::unique_lock<std::mutex> ul(rod_);
    ctrl_.wait(ul);
}

void ThreadPool::awake()
{
    ctrl_.notify_all();
}

uint32_t ThreadPool::getThreadsCount() const { return allThreadsCount_.load(MEM_ORD); }

uint32_t ThreadPool::getTasksCount() const { return tasksCount_.load(MEM_ORD); }

uint32_t ThreadPool::getIdleCount() const { return idleThreadsCount_.load(MEM_ORD); }

void ThreadPool::newThread() { newThreads(1); }

void ThreadPool::newThreads(uint32_t count)
{
    if(isWorking_.load() == false)
        return;
    if(count <= 0)
        count = std::thread::hardware_concurrency();
    for(int i=0; i<count; i++)
    {
        std::thread(&ThreadPool::task, this).detach();
        allThreadsCount_++;
    }
}

void ThreadPool::closeThreads(uint32_t count)
{
    int temp;
    count = (count < (temp = allThreadsCount_.load(MEM_ORD))) ? count:temp;
    for(int i=0; i<count; i++)
    {
        while(fireOne_.exchange(true, MEM_ORD) == true)
            condition_.notify_one();
        condition_.notify_one();
    }
}





void ThreadPool::task()
{
    while(isWorking_.load(MEM_ORD) == true && fireOne_.exchange(false, MEM_ORD) == false)
    {
        std::unique_lock<std::mutex> ul(mutex_);
        if(tasks_.empty() == true)
        {
            idleThreadsCount_++;
            condition_.wait(ul);
            idleThreadsCount_--;
        }
        else
        {
            Task work = std::move(tasks_.front());
            tasks_.pop();
            ul.unlock();
            tasksCount_--;
            work();
            ul.lock();
        }
    }
    allThreadsCount_--;
}