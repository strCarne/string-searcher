#ifndef POOL_H
#define POOL_H


#include <functional>
#include <future>
#include <memory>
#include <pthread.h>
#include <queue>
#include <vector>

namespace thread {

class Pool {
public:
    inline explicit Pool(size_t num_threads) : is_running_(true) {
        pthread_mutex_init(&queue_mutex_, nullptr);
        pthread_cond_init(&condition_, nullptr);

        for (size_t i = 0; i < num_threads; ++i) {
            pthread_t thread;
            pthread_create(&thread, nullptr, &Pool::thread_entry, this);
            threads_.push_back(thread);
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
            pthread_mutex_unlock(&queue_mutex_);
        }

        pthread_cond_signal(&condition_);

        return result;
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
    }

private:
    std::vector<pthread_t> threads_;
    std::queue<std::function<void()>> tasks_;
    pthread_mutex_t queue_mutex_;
    pthread_cond_t condition_;
    bool is_running_;

    inline static void *thread_entry(void *arg) {
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

                pthread_mutex_unlock(&queue_mutex_);
            }

            if (task) {
                task();
            }
        }
    }
};

} // namespace thread

#endif // POOL_H
