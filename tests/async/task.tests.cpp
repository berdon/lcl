#include <gtest/gtest.h>
#include <chrono>
#include <atomic>
#include <stdexcept>

#include "async/task.hpp"
#include "result/Either.hpp"

using namespace lcl::async;

namespace {
  // Helper function to wait for a deferred_task to resolve with timeout.
  template <typename T>
  void wait_for_resolution(const DeferredTask<T>& dt) {
    const auto start = std::chrono::steady_clock::now();
    while (!dt.promise_->is_resolved_) {
      if (std::chrono::steady_clock::now() - start > std::chrono::seconds(5)) {
        FAIL() << "Timeout waiting for resolution";
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}  // namespace

TEST(AsyncTest, CanAwaitResult) {
  const DeferredTask<int> main_dt{[](DeferredTask<int>::resolver res) -> Task<void> {
    auto my_task = []() -> Task<int> {
      co_return 42;
    };
    try {
      const int val = co_await my_task();
      res.resolve(val);
    } catch (...) {
      res.reject(std::current_exception());
    }
    co_return;
  }};

  wait_for_resolution(main_dt);
  ASSERT_FALSE(main_dt.promise_->exception_);
  EXPECT_EQ(*main_dt.promise_->value_, 42);
}

TEST(AsyncTest, ExecutionOrderMaintained) {
  const DeferredTask<std::string> main_dt{[](DeferredTask<std::string>::resolver res) -> Task<void> {
    auto get_one = []() -> Task<std::string> {
      co_return "1";
    };
    auto get_two = []() -> Task<std::string> {
      co_return "2";
    };
    try {
      std::string sum = co_await get_one();
      sum += co_await get_two();
      res.resolve(sum);
    } catch (...) {
      res.reject(std::current_exception());
    }
    co_return;
  }};

  wait_for_resolution(main_dt);
  ASSERT_FALSE(main_dt.promise_->exception_);
  EXPECT_EQ(*main_dt.promise_->value_, "12");
}

TEST(AsyncTest, CanStartTaskDoOtherThingsThenAwait) {
  std::atomic started{0};
  const DeferredTask<int> main_dt{[&started](DeferredTask<int>::resolver res) -> Task<void> {
    auto my_task = [&started]() -> Task<int> {
      started.store(1);
      co_return 42;
    };
    try {
      auto t = my_task();
      // Simulate doing other work before awaiting
      if (started.load() != 1) {
        throw std::runtime_error("Task did not start before other things");
      }
      const int val = co_await t;
      res.resolve(val);
    } catch (...) {
      res.reject(std::current_exception());
    }
    co_return;
  }};

  wait_for_resolution(main_dt);
  ASSERT_FALSE(main_dt.promise_->exception_);
  EXPECT_EQ(*main_dt.promise_->value_, 42);
}

TEST(AsyncTest, CanResumeWithDeferredTask) {
  DeferredTask<int> dt {};
  auto resolver = dt.get_resolver();

  get_thread_pool().enqueue([&resolver] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    resolver.resolve(42);
  });

  const DeferredTask<int> main_dt{[&dt](DeferredTask<int>::resolver res) -> Task<void> {
    try {
      const int val = co_await dt;
      res.resolve(val);
    } catch (...) {
      res.reject(std::current_exception());
    }
    co_return;
  }};

  wait_for_resolution(main_dt);
  ASSERT_FALSE(main_dt.promise_->exception_);
  EXPECT_EQ(*main_dt.promise_->value_, 42);
}

TEST(AsyncTest, CanThrowWithDeferredTask) {
  DeferredTask<int> dt{};
  auto resolver = dt.get_resolver();

  get_thread_pool().enqueue([&resolver] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    resolver.reject(std::runtime_error("test error"));
  });

  const DeferredTask<int> main_dt{[&dt](DeferredTask<int>::resolver res) -> Task<void> {
    try {
      const int val = co_await dt;
      res.resolve(val);
    } catch (...) {
      res.reject(std::current_exception());
    }
    co_return;
  }};

  wait_for_resolution(main_dt);
  ASSERT_TRUE(main_dt.promise_->exception_);
  try {
    std::rethrow_exception(main_dt.promise_->exception_);
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "test error");
  }
}

template <typename T>
class Foo {
  T value;

public:
  Foo() = default;
  Foo(T val) : value(val) {}

  // Add operator== for comparison with T
  bool operator==(const T& other) const {
    return value == other;
  }
};

TEST(AsyncTest, CanAwaitTemplateFunction) {
  const DeferredTask<Foo<int>> main_dt{[](DeferredTask<Foo<int>>::resolver res) -> Task<void> {
    auto my_task = []() -> Task<Foo<int>> {
      co_return 42;
    };
    try {
      const Foo<int> val = co_await my_task();
      res.resolve(val);
    } catch (...) {
      res.reject(std::current_exception());
    }
    co_return;
  }};

  wait_for_resolution(main_dt);
  ASSERT_FALSE(main_dt.promise_->exception_);
  EXPECT_EQ(*main_dt.promise_->value_, 42);
}

enum ErrorCodes : uint8_t {
  EC_ONE
};

Task<lcl::Either<bool, lcl::Error<ErrorCodes, void>>> delay_task(const int delay) {
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
  co_return true;
}

TEST(AsyncTest, CanAwaitTaskEitherFunction) {
  auto task = delay_task(100);
  task.await_ready();
  ASSERT_FALSE(task.handle_.promise().exception_);
  EXPECT_EQ(*task.handle_.promise().value_.value(), true);
}