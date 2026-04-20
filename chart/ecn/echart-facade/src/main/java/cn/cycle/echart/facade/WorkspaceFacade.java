package cn.cycle.echart.facade;

import cn.cycle.echart.core.EventBus;
import cn.cycle.echart.workspace.ErrorHandler;
import cn.cycle.echart.workspace.Panel;
import cn.cycle.echart.workspace.PanelLayout;
import cn.cycle.echart.workspace.PanelManager;
import cn.cycle.echart.workspace.Workspace;
import cn.cycle.echart.workspace.WorkspaceExporter;
import cn.cycle.echart.workspace.WorkspaceImporter;
import cn.cycle.echart.workspace.WorkspaceManager;
import cn.cycle.echart.workspace.WorkspacePersister;
import cn.cycle.echart.workspace.WorkspaceRecovery;

import java.io.IOException;
import java.nio.file.Path;
import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * 工作区业务门面。
 * 
 * <p>提供工作区系统的统一访问接口。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class WorkspaceFacade {

    private final WorkspaceManager workspaceManager;
    private final WorkspacePersister persister;
    private final WorkspaceRecovery recovery;
    private final WorkspaceExporter exporter;
    private final WorkspaceImporter importer;
    private final PanelManager panelManager;
    private final EventBus eventBus;

    public WorkspaceFacade(WorkspaceManager workspaceManager, PanelManager panelManager,
                           EventBus eventBus) {
        this.workspaceManager = Objects.requireNonNull(workspaceManager, "workspaceManager cannot be null");
        this.panelManager = Objects.requireNonNull(panelManager, "panelManager cannot be null");
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.persister = new WorkspacePersister();
        this.recovery = new WorkspaceRecovery(workspaceManager, persister);
        this.exporter = new WorkspaceExporter();
        this.importer = new WorkspaceImporter();
    }

    public Workspace createWorkspace(String name) {
        return workspaceManager.createWorkspace(name);
    }

    public Workspace createWorkspace(String name, String description) {
        Workspace workspace = workspaceManager.createWorkspace(name);
        workspace.setDescription(description);
        return workspace;
    }

    public void deleteWorkspace(String workspaceId) {
        workspaceManager.removeWorkspace(workspaceId);
    }

    public Workspace getWorkspace(String workspaceId) {
        return workspaceManager.getWorkspace(workspaceId);
    }

    public List<Workspace> getAllWorkspaces() {
        return workspaceManager.getAllWorkspaces();
    }

    public Workspace getCurrentWorkspace() {
        return workspaceManager.getCurrentWorkspace();
    }

    public void setCurrentWorkspace(String workspaceId) {
        Workspace workspace = workspaceManager.getWorkspace(workspaceId);
        if (workspace != null) {
            workspaceManager.setCurrentWorkspace(workspace);
        }
    }

    public void saveWorkspace(String workspaceId) throws IOException {
        Workspace workspace = workspaceManager.getWorkspace(workspaceId);
        if (workspace != null && workspace.getFilePath() != null) {
            persister.save(workspace, new java.io.File(workspace.getFilePath()));
        }
    }

    public void saveWorkspace(String workspaceId, Path filePath) throws IOException {
        Workspace workspace = workspaceManager.getWorkspace(workspaceId);
        if (workspace != null) {
            persister.save(workspace, filePath.toFile());
        }
    }

    public Workspace loadWorkspace(Path filePath) throws IOException {
        Workspace workspace = persister.load(filePath.toFile());
        if (workspace != null) {
            workspaceManager.addWorkspace(workspace);
        }
        return workspace;
    }

    public void exportToJson(String workspaceId, Path targetPath) throws IOException {
        Workspace workspace = workspaceManager.getWorkspace(workspaceId);
        if (workspace != null) {
            exporter.exportToJson(workspace, targetPath);
        }
    }

    public void exportToXml(String workspaceId, Path targetPath) throws IOException {
        Workspace workspace = workspaceManager.getWorkspace(workspaceId);
        if (workspace != null) {
            exporter.exportToXml(workspace, targetPath);
        }
    }

    public Workspace importFromJson(Path sourcePath) throws IOException {
        Workspace workspace = importer.importFromJson(sourcePath);
        if (workspace != null) {
            workspaceManager.addWorkspace(workspace);
        }
        return workspace;
    }

    public Workspace importFromXml(Path sourcePath) throws IOException {
        Workspace workspace = importer.importFromXml(sourcePath);
        if (workspace != null) {
            workspaceManager.addWorkspace(workspace);
        }
        return workspace;
    }

    public void createRecoveryPoint() {
        recovery.createRecoveryPoint();
    }

    public Workspace recoverFromLatest() throws IOException {
        return recovery.recoverFromLatest();
    }

    public List<WorkspaceRecovery.RecoveryPoint> getRecoveryPoints() {
        return recovery.getRecoveryPoints();
    }

    public void clearRecoveryPoints() {
        recovery.clearRecoveryPoints();
    }

    public void setAutoRecoveryEnabled(boolean enabled) {
        recovery.setAutoRecoveryEnabled(enabled);
    }

    public Panel registerPanel(String id, String name, PanelManager.PanelType type) {
        return panelManager.registerPanel(id, name, type);
    }

    public void unregisterPanel(String id) {
        panelManager.unregisterPanel(id);
    }

    public Panel getPanel(String id) {
        return panelManager.getPanel(id);
    }

    public List<Panel> getAllPanels() {
        return panelManager.getAllPanels();
    }

    public void showPanel(String id) {
        panelManager.showPanel(id);
    }

    public void hidePanel(String id) {
        panelManager.hidePanel(id);
    }

    public void togglePanel(String id) {
        panelManager.togglePanel(id);
    }

    public void setPanelPosition(String id, PanelManager.PanelPosition position) {
        panelManager.setPanelPosition(id, position);
    }

    public void setPanelSize(String id, double width, double height) {
        panelManager.setPanelSize(id, width, height);
    }

    public PanelLayout getCurrentLayout() {
        return panelManager.getCurrentLayout();
    }

    public void saveCurrentLayout(String name) {
        panelManager.saveCurrentLayout(name);
    }

    public void applyLayout(PanelLayout layout) {
        panelManager.applyLayout(layout);
    }

    public void updateWorkspaceConfig(String workspaceId, Map<String, Object> config) {
        Workspace workspace = workspaceManager.getWorkspace(workspaceId);
        if (workspace != null && workspace.getConfig() != null) {
            if (config.containsKey("theme")) {
                workspace.getConfig().setTheme((String) config.get("theme"));
            }
            if (config.containsKey("language")) {
                workspace.getConfig().setLanguage((String) config.get("language"));
            }
            if (config.containsKey("zoomLevel")) {
                workspace.getConfig().setZoomLevel((Double) config.get("zoomLevel"));
            }
            if (config.containsKey("centerLatitude")) {
                workspace.getConfig().setCenterLatitude((Double) config.get("centerLatitude"));
            }
            if (config.containsKey("centerLongitude")) {
                workspace.getConfig().setCenterLongitude((Double) config.get("centerLongitude"));
            }
            workspaceManager.updateWorkspace(workspace);
        }
    }

    public void resetWorkspaceConfig(String workspaceId) {
        Workspace workspace = workspaceManager.getWorkspace(workspaceId);
        if (workspace != null) {
            workspace.setConfig(new cn.cycle.echart.workspace.WorkspaceConfig());
            workspaceManager.updateWorkspace(workspace);
        }
    }

    public int getWorkspaceCount() {
        return workspaceManager.getWorkspaceCount();
    }

    public boolean hasUnsavedChanges(String workspaceId) {
        Workspace workspace = workspaceManager.getWorkspace(workspaceId);
        return workspace != null && workspace.isDirty();
    }

    public void markClean(String workspaceId) {
        Workspace workspace = workspaceManager.getWorkspace(workspaceId);
        if (workspace != null) {
            workspace.setDirty(false);
        }
    }
}
