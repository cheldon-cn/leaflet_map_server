package cn.cycle.echart.workspace;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventBus;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * 工作区管理器。
 * 
 * <p>管理工作区的创建、加载、保存和切换。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class WorkspaceManager {

    private final EventBus eventBus;
    private final WorkspacePersister persister;
    private Workspace currentWorkspace;
    private final List<WorkspaceManagerListener> listeners;
    
    private String lastWorkspacePath;
    private boolean autoSave;
    private int autoSaveInterval;

    public WorkspaceManager(EventBus eventBus) {
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.persister = new WorkspacePersister();
        this.currentWorkspace = null;
        this.listeners = new ArrayList<>();
        this.lastWorkspacePath = null;
        this.autoSave = false;
        this.autoSaveInterval = 300;
    }

    /**
     * 创建新工作区。
     * 
     * @param name 工作区名称
     * @return 创建的工作区
     */
    public Workspace createWorkspace(String name) {
        Workspace workspace = new Workspace(name);
        notifyWorkspaceCreated(workspace);
        return workspace;
    }

    /**
     * 加载工作区。
     * 
     * @param file 工作区文件
     * @return 加载的工作区
     * @throws Exception 加载异常
     */
    public Workspace loadWorkspace(File file) throws Exception {
        Objects.requireNonNull(file, "file cannot be null");
        
        Workspace workspace = persister.load(file);
        workspace.setFilePath(file.getAbsolutePath());
        
        lastWorkspacePath = file.getAbsolutePath();
        
        notifyWorkspaceLoaded(workspace);
        
        return workspace;
    }

    /**
     * 保存工作区。
     * 
     * @param workspace 工作区
     * @param file 保存文件
     * @throws Exception 保存异常
     */
    public void saveWorkspace(Workspace workspace, File file) throws Exception {
        Objects.requireNonNull(workspace, "workspace cannot be null");
        Objects.requireNonNull(file, "file cannot be null");
        
        persister.save(workspace, file);
        workspace.setFilePath(file.getAbsolutePath());
        workspace.markClean();
        
        lastWorkspacePath = file.getAbsolutePath();
        
        notifyWorkspaceSaved(workspace);
    }

    /**
     * 保存当前工作区。
     * 
     * @throws Exception 保存异常
     */
    public void saveCurrentWorkspace() throws Exception {
        if (currentWorkspace != null && currentWorkspace.getFilePath() != null) {
            saveWorkspace(currentWorkspace, new File(currentWorkspace.getFilePath()));
        }
    }

    /**
     * 设置当前工作区。
     * 
     * @param workspace 工作区
     */
    public void setCurrentWorkspace(Workspace workspace) {
        Workspace oldWorkspace = this.currentWorkspace;
        this.currentWorkspace = workspace;
        
        notifyWorkspaceChanged(oldWorkspace, workspace);
    }

    /**
     * 获取当前工作区。
     * 
     * @return 当前工作区
     */
    public Workspace getCurrentWorkspace() {
        return currentWorkspace;
    }

    /**
     * 关闭当前工作区。
     */
    public void closeCurrentWorkspace() {
        if (currentWorkspace != null) {
            Workspace closed = currentWorkspace;
            currentWorkspace = null;
            notifyWorkspaceClosed(closed);
        }
    }

    /**
     * 重置当前工作区。
     */
    public void resetCurrentWorkspace() {
        if (currentWorkspace != null) {
            currentWorkspace.reset();
            notifyWorkspaceReset(currentWorkspace);
        }
    }

    /**
     * 获取上次工作区路径。
     * 
     * @return 路径
     */
    public String getLastWorkspacePath() {
        return lastWorkspacePath;
    }

    /**
     * 设置自动保存。
     * 
     * @param autoSave 是否自动保存
     */
    public void setAutoSave(boolean autoSave) {
        this.autoSave = autoSave;
    }

    /**
     * 是否自动保存。
     * 
     * @return 是否自动保存
     */
    public boolean isAutoSave() {
        return autoSave;
    }

    /**
     * 设置自动保存间隔。
     * 
     * @param interval 间隔（秒）
     */
    public void setAutoSaveInterval(int interval) {
        this.autoSaveInterval = interval;
    }

    /**
     * 获取自动保存间隔。
     * 
     * @return 间隔（秒）
     */
    public int getAutoSaveInterval() {
        return autoSaveInterval;
    }

    /**
     * 添加监听器。
     * 
     * @param listener 监听器
     */
    public void addListener(WorkspaceManagerListener listener) {
        listeners.add(listener);
    }

    /**
     * 移除监听器。
     * 
     * @param listener 监听器
     */
    public void removeListener(WorkspaceManagerListener listener) {
        listeners.remove(listener);
    }

    private void notifyWorkspaceCreated(Workspace workspace) {
        eventBus.publish(new AppEvent(this, AppEventType.WORKSPACE_RESET, workspace));
        
        for (WorkspaceManagerListener listener : listeners) {
            listener.onWorkspaceCreated(workspace);
        }
    }

    private void notifyWorkspaceLoaded(Workspace workspace) {
        eventBus.publish(new AppEvent(this, AppEventType.WORKSPACE_LOADED, workspace));
        
        for (WorkspaceManagerListener listener : listeners) {
            listener.onWorkspaceLoaded(workspace);
        }
    }

    private void notifyWorkspaceSaved(Workspace workspace) {
        eventBus.publish(new AppEvent(this, AppEventType.WORKSPACE_SAVED, workspace));
        
        for (WorkspaceManagerListener listener : listeners) {
            listener.onWorkspaceSaved(workspace);
        }
    }

    private void notifyWorkspaceChanged(Workspace oldWorkspace, Workspace newWorkspace) {
        for (WorkspaceManagerListener listener : listeners) {
            listener.onWorkspaceChanged(oldWorkspace, newWorkspace);
        }
    }

    private void notifyWorkspaceClosed(Workspace workspace) {
        for (WorkspaceManagerListener listener : listeners) {
            listener.onWorkspaceClosed(workspace);
        }
    }

    private void notifyWorkspaceReset(Workspace workspace) {
        eventBus.publish(new AppEvent(this, AppEventType.WORKSPACE_RESET, workspace));
        
        for (WorkspaceManagerListener listener : listeners) {
            listener.onWorkspaceReset(workspace);
        }
    }

    /**
     * 工作区管理器监听器接口。
     */
    public interface WorkspaceManagerListener {
        void onWorkspaceCreated(Workspace workspace);
        void onWorkspaceLoaded(Workspace workspace);
        void onWorkspaceSaved(Workspace workspace);
        void onWorkspaceChanged(Workspace oldWorkspace, Workspace newWorkspace);
        void onWorkspaceClosed(Workspace workspace);
        void onWorkspaceReset(Workspace workspace);
    }
}
