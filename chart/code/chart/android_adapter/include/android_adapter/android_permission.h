#ifndef OGC_ANDROID_ADAPTER_ANDROID_PERMISSION_H
#define OGC_ANDROID_ADAPTER_ANDROID_PERMISSION_H

#include "android_adapter/export.h"

#ifdef __ANDROID__

#include <string>
#include <vector>
#include <functional>
#include <map>

struct ANativeActivity;

namespace ogc {
namespace android {

enum class PermissionStatus {
    kGranted = 0,
    kDenied = 1,
    kDeniedDoNotAskAgain = 2
};

enum class PermissionType {
    kLocation = 0,
    kStorage = 1,
    kCamera = 2,
    kNetwork = 3,
    kPhone = 4,
    kCustom = 100
};

struct PermissionResult {
    std::string name;
    PermissionStatus status;
    bool shouldShowRationale;
};

class OGC_ANDROID_ADAPTER_API AndroidPermission {
public:
    using PermissionCallback = std::function<void(const std::vector<PermissionResult>&)>;
    
    static AndroidPermission* GetInstance();
    
    void Initialize(ANativeActivity* activity);
    
    bool CheckPermission(const std::string& permission) const;
    bool CheckPermission(PermissionType type) const;
    
    void RequestPermission(const std::string& permission, PermissionCallback callback);
    void RequestPermissions(const std::vector<std::string>& permissions, PermissionCallback callback);
    void RequestPermission(PermissionType type, PermissionCallback callback);
    
    bool ShouldShowRequestPermissionRationale(const std::string& permission) const;
    
    void OpenAppSettings();
    
    std::string GetPermissionName(PermissionType type) const;
    PermissionType GetPermissionType(const std::string& name) const;
    
    void OnRequestPermissionsResult(int requestCode, 
                                    const std::vector<std::string>& permissions,
                                    const std::vector<int>& grantResults);
    
    bool HasLocationPermission() const;
    bool HasStoragePermission() const;
    bool HasCameraPermission() const;
    bool HasNetworkPermission() const;

private:
    AndroidPermission();
    ~AndroidPermission();
    
    AndroidPermission(const AndroidPermission&) = delete;
    AndroidPermission& operator=(const AndroidPermission&) = delete;
    
    std::string GetAndroidPermissionName(PermissionType type) const;
    int GenerateRequestCode();
    
    static AndroidPermission* s_instance;
    
    ANativeActivity* m_activity;
    std::map<int, PermissionCallback> m_pendingCallbacks;
    std::map<PermissionType, std::string> m_permissionMap;
    int m_nextRequestCode;
};

}  
}  

#endif

#endif
