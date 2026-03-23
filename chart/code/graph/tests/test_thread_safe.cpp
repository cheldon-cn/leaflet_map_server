#include <gtest/gtest.h>
#include "ogc/draw/thread_safe.h"
#include <thread>
#include <vector>
#include <atomic>

using namespace ogc::draw;

class ThreadSafeTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(ThreadSafeTest, ValueConstructor) {
    ThreadSafe<int> value(42);
    EXPECT_EQ(value.Get(), 42);
}

TEST_F(ThreadSafeTest, SetGet) {
    ThreadSafe<int> value;
    value.Set(100);
    EXPECT_EQ(value.Get(), 100);
}

TEST_F(ThreadSafeTest, Exchange) {
    ThreadSafe<int> value(10);
    auto old = value.Exchange(20);
    EXPECT_EQ(old, 10);
    EXPECT_EQ(value.Get(), 20);
}

TEST_F(ThreadSafeTest, WithLock) {
    ThreadSafe<int> value(5);
    auto result = value.WithLock([](int& v) {
        v *= 2;
        return v;
    });
    EXPECT_EQ(result, 10);
    EXPECT_EQ(value.Get(), 10);
}

TEST_F(ThreadSafeTest, WithLockConst) {
    const ThreadSafe<int> value(5);
    auto result = value.WithLock([](const int& v) {
        return v * 2;
    });
    EXPECT_EQ(result, 10);
    EXPECT_EQ(value.Get(), 5);
}

TEST_F(ThreadSafeTest, ConcurrentAccess) {
    ThreadSafe<int> value(0);
    std::atomic<int> sum{0};
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&value, &sum]() {
            for (int j = 0; j < 100; j++) {
                value.WithLock([&sum](int& v) {
                    v++;
                    sum++;
                });
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(value.Get(), 1000);
    EXPECT_EQ(sum.load(), 1000);
}

class ReadWriteLockTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(ReadWriteLockTest, LockReadUnlockRead) {
    ReadWriteLock lock;
    lock.LockRead();
    lock.UnlockRead();
}

TEST_F(ReadWriteLockTest, LockWriteUnlockWrite) {
    ReadWriteLock lock;
    lock.LockWrite();
    lock.UnlockWrite();
}

TEST_F(ReadWriteLockTest, MultipleReaders) {
    ReadWriteLock lock;
    lock.LockRead();
    lock.LockRead();
    lock.UnlockRead();
    lock.UnlockRead();
}

TEST_F(ReadWriteLockTest, ConcurrentReaders) {
    ReadWriteLock lock;
    std::atomic<int> readers{0};
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&lock, &readers]() {
            lock.LockRead();
            readers++;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            readers--;
            lock.UnlockRead();
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}
