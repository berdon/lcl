/*
 * Tags: async, coroutine, thread_pool, c++23
 */

#pragma once
#include <coroutine>
#include <exception>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <optional>
#include <memory>

namespace lcl::async {
  // Simple thread pool implementation
  class thread_pool {
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::vector<std::thread> workers_;
    std::atomic<bool> stop_{false};

  public:
    explicit thread_pool(const size_t num_threads = std::thread::hardware_concurrency()) {
      for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back([this] {
          while (true) {
            std::function<void()> task;
            {
              std::unique_lock<std::mutex> lock(mutex_);
              cv_.wait(lock, [this] { return !tasks_.empty() || stop_.load(); });
              if (stop_.load() && tasks_.empty()) {
                return;
              }
              task = std::move(tasks_.front());
              tasks_.pop();
            }
            task();
          }
        });
      }
    }

    ~thread_pool() {
      stop_.store(true);
      cv_.notify_all();
      for (auto& worker : workers_) {
        worker.join();
      }
    }

    void enqueue(std::function<void()> func) {
      {
        std::unique_lock lock(mutex_);
        if (stop_.load()) {
          throw std::runtime_error("Cannot enqueue on stopped thread pool");
        }
        tasks_.emplace(std::move(func));
      }
      cv_.notify_one();
    }
  };

  // Global thread pool accessor
  inline thread_pool& get_thread_pool() {
    static thread_pool pool{};
    return pool;
  }

  // A task type for async operations
  template<typename T>
  struct Task {
    struct promise_type {
      std::optional<T> value_;
      std::exception_ptr exception_;

      Task get_return_object() { return {std::coroutine_handle<promise_type>::from_promise(*this)}; }
      std::suspend_never initial_suspend() { return {}; }
      std::suspend_always final_suspend() noexcept { return {}; }
      void return_value(T value) { value_ = value; }
      void unhandled_exception() { exception_ = std::current_exception(); }
    };

    bool await_ready() { return handle_.done(); }
    void await_suspend(std::coroutine_handle<> handle) {
      get_thread_pool().enqueue([handle]() mutable {
        handle.resume();
      });
    }
    T await_resume() {
      auto& promise = handle_.promise();
      if (promise.exception_) std::rethrow_exception(promise.exception_);
      return promise.value_.value();
    }

    std::coroutine_handle<promise_type> handle_;
  };

  // Specialization for void return type
  template<>
  struct Task<void> {
    struct promise_type {
      std::exception_ptr exception_;

      Task get_return_object() { return {std::coroutine_handle<promise_type>::from_promise(*this)}; }
      std::suspend_never initial_suspend() { return {}; }
      std::suspend_always final_suspend() noexcept { return {}; }
      void return_void() {}
      void unhandled_exception() { exception_ = std::current_exception(); }
    };

    [[nodiscard]] bool await_ready() const { return handle_.done(); }
    void await_suspend(std::coroutine_handle<> handle) {
      get_thread_pool().enqueue([handle]() mutable {
        handle.resume();
      });
    }
    void await_resume() const {
      auto& promise = handle_.promise();
      if (promise.exception_) std::rethrow_exception(promise.exception_);
    }

    std::coroutine_handle<promise_type> handle_;
  };

  // Deferred task for manual resolution
  template<typename T>
  struct DeferredTask {
    struct Promise {
      std::optional<T> value_;
      std::exception_ptr exception_;
      std::coroutine_handle<> continuation_;
      bool is_resolved_ = false;
    };

    struct resolver {
      std::shared_ptr<Promise> promise_;

      void resolve(T value) {
        if (promise_->is_resolved_) return;
        promise_->value_ = value;
        promise_->is_resolved_ = true;
        if (promise_->continuation_) {
          get_thread_pool().enqueue([h = promise_->continuation_]() mutable {
            if (h && !h.done()) h.resume();
          });
        }
      }

      void reject(std::exception_ptr ex) {
        if (promise_->is_resolved_) return;
        promise_->exception_ = ex;
        promise_->is_resolved_ = true;
        if (promise_->continuation_) {
          get_thread_pool().enqueue([h = promise_->continuation_]() mutable {
            if (h && !h.done()) h.resume();
          });
        }
      }

      template<typename E>
      void reject(const E& ex) {
        reject(std::make_exception_ptr(ex));
      }
    };

    // Constructor for manual resolution
    DeferredTask() : promise_(std::make_shared<Promise>()) {}

    // Constructor accepting a callable
    explicit DeferredTask(std::function<Task<void>(resolver)> fn) : promise_(std::make_shared<Promise>()) {
      resolver res{promise_};
      try {
        get_thread_pool().enqueue([fn = std::move(fn), res]() mutable {
          fn(res);
        });
      } catch (...) {
        res.reject(std::current_exception());
      }
    }

    bool await_ready() {
      return promise_->is_resolved_;
    }

    void await_suspend(std::coroutine_handle<> handle) {
      if (promise_->is_resolved_) {
        get_thread_pool().enqueue([handle]() mutable {
          if (handle && !handle.done()) handle.resume();
        });
      } else {
        promise_->continuation_ = handle;
      }
    }

    T await_resume() {
      if (promise_->exception_) {
        std::rethrow_exception(promise_->exception_);
      }
      return *promise_->value_;
    }

    resolver get_resolver() {
      return {promise_};
    }

    std::shared_ptr<Promise> promise_;
  };

  // Specialization for void return type
  template<>
  struct DeferredTask<void> {
    struct Promise {
      std::exception_ptr exception_;
      std::coroutine_handle<> continuation_;
      bool is_resolved_ = false;
    };

    struct resolver {
      std::shared_ptr<Promise> promise_;

      void resolve() const {
        if (promise_->is_resolved_) return;
        promise_->is_resolved_ = true;
        if (promise_->continuation_) {
          get_thread_pool().enqueue([h = promise_->continuation_]() mutable {
            if (h && !h.done()) h.resume();
          });
        }
      }

      void reject(const std::exception_ptr &ex) const {
        if (promise_->is_resolved_) return;
        promise_->exception_ = ex;
        promise_->is_resolved_ = true;
        if (promise_->continuation_) {
          get_thread_pool().enqueue([h = promise_->continuation_]() mutable {
            if (h && !h.done()) h.resume();
          });
        }
      }

      template<typename E>
      void reject(const E& ex) {
        reject(std::make_exception_ptr(ex));
      }
    };

    // Constructor for manual resolution
    DeferredTask() : promise_(std::make_shared<Promise>()) {}

    // Constructor accepting a callable
    explicit DeferredTask(std::function<Task<void>(resolver)> fn) : promise_(std::make_shared<Promise>()) {
      resolver res{promise_};
      try {
        get_thread_pool().enqueue([fn = std::move(fn), res]() mutable {
          // ReSharper disable once CppExpressionWithoutSideEffects
          fn(res);
        });
      } catch (...) {
        res.reject(std::current_exception());
      }
    }

    [[nodiscard]] bool await_ready() const {
      return promise_->is_resolved_;
    }

    void await_suspend(std::coroutine_handle<> handle) const {
      if (promise_->is_resolved_) {
        get_thread_pool().enqueue([handle]() mutable {
          if (handle && !handle.done()) handle.resume();
        });
      } else {
        promise_->continuation_ = handle;
      }
    }

    void await_resume() const {
      if (promise_->exception_) {
        std::rethrow_exception(promise_->exception_);
      }
    }

    [[nodiscard]] resolver get_resolver() const {
      return {promise_};
    }

    std::shared_ptr<Promise> promise_;
  };

  inline Task<void> yield() {
    co_return;
  }
}