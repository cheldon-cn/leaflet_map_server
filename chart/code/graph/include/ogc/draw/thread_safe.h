#ifndef OGC_DRAW_THREAD_SAFE_H
#define OGC_DRAW_THREAD_SAFE_H

#include <mutex>
#include <atomic>
#include <memory>
#include <functional>

namespace ogc {
namespace draw {

template<typename T>
class ThreadSafe {
public:
    ThreadSafe() = default;
    explicit ThreadSafe(const T& value) : m_value(value) {}
    explicit ThreadSafe(T&& value) : m_value(std::move(value)) {}
    
    ThreadSafe(const ThreadSafe& other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_value = other.m_value;
    }
    
    ThreadSafe(ThreadSafe&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_value = std::move(other.m_value);
    }
    
    ThreadSafe& operator=(const ThreadSafe& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lock1(m_mutex, std::defer_lock);
            std::lock_guard<std::mutex> lock2(other.m_mutex, std::defer_lock);
            std::lock(lock1, lock2);
            m_value = other.m_value;
        }
        return *this;
    }
    
    ThreadSafe& operator=(ThreadSafe&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lock1(m_mutex, std::defer_lock);
            std::lock_guard<std::mutex> lock2(other.m_mutex, std::defer_lock);
            std::lock(lock1, lock2);
            m_value = std::move(other.m_value);
        }
        return *this;
    }
    
    T Get() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_value;
    }
    
    void Set(const T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_value = value;
    }
    
    void Set(T&& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_value = std::move(value);
    }
    
    T Exchange(const T& newValue) {
        std::lock_guard<std::mutex> lock(m_mutex);
        T oldValue = std::move(m_value);
        m_value = newValue;
        return oldValue;
    }
    
    T Exchange(T&& newValue) {
        std::lock_guard<std::mutex> lock(m_mutex);
        T oldValue = std::move(m_value);
        m_value = std::move(newValue);
        return oldValue;
    }
    
    template<typename Func>
    auto WithLock(Func&& func) -> decltype(func(m_value)) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return func(m_value);
    }
    
    template<typename Func>
    auto WithLock(Func&& func) const -> decltype(func(m_value)) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return func(m_value);
    }
    
    std::mutex& GetMutex() const { return m_mutex; }
    
private:
    mutable T m_value;
    mutable std::mutex m_mutex;
};

class ReadWriteLock {
public:
    ReadWriteLock() : m_readers(0), m_writers(0), m_writeWaiting(0) {}
    
    void LockRead() {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_writers > 0 || m_writeWaiting > 0) {
            m_readCond.wait(lock);
        }
        ++m_readers;
    }
    
    void UnlockRead() {
        std::unique_lock<std::mutex> lock(m_mutex);
        --m_readers;
        if (m_readers == 0) {
            m_writeCond.notify_one();
        }
    }
    
    void LockWrite() {
        std::unique_lock<std::mutex> lock(m_mutex);
        ++m_writeWaiting;
        while (m_readers > 0 || m_writers > 0) {
            m_writeCond.wait(lock);
        }
        --m_writeWaiting;
        ++m_writers;
    }
    
    void UnlockWrite() {
        std::unique_lock<std::mutex> lock(m_mutex);
        --m_writers;
        if (m_writeWaiting > 0) {
            m_writeCond.notify_one();
        } else {
            m_readCond.notify_all();
        }
    }
    
    bool TryLockRead() {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_writers > 0 || m_writeWaiting > 0) {
            return false;
        }
        ++m_readers;
        return true;
    }
    
    bool TryLockWrite() {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_readers > 0 || m_writers > 0) {
            return false;
        }
        ++m_writers;
        return true;
    }
    
private:
    std::mutex m_mutex;
    std::condition_variable m_readCond;
    std::condition_variable m_writeCond;
    int m_readers;
    int m_writers;
    int m_writeWaiting;
};

class ReadLockGuard {
public:
    explicit ReadLockGuard(ReadWriteLock& lock) : m_lock(lock) {
        m_lock.LockRead();
    }
    ~ReadLockGuard() {
        m_lock.UnlockRead();
    }
    ReadLockGuard(const ReadLockGuard&) = delete;
    ReadLockGuard& operator=(const ReadLockGuard&) = delete;
private:
    ReadWriteLock& m_lock;
};

