#ifndef OGC_JNI_BRIDGE_JNI_MEMORY_H
#define OGC_JNI_BRIDGE_JNI_MEMORY_H

#include "jni_bridge/export.h"

#ifdef __ANDROID__

#include <jni.h>
#include <memory>
#include <functional>
#include <mutex>
#include <map>
#include <atomic>

namespace ogc {
namespace jni {

class OGC_JNI_BRIDGE_API JniMemoryManager {
public:
    using DestructorFn = std::function<void(void*)>;
    
    static JniMemoryManager* GetInstance();
    
    void* Allocate(size_t size);
    void Deallocate(void* ptr);
    
    void* AllocateWithDestructor(size_t size, DestructorFn destructor);
    
    template<typename T, typename... Args>
    T* New(Args&&... args) {
        void* ptr = Allocate(sizeof(T));
        if (ptr) {
            new (ptr) T(std::forward<Args>(args)...);
            RegisterDestructor(ptr, [](void* p) {
                static_cast<T*>(p)->~T();
            });
        }
        return static_cast<T*>(ptr);
    }
    
    template<typename T>
    void Delete(T* ptr) {
        if (ptr) {
            ptr->~T();
            Deallocate(ptr);
        }
    }
    
    jlong ToJLong(void* ptr);
    void* FromJLong(jlong value);
    
    jobject CreateNativePointer(JNIEnv* env, void* ptr, const std::string& className);
    void* GetNativePointer(JNIEnv* env, jobject obj);
    void SetNativePointer(JNIEnv* env, jobject obj, void* ptr);
    
    void RegisterDestructor(void* ptr, DestructorFn destructor);
    void UnregisterDestructor(void* ptr);
    
    void RegisterNativeObject(jlong handle, void* obj);
    void UnregisterNativeObject(jlong handle);
    void* GetNativeObject(jlong handle);
    
    size_t GetAllocatedCount() const;
    size_t GetAllocatedSize() const;
    void CollectGarbage();
    
    void SetMemoryLimit(size_t limit);
    bool IsMemoryLimitExceeded() const;

private:
    JniMemoryManager();
    ~JniMemoryManager();
    
    JniMemoryManager(const JniMemoryManager&) = delete;
    JniMemoryManager& operator=(const JniMemoryManager&) = delete;
    
    static JniMemoryManager* s_instance;
    
    std::mutex m_mutex;
    std::map<void*, size_t> m_allocations;
    std::map<void*, DestructorFn> m_destructors;
    std::map<jlong, void*> m_nativeObjects;
    std::atomic<jlong> m_nextHandle;
    size_t m_totalAllocated;
    size_t m_memoryLimit;
    bool m_limitExceeded;
};

class OGC_JNI_BRIDGE_API JniNativeObjectBase {
public:
    JniNativeObjectBase();
    virtual ~JniNativeObjectBase();
    
    jlong GetHandle() const { return m_handle; }
    void AddRef();
    void Release();
    int GetRefCount() const { return m_refCount; }
    
protected:
    virtual void OnDestroy() {}
    
private:
    jlong m_handle;
    std::atomic<int> m_refCount;
};

template<typename T>
class JniNativeObject : public JniNativeObjectBase {
public:
    template<typename... Args>
    static T* Create(Args&&... args) {
        T* obj = new T(std::forward<Args>(args)...);
        return obj;
    }
    
    static T* FromHandle(jlong handle) {
        return static_cast<T*>(JniMemoryManager::GetInstance()->GetNativeObject(handle));
    }
    
    static T* FromJObject(JNIEnv* env, jobject obj) {
        void* ptr = JniMemoryManager::GetInstance()->GetNativePointer(env, obj);
        return static_cast<T*>(ptr);
    }
};

template<typename T>
class JniGlobalRef {
public:
    JniGlobalRef(JNIEnv* env, T ref) : m_env(env), m_ref(nullptr) {
        if (ref) {
            m_ref = static_cast<T>(env->NewGlobalRef(ref));
        }
    }
    
    ~JniGlobalRef() {
        if (m_ref) {
            m_env->DeleteGlobalRef(m_ref);
        }
    }
    
    T Get() const { return m_ref; }
    operator T() const { return m_ref; }
    bool IsValid() const { return m_ref != nullptr; }
    
private:
    JNIEnv* m_env;
    T m_ref;
    
    JniGlobalRef(const JniGlobalRef&) = delete;
    JniGlobalRef& operator=(const JniGlobalRef&) = delete;
};

}  
}  

#endif

#endif
