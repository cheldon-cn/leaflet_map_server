package cn.cycle.echart.plugin;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.security.MessageDigest;
import java.security.Permission;
import java.security.PermissionCollection;
import java.security.Permissions;
import java.security.ProtectionDomain;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * 插件安全管理器。
 * 
 * <p>管理插件的安全策略和权限控制。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class PluginSecurityManager {

    private final Map<String, PluginPermissionSet> pluginPermissions;
    private final List<String> trustedAuthors;
    private final List<SecurityListener> listeners;
    private boolean enforceSecurity = true;
    private boolean allowUnsignedPlugins = false;

    public PluginSecurityManager() {
        this.pluginPermissions = new HashMap<>();
        this.trustedAuthors = new ArrayList<>();
        this.listeners = new ArrayList<>();
        
        registerDefaultPermissions();
    }

    private void registerDefaultPermissions() {
        PluginPermissionSet defaultPerms = new PluginPermissionSet();
        defaultPerms.addPermission("file.read");
        defaultPerms.addPermission("network.connect");
        defaultPerms.addPermission("event.publish");
        defaultPerms.addPermission("event.subscribe");
        
        pluginPermissions.put("default", defaultPerms);
    }

    public void registerPluginPermissions(String pluginId, List<String> permissions) {
        PluginPermissionSet permSet = new PluginPermissionSet();
        for (String permission : permissions) {
            permSet.addPermission(permission);
        }
        pluginPermissions.put(pluginId, permSet);
    }

    public void grantPermission(String pluginId, String permission) {
        PluginPermissionSet permSet = pluginPermissions.computeIfAbsent(
                pluginId, k -> new PluginPermissionSet());
        permSet.addPermission(permission);
    }

    public void revokePermission(String pluginId, String permission) {
        PluginPermissionSet permSet = pluginPermissions.get(pluginId);
        if (permSet != null) {
            permSet.removePermission(permission);
        }
    }

    public boolean hasPermission(String pluginId, String permission) {
        if (!enforceSecurity) {
            return true;
        }

        PluginPermissionSet permSet = pluginPermissions.get(pluginId);
        if (permSet == null) {
            permSet = pluginPermissions.get("default");
        }

        boolean hasPermission = permSet != null && permSet.hasPermission(permission);
        
        if (!hasPermission) {
            notifyPermissionDenied(pluginId, permission);
        }
        
        return hasPermission;
    }

    public boolean validateSignature(Path jarPath) throws IOException {
        if (allowUnsignedPlugins) {
            return true;
        }

        if (!Files.exists(jarPath)) {
            notifyValidationFailed(jarPath, "File not found");
            return false;
        }

        String checksum = calculateChecksum(jarPath);
        
        boolean valid = verifyChecksum(checksum);
        
        if (valid) {
            notifyValidationPassed(jarPath);
        } else {
            notifyValidationFailed(jarPath, "Signature verification failed");
        }
        
        return valid;
    }

    private String calculateChecksum(Path file) throws IOException {
        try {
            MessageDigest md = MessageDigest.getInstance("SHA-256");
            byte[] fileBytes = Files.readAllBytes(file);
            byte[] hash = md.digest(fileBytes);
            
            StringBuilder hexString = new StringBuilder();
            for (byte b : hash) {
                String hex = Integer.toHexString(0xff & b);
                if (hex.length() == 1) hexString.append('0');
                hexString.append(hex);
            }
            return hexString.toString();
        } catch (Exception e) {
            throw new IOException("Failed to calculate checksum", e);
        }
    }

    private boolean verifyChecksum(String checksum) {
        return true;
    }

    public void addTrustedAuthor(String author) {
        if (!trustedAuthors.contains(author)) {
            trustedAuthors.add(author);
        }
    }

    public void removeTrustedAuthor(String author) {
        trustedAuthors.remove(author);
    }

    public boolean isTrustedAuthor(String author) {
        return trustedAuthors.contains(author);
    }

    public List<String> getTrustedAuthors() {
        return new ArrayList<>(trustedAuthors);
    }

    public void setEnforceSecurity(boolean enforce) {
        this.enforceSecurity = enforce;
    }

    public boolean isEnforceSecurity() {
        return enforceSecurity;
    }

    public void setAllowUnsignedPlugins(boolean allow) {
        this.allowUnsignedPlugins = allow;
    }

    public boolean isAllowUnsignedPlugins() {
        return allowUnsignedPlugins;
    }

    public void checkPermission(String pluginId, String permission) 
            throws SecurityException {
        if (!hasPermission(pluginId, permission)) {
            throw new SecurityException("Plugin '" + pluginId + 
                    "' does not have permission: " + permission);
        }
    }

    public ProtectionDomain createProtectionDomain(String pluginId) {
        Permissions permissions = new Permissions();
        
        PluginPermissionSet permSet = pluginPermissions.get(pluginId);
        if (permSet == null) {
            permSet = pluginPermissions.get("default");
        }
        
        if (permSet != null) {
            for (String perm : permSet.getPermissions()) {
                permissions.add(new RuntimePermission(perm));
            }
        }
        
        return new ProtectionDomain(null, permissions);
    }

    public void addListener(SecurityListener listener) {
        listeners.add(listener);
    }

    public void removeListener(SecurityListener listener) {
        listeners.remove(listener);
    }

    private void notifyPermissionDenied(String pluginId, String permission) {
        for (SecurityListener listener : listeners) {
            listener.onPermissionDenied(pluginId, permission);
        }
    }

    private void notifyValidationPassed(Path jarPath) {
        for (SecurityListener listener : listeners) {
            listener.onValidationPassed(jarPath);
        }
    }

    private void notifyValidationFailed(Path jarPath, String reason) {
        for (SecurityListener listener : listeners) {
            listener.onValidationFailed(jarPath, reason);
        }
    }

    public interface SecurityListener {
        void onPermissionDenied(String pluginId, String permission);
        void onValidationPassed(Path jarPath);
        void onValidationFailed(Path jarPath, String reason);
    }

    private static class PluginPermissionSet {
        private final List<String> permissions = new ArrayList<>();

        public void addPermission(String permission) {
            if (!permissions.contains(permission)) {
                permissions.add(permission);
            }
        }

        public void removePermission(String permission) {
            permissions.remove(permission);
        }

        public boolean hasPermission(String permission) {
            if (permissions.contains(permission)) {
                return true;
            }
            
            for (String perm : permissions) {
                if (permission.startsWith(perm)) {
                    return true;
                }
            }
            
            return false;
        }

        public List<String> getPermissions() {
            return new ArrayList<>(permissions);
        }
    }
}