class WriteLockGuard {
public:
    explicit WriteLockGuard(ReadWriteLock& lock) : m_lock(lock) {
        m_lock.LockWrite();
    }
    ~WriteLockGuard() {
        m_lock.UnlockWrite();
    }
    WriteLockGuard(const WriteLockGuard&) = delete;
    WriteLockGuard& operator=(const WriteLockGuard&) = delete;
private:
    ReadWriteLock& m_lock;
};

class SpinLock {
public:
    SpinLock() : m_flag(false) {}
    
    void Lock() {
        while (m_flag.exchange(true, std::memory_order_acquire)) {
            while (m_flag.load(std::memory_order_relaxed)) {
                std::this_thread::yield();
            }
        }
    }
    
    void Unlock() {
        m_flag.store(false, std::memory_order_release);
    }
    
    bool TryLock() {
        return !m_flag.exchange(true, std::memory_order_acquire);
    }
    
private:
    std::atomic<bool> m_flag;
};

class SpinLockGuard {
public:
    explicit SpinLockGuard(SpinLock& lock) : m_lock(lock) {
        m_lock.Lock();
    }
    ~SpinLockGuard() {
        m_lock.Unlock();
    }
    SpinLockGuard(const SpinLockGuard&) = delete;
    SpinLockGuard& operator=(const SpinLockGuard&) = delete;
private:
    SpinLock& m_lock;
};

class AtomicCounter {
public:
    AtomicCounter() : m_count(0) {}
    explicit AtomicCounter(int initial) : m_count(initial) {}
    
    int Increment() {
        return m_count.fetch_add(1, std::memory_order_relaxed) + 1;
    }
    
    int Decrement() {
        return m_count.fetch_sub(1, std::memory_order_relaxed) - 1;
    }
    
    int Add(int value) {
        return m_count.fetch_add(value, std::memory_order_relaxed) + value;
    }
    
    int Subtract(int value) {
        return m_count.fetch_sub(value, std::memory_order_relaxed) - value;
    }
    
    int Get() const {
        return m_count.load(std::memory_order_relaxed);
    }
    
    void Set(int value) {
        m_count.store(value, std::memory_order_relaxed);
    }
    
    int Exchange(int value) {
        return m_count.exchange(value, std::memory_order_relaxed);
    }
    
    bool CompareExchange(int& expected, int desired) {
        return m_count.compare_exchange_weak(expected, desired, 
                                              std::memory_order_relaxed);
    }
    
    int operator++() { return Increment(); }
    int operator--() { return Decrement(); }
    int operator++(int) { return m_count.fetch_add(1, std::memory_order_relaxed); }
    int operator--(int) { return m_count.fetch_sub(1, std::memory_order_relaxed); }
    
private:
    std::atomic<int> m_count;
};

class ThreadSafeFlag {
public:
    ThreadSafeFlag() : m_flag(false) {}
    explicit ThreadSafeFlag(bool initial) : m_flag(initial) {}
    
    void Set() {
        m_flag.store(true, std::memory_order_release);
    }
    
    void Clear() {
        m_flag.store(false, std::memory_order_release);
    }
    
    bool IsSet() const {
        return m_flag.load(std::memory_order_acquire);
    }
    
    bool TestAndSet() {
        return m_flag.exchange(true, std::memory_order_acq_rel);
    }
    
    bool TestAndClear() {
        return m_flag.exchange(false, std::memory_order_acq_rel);
    }
    
    void Wait(bool value = true) const {
        while (m_flag.load(std::memory_order_acquire) != value) {
            std::this_thread::yield();
        }
    }
    
    template<typename Rep, typename Period>
    bool WaitFor(const std::chrono::duration<Rep, Period>& timeout, bool value = true) const {
        auto start = std::chrono::steady_clock::now();
        while (m_flag.load(std::memory_order_acquire) != value) {
            if (std::chrono::steady_clock::now() - start >= timeout) {
                return false;
            }
            std::this_thread::yield();
        }
        return true;
    }
    
private:
    std::atomic<bool> m_flag;
};

}
}

#endif
