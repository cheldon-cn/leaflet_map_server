#pragma once

#include <mutex>
#include <shared_mutex>

namespace ogc {

class CNReadWriteLock {
public:
    CNReadWriteLock() = default;

    void LockRead() {
        mutex_.lock_shared();
    }

    void UnlockRead() {
        mutex_.unlock_shared();
    }

    void LockWrite() {
        mutex_.lock();
    }

    void UnlockWrite() {
        mutex_.unlock();
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
};

} // namespace ogc
