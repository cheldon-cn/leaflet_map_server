#pragma once

#include <mutex>
#include <shared_mutex>

namespace ogc {

class CNReadWriteLock {
public:
    CNReadWriteLock() = default;

    void LockRead() {
        read_lock_ = std::unique_lock<std::shared_mutex>(mutex_);
    }

    void UnlockRead() {
        read_lock_.unlock();
    }

    void LockWrite() {
        write_lock_ = std::unique_lock<std::shared_mutex>(mutex_);
    }

    void UnlockWrite() {
        write_lock_.unlock();
    }

    class ReadGuard {
    public:
        explicit ReadGuard(CNReadWriteLock& lock) : lock_(lock) {
            lock_.LockRead();
        }
        ~ReadGuard() {
            lock_.UnlockRead();
        }
        ReadGuard(const ReadGuard&) = delete;
        ReadGuard& operator=(const ReadGuard&) = delete;
    private:
        CNReadWriteLock& lock_;
    };

    class WriteGuard {
    public:
        explicit WriteGuard(CNReadWriteLock& lock) : lock_(lock) {
            lock_.LockWrite();
        }
        ~WriteGuard() {
            lock_.UnlockWrite();
        }
        WriteGuard(const WriteGuard&) = delete;
        WriteGuard& operator=(const WriteGuard&) = delete;
    private:
        CNReadWriteLock& lock_;
    };

private:
    mutable std::shared_mutex mutex_;
    std::unique_lock<std::shared_mutex> read_lock_;
    std::unique_lock<std::shared_mutex> write_lock_;
};

} // namespace ogc
