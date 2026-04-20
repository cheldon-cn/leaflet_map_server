package cn.cycle.echart.workspace;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.time.Instant;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * 工作区恢复机制。
 * 
 * <p>负责工作区的崩溃恢复和自动保存恢复。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class WorkspaceRecovery {

    private final WorkspaceManager workspaceManager;
    private final WorkspacePersister persister;
    private final List<RecoveryListener> listeners;
    private final List<RecoveryPoint> recoveryPoints;
    
    private boolean autoRecoveryEnabled = true;
    private int maxRecoveryPoints = 10;
    private Path recoveryDirectory;

    public WorkspaceRecovery(WorkspaceManager workspaceManager, WorkspacePersister persister) {
        this.workspaceManager = Objects.requireNonNull(workspaceManager, "workspaceManager cannot be null");
        this.persister = Objects.requireNonNull(persister, "persister cannot be null");
        this.listeners = new ArrayList<>();
        this.recoveryPoints = new ArrayList<>();
        this.recoveryDirectory = Paths.get(System.getProperty("user.home"), ".echart", "recovery");
    }

    public void createRecoveryPoint() {
        Workspace workspace = workspaceManager.getCurrentWorkspace();
        if (workspace == null) {
            return;
        }

        RecoveryPoint point = new RecoveryPoint(
                workspace.getId(),
                Instant.now(),
                workspace.getName()
        );

        try {
            Path recoveryFile = recoveryDirectory.resolve(
                    "recovery_" + point.getId() + ".ws");
            
            if (!Files.exists(recoveryDirectory)) {
                Files.createDirectories(recoveryDirectory);
            }
            
            persister.save(workspace, recoveryFile.toFile());
            point.setFilePath(recoveryFile);

            recoveryPoints.add(point);
            trimRecoveryPoints();

            notifyRecoveryPointCreated(point);
        } catch (IOException e) {
            notifyRecoveryFailed(point, e);
        }
    }

    public Workspace recoverFromLatest() throws IOException {
        if (recoveryPoints.isEmpty()) {
            return null;
        }

        RecoveryPoint latest = recoveryPoints.get(recoveryPoints.size() - 1);
        return recoverFromPoint(latest);
    }

    public Workspace recoverFromPoint(RecoveryPoint point) throws IOException {
        if (point == null || point.getFilePath() == null) {
            return null;
        }

        try {
            Workspace workspace = persister.load(point.getFilePath().toFile());
            workspaceManager.setCurrentWorkspace(workspace);
            
            notifyRecovered(point, workspace);
            return workspace;
        } catch (IOException e) {
            notifyRecoveryFailed(point, e);
            throw e;
        }
    }

    public List<RecoveryPoint> getRecoveryPoints() {
        return new ArrayList<>(recoveryPoints);
    }

    public RecoveryPoint getLatestRecoveryPoint() {
        return recoveryPoints.isEmpty() ? null : recoveryPoints.get(recoveryPoints.size() - 1);
    }

    public void clearRecoveryPoints() {
        for (RecoveryPoint point : recoveryPoints) {
            if (point.getFilePath() != null) {
                point.getFilePath().toFile().delete();
            }
        }
        recoveryPoints.clear();
    }

    public void setAutoRecoveryEnabled(boolean enabled) {
        this.autoRecoveryEnabled = enabled;
    }

    public boolean isAutoRecoveryEnabled() {
        return autoRecoveryEnabled;
    }

    public void setMaxRecoveryPoints(int max) {
        this.maxRecoveryPoints = max;
        trimRecoveryPoints();
    }

    public int getMaxRecoveryPoints() {
        return maxRecoveryPoints;
    }

    public void setRecoveryDirectory(Path directory) {
        this.recoveryDirectory = directory;
        if (!directory.toFile().exists()) {
            directory.toFile().mkdirs();
        }
    }

    public Path getRecoveryDirectory() {
        return recoveryDirectory;
    }

    private void trimRecoveryPoints() {
        while (recoveryPoints.size() > maxRecoveryPoints) {
            RecoveryPoint removed = recoveryPoints.remove(0);
            if (removed.getFilePath() != null) {
                removed.getFilePath().toFile().delete();
            }
        }
    }

    public void addListener(RecoveryListener listener) {
        listeners.add(listener);
    }

    public void removeListener(RecoveryListener listener) {
        listeners.remove(listener);
    }

    private void notifyRecoveryPointCreated(RecoveryPoint point) {
        for (RecoveryListener listener : listeners) {
            listener.onRecoveryPointCreated(point);
        }
    }

    private void notifyRecovered(RecoveryPoint point, Workspace workspace) {
        for (RecoveryListener listener : listeners) {
            listener.onRecovered(point, workspace);
        }
    }

    private void notifyRecoveryFailed(RecoveryPoint point, Exception e) {
        for (RecoveryListener listener : listeners) {
            listener.onRecoveryFailed(point, e);
        }
    }

    public interface RecoveryListener {
        void onRecoveryPointCreated(RecoveryPoint point);
        void onRecovered(RecoveryPoint point, Workspace workspace);
        void onRecoveryFailed(RecoveryPoint point, Exception e);
    }

    public static class RecoveryPoint {
        private final String id;
        private final String workspaceId;
        private final Instant timestamp;
        private final String workspaceName;
        private Path filePath;

        public RecoveryPoint(String workspaceId, Instant timestamp, String workspaceName) {
            this.id = java.util.UUID.randomUUID().toString();
            this.workspaceId = workspaceId;
            this.timestamp = timestamp;
            this.workspaceName = workspaceName;
        }

        public String getId() {
            return id;
        }

        public String getWorkspaceId() {
            return workspaceId;
        }

        public Instant getTimestamp() {
            return timestamp;
        }

        public String getWorkspaceName() {
            return workspaceName;
        }

        public Path getFilePath() {
            return filePath;
        }

        public void setFilePath(Path filePath) {
            this.filePath = filePath;
        }
    }
}
