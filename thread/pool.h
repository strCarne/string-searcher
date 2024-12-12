#pragma once

#include <functional>
#include <future>
#include <memory>
#include <pthread.h>
#include <queue>
#include <sched.h>
#include <thread>
#include <vector>

namespace thread {

class Pool {
public:
    inline explicit Pool(size_t num_threads) : is_running_(true) {
        pthread_setname_np(pthread_self(), "searcher:master");

        pthread_mutex_init(&queue_mutex_, nullptr);
        pthread_cond_init(&condition_, nullptr);
        pthread_cond_init(&wait_condition_, nullptr);


        std::size_t cores_num = std::thread::hardware_concurrency();

        this->cpu_cores_.resize(cores_num);
        for (auto i = 0UL; i < cores_num; ++i) {
            CPU_ZERO(&this->cpu_cores_[i]);
            CPU_SET(i, &this->cpu_cores_[i]);
        }

        for (size_t i = 0; i < num_threads; ++i) {
            pthread_t thread;
            pthread_create(&thread, nullptr, &Pool::thread_entry, this);

            threads_.push_back(thread);

            pthread_setaffinity_np(thread, sizeof(cpu_set_t), &this->cpu_cores_[i%cores_num]);
        }
    }

    inline ~Pool() { shutdown(); }

    template <typename Func, typename... Args>
    inline auto submit(Func &&func,
                       Args &&...args) -> std::future<decltype(func(args...))> {
        using ReturnType = decltype(func(args...));

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

        std::future<ReturnType> result = task->get_future();

        {
            pthread_mutex_lock(&queue_mutex_);
            tasks_.emplace([task]() { (*task)(); });
            pthread_cond_signal(&condition_);
            pthread_mutex_unlock(&queue_mutex_);
        }

        return result;
    }

    inline void wait() {
        pthread_mutex_lock(&queue_mutex_);
        while (!tasks_.empty() || active_threads_ > 0) {
            pthread_cond_wait(&wait_condition_, &queue_mutex_);
        }
        pthread_mutex_unlock(&queue_mutex_);
    }

    inline void shutdown() {
        if (!is_running_)
            return;

        is_running_ = false;
        pthread_cond_broadcast(&condition_);

        for (pthread_t &thread : threads_) {
            pthread_join(thread, nullptr);
        }

        pthread_mutex_destroy(&queue_mutex_);
        pthread_cond_destroy(&condition_);
        pthread_cond_destroy(&wait_condition_);
    }

private:
    std::vector<cpu_set_t> cpu_cores_;
    std::vector<pthread_t> threads_;
    std::queue<std::function<void()>> tasks_;
    pthread_mutex_t queue_mutex_;
    pthread_cond_t condition_;
    pthread_cond_t wait_condition_;
    bool is_running_;
    int active_threads_ = 0;

    inline static void *thread_entry(void *arg) {
        pthread_setname_np(pthread_self(), "searcher:slave");

        auto *pool = static_cast<Pool *>(arg);
        pool->worker_thread();
        return nullptr;
    }

    inline void worker_thread() {
        while (true) {
            std::function<void()> task;

            {
                pthread_mutex_lock(&queue_mutex_);

                while (tasks_.empty() && is_running_) {
                    pthread_cond_wait(&condition_, &queue_mutex_);
                }

                if (!is_running_ && tasks_.empty()) {
                    pthread_mutex_unlock(&queue_mutex_);
                    break;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
                ++active_threads_;

                pthread_mutex_unlock(&queue_mutex_);
            }

            if (task) {
                task();
            }

            {
                pthread_mutex_lock(&queue_mutex_);
                --active_threads_;
                if (tasks_.empty() && active_threads_ == 0) {
                    pthread_cond_signal(&wait_condition_);
                }
                pthread_mutex_unlock(&queue_mutex_);
            }
        }
    }
};

} // namespace thread
