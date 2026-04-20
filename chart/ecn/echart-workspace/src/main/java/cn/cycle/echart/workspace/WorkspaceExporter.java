package cn.cycle.echart.workspace;

import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.Instant;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * 工作区导出器。
 * 
 * <p>将工作区导出为可共享的格式。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class WorkspaceExporter {

    private final List<ExportListener> listeners;

    public WorkspaceExporter() {
        this.listeners = new ArrayList<>();
    }

    public void exportToJson(Workspace workspace, Path targetPath) throws IOException {
        Objects.requireNonNull(workspace, "workspace cannot be null");
        Objects.requireNonNull(targetPath, "targetPath cannot be null");

        StringBuilder json = new StringBuilder();
        json.append("{\n");
        json.append("  \"id\": \"").append(escapeJson(workspace.getId())).append("\",\n");
        json.append("  \"name\": \"").append(escapeJson(workspace.getName())).append("\",\n");
        json.append("  \"description\": \"").append(escapeJson(workspace.getDescription())).append("\",\n");
        json.append("  \"createdAt\": \"").append(workspace.getCreatedTime()).append("\",\n");
        json.append("  \"modifiedAt\": \"").append(workspace.getModifiedTime()).append("\",\n");
        json.append("  \"exportedAt\": \"").append(Instant.now()).append("\",\n");
        json.append("  \"version\": \"").append(workspace.getVersion()).append("\",\n");
        json.append("  \"config\": ").append(configToJson(workspace.getConfig())).append("\n");
        json.append("}\n");

        try (BufferedWriter writer = Files.newBufferedWriter(targetPath)) {
            writer.write(json.toString());
        }

        notifyExported(workspace, targetPath);
    }

    public void exportToXml(Workspace workspace, Path targetPath) throws IOException {
        Objects.requireNonNull(workspace, "workspace cannot be null");
        Objects.requireNonNull(targetPath, "targetPath cannot be null");

        StringBuilder xml = new StringBuilder();
        xml.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        xml.append("<workspace>\n");
        xml.append("  <id>").append(escapeXml(workspace.getId())).append("</id>\n");
        xml.append("  <name>").append(escapeXml(workspace.getName())).append("</name>\n");
        xml.append("  <description>").append(escapeXml(workspace.getDescription())).append("</description>\n");
        xml.append("  <createdAt>").append(workspace.getCreatedTime()).append("</createdAt>\n");
        xml.append("  <modifiedAt>").append(workspace.getModifiedTime()).append("</modifiedAt>\n");
        xml.append("  <exportedAt>").append(Instant.now()).append("</exportedAt>\n");
        xml.append("  <version>").append(workspace.getVersion()).append("</version>\n");
        xml.append("  <config>").append(configToXml(workspace.getConfig())).append("</config>\n");
        xml.append("</workspace>\n");

        try (BufferedWriter writer = Files.newBufferedWriter(targetPath)) {
            writer.write(xml.toString());
        }

        notifyExported(workspace, targetPath);
    }

    private String configToJson(WorkspaceConfig config) {
        if (config == null) {
            return "{}";
        }
        StringBuilder sb = new StringBuilder();
        sb.append("{\n");
        sb.append("    \"theme\": \"").append(escapeJson(config.getTheme())).append("\",\n");
        sb.append("    \"language\": \"").append(escapeJson(config.getLanguage())).append("\",\n");
        sb.append("    \"zoomLevel\": ").append(config.getZoomLevel()).append(",\n");
        sb.append("    \"centerLatitude\": ").append(config.getCenterLatitude()).append(",\n");
        sb.append("    \"centerLongitude\": ").append(config.getCenterLongitude()).append("\n");
        sb.append("  }");
        return sb.toString();
    }

    private String configToXml(WorkspaceConfig config) {
        if (config == null) {
            return "";
        }
        StringBuilder sb = new StringBuilder();
        sb.append("<theme>").append(escapeXml(config.getTheme())).append("</theme>");
        sb.append("<language>").append(escapeXml(config.getLanguage())).append("</language>");
        sb.append("<zoomLevel>").append(config.getZoomLevel()).append("</zoomLevel>");
        sb.append("<centerLatitude>").append(config.getCenterLatitude()).append("</centerLatitude>");
        sb.append("<centerLongitude>").append(config.getCenterLongitude()).append("</centerLongitude>");
        return sb.toString();
    }

    private String escapeJson(String value) {
        if (value == null) return "";
        return value.replace("\\", "\\\\")
                .replace("\"", "\\\"")
                .replace("\n", "\\n")
                .replace("\r", "\\r")
                .replace("\t", "\\t");
    }

    private String escapeXml(String value) {
        if (value == null) return "";
        return value.replace("&", "&amp;")
                .replace("<", "&lt;")
                .replace(">", "&gt;")
                .replace("\"", "&quot;")
                .replace("'", "&apos;");
    }

    public void addListener(ExportListener listener) {
        listeners.add(listener);
    }

    public void removeListener(ExportListener listener) {
        listeners.remove(listener);
    }

    private void notifyExported(Workspace workspace, Path targetPath) {
        for (ExportListener listener : listeners) {
            listener.onExported(workspace, targetPath);
        }
    }

    public interface ExportListener {
        void onExported(Workspace workspace, Path targetPath);
    }
}
