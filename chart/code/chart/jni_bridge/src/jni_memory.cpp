#include "jni_bridge/jni_memory.h"

#ifdef __ANDROID__

#include <android/log.h>
#include <cstdlib>
#include <cstring>
#include <cassert>

#define LOG_TAG "JniMemoryManager"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace ogc {
namespace jni {

JniMemoryManager* JniMemoryManager::s_instance = nullptr;

JniMemoryManager* JniMemoryManager::GetInstance() {
    if (!s_instance) {
        s_instance = new JniMemoryManager();
    }
    return s_instance;
}

JniMemoryManager::JniMemoryManager()
    : m_nextHandle(1)
    , m_totalAllocated(0)
    , m_memoryLimit(0)
    , m_limitExceeded(false) {
}

JniMemoryManager::~JniMemoryManager() {
    CollectGarbage();
}

void* JniMemoryManager::Allocate(size_t size) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_memoryLimit > 0 && m_totalAllocated + size > m_memoryLimit) {
        m_limitExceeded = true;
        LOGE("Memory limit exceeded: %zu > %zu", m_totalAllocated + size, m_memoryLimit);
        return nullptr;
    }
    
    void* ptr = std::malloc(size);
    if (ptr) {
        m_allocations[ptr] = size;
        m_totalAllocated += size;
    }
    
    return ptr;
}

void JniMemoryManager::Deallocate(void* ptr) {
    if (!ptr) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_allocations.find(ptr);
    if (it != m_allocations.end()) {
        m_totalAllocated -= it->second;
        m_allocations.erase(it);
        
        auto destructorIt = m_destructors.find(ptr);
        if (destructorIt != m_destructors.end()) {
            destructorIt->second(ptr);
            m_destructors.erase(destructorIt);
        }
        
        std::free(ptr);
    }
}

void* JniMemoryManager::AllocateWithDestructor(size_t size, DestructorFn destructor) {
    void* ptr = Allocate(size);
    if (ptr && destructor) {
        RegisterDestructor(ptr, destructor);
    }
    return ptr;
}

jlong JniMemoryManager::ToJLong(void* ptr) {
    return reinterpret_cast<jlong>(ptr);
}

void* JniMemoryManager::FromJLong(jlong value) {
    return reinterpret_cast<void*>(value);
}

jobject JniMemoryManager::CreateNativePointer(JNIEnv* env, void* ptr, const std::string& className) {
    if (!env || !ptr) {
        return nullptr;
    }
    
    jclass clazz = env->FindClass(className.c_str());
    if (!clazz) {
        return nullptr;
    }
    
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "(J)V");
    if (!constructor) {
        env->DeleteLocalRef(clazz);
        return nullptr;
    }
    
    jobject obj = env->NewObject(clazz, constructor, ToJLong(ptr));
    env->DeleteLocalRef(clazz);
    
    return obj;
}

void* JniMemoryManager::GetNativePointer(JNIEnv* env, jobject obj) {
    if (!env || !obj) {
        return nullptr;
    }
    
    jclass clazz = env->GetObjectClass(obj);
    jfieldID fieldID = env->GetFieldID(clazz, "nativePointer", "J");
    
    if (!fieldID) {
        env->DeleteLocalRef(clazz);
        return nullptr;
    }
    
    jlong ptr = env->GetLongField(obj, fieldID);
    env->DeleteLocalRef(clazz);
    
    return FromJLong(ptr);
}

void JniMemoryManager::SetNativePointer(JNIEnv* env, jobject obj, void* ptr) {
    if (!env || !obj) {
        return;
    }
    
    jclass clazz = env->GetObjectClass(obj);
    jfieldID fieldID = env->GetFieldID(clazz, "nativePointer", "J");
    
    if (fieldID) {
        env->SetLongField(obj, fieldID, ToJLong(ptr));
    }
    
    env->DeleteLocalRef(clazz);
}

void JniMemoryManager::RegisterDestructor(void* ptr, DestructorFn destructor) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_destructors[ptr] = std::move(destructor);
}

void JniMemoryManager::UnregisterDestructor(void* ptr) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_destructors.erase(ptr);
}

void JniMemoryManager::RegisterNativeObject(jlong handle, void* obj) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_nativeObjects[handle] = obj;
}

void JniMemoryManager::UnregisterNativeObject(jlong handle) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_nativeObjects.erase(handle);
}

void* JniMemoryManager::GetNativeObject(jlong handle) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_nativeObjects.find(handle);
    if (it != m_nativeObjects.end()) {
        return it->second;
    }
    return nullptr;
}

size_t JniMemoryManager::GetAllocatedCount() const {
    return m_allocations.size();
}

size_t JniMemoryManager::GetAllocatedSize() const {
    return m_totalAllocated;
}

void JniMemoryManager::CollectGarbage() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto& pair : m_destructors) {
        pair.second(pair.first);
    }
    m_destructors.clear();
    
    for (auto& pair : m_allocations) {
        std::free(pair.first);
    }
    m_allocations.clear();
    m_nativeObjects.clear();
    m_totalAllocated = 0;
}

void JniMemoryManager::SetMemoryLimit(size_t limit) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_memoryLimit = limit;
}

bool JniMemoryManager::IsMemoryLimitExceeded() const {
    return m_limitExceeded;
}

JniNativeObjectBase::JniNativeObjectBase()
    : m_handle(JniMemoryManager::GetInstance()->ToJLong(this))
    , m_refCount(1) {
    JniMemoryManager::GetInstance()->RegisterNativeObject(m_handle, this);
}

JniNativeObjectBase::~JniNativeObjectBase() {
    JniMemoryManager::GetInstance()->UnregisterNativeObject(m_handle);
}

void JniNativeObjectBase::AddRef() {
    m_refCount++;
}

void JniNativeObjectBase::Release() {
    if (--m_refCount == 0) {
        OnDestroy();
        delete this;
    }
}

}  
}  

#endif
