package cn.cycle.echart.workspace;

import java.io.BufferedReader;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.Instant;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * 工作区导入器。
 * 
 * <p>从外部文件导入工作区配置。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class WorkspaceImporter {

    private final List<ImportListener> listeners;
    private boolean overwriteExisting = false;

    public WorkspaceImporter() {
        this.listeners = new ArrayList<>();
    }

    public Workspace importFromJson(Path sourcePath) throws IOException {
        Objects.requireNonNull(sourcePath, "sourcePath cannot be null");

        if (!Files.exists(sourcePath)) {
            throw new IOException("File not found: " + sourcePath);
        }

        String content = readFileContent(sourcePath);
        Workspace workspace = parseJsonWorkspace(content);

        notifyImported(workspace, sourcePath);
        return workspace;
    }

    public Workspace importFromXml(Path sourcePath) throws IOException {
        Objects.requireNonNull(sourcePath, "sourcePath cannot be null");

        if (!Files.exists(sourcePath)) {
            throw new IOException("File not found: " + sourcePath);
        }

        String content = readFileContent(sourcePath);
        Workspace workspace = parseXmlWorkspace(content);

        notifyImported(workspace, sourcePath);
        return workspace;
    }

    private String readFileContent(Path path) throws IOException {
        StringBuilder content = new StringBuilder();
        try (BufferedReader reader = Files.newBufferedReader(path)) {
            String line;
            while ((line = reader.readLine()) != null) {
                content.append(line).append("\n");
            }
        }
        return content.toString();
    }

    private Workspace parseJsonWorkspace(String json) {
        String id = extractJsonValue(json, "id");
        String name = extractJsonValue(json, "name");
        String description = extractJsonValue(json, "description");
        String version = extractJsonValue(json, "version");

        Workspace workspace = new Workspace(name);
        if (id != null && !id.isEmpty()) {
            workspace.setId(id);
        }
        workspace.setDescription(description != null ? description : "");
        workspace.setVersion(version != null ? version : "1.0.0");

        WorkspaceConfig config = parseJsonConfig(json);
        workspace.setConfig(config);

        return workspace;
    }

    private WorkspaceConfig parseJsonConfig(String json) {
        WorkspaceConfig config = new WorkspaceConfig();

        String theme = extractJsonValue(json, "theme");
        String language = extractJsonValue(json, "language");
        Double zoomLevel = extractJsonDouble(json, "zoomLevel");
        Double centerLat = extractJsonDouble(json, "centerLatitude");
        Double centerLon = extractJsonDouble(json, "centerLongitude");

        if (theme != null) config.setTheme(theme);
        if (language != null) config.setLanguage(language);
        if (zoomLevel != null) config.setZoomLevel(zoomLevel);
        if (centerLat != null) config.setCenterLatitude(centerLat);
        if (centerLon != null) config.setCenterLongitude(centerLon);

        return config;
    }

    private Workspace parseXmlWorkspace(String xml) {
        String id = extractXmlValue(xml, "id");
        String name = extractXmlValue(xml, "name");
        String description = extractXmlValue(xml, "description");
        String version = extractXmlValue(xml, "version");

        Workspace workspace = new Workspace(name != null ? name : "Imported Workspace");
        if (id != null && !id.isEmpty()) {
            workspace.setId(id);
        }
        workspace.setDescription(description != null ? description : "");
        workspace.setVersion(version != null ? version : "1.0.0");

        WorkspaceConfig config = parseXmlConfig(xml);
        workspace.setConfig(config);

        return workspace;
    }

    private WorkspaceConfig parseXmlConfig(String xml) {
        WorkspaceConfig config = new WorkspaceConfig();

        String theme = extractXmlValue(xml, "theme");
        String language = extractXmlValue(xml, "language");
        String zoomLevelStr = extractXmlValue(xml, "zoomLevel");
        String centerLatStr = extractXmlValue(xml, "centerLatitude");
        String centerLonStr = extractXmlValue(xml, "centerLongitude");

        if (theme != null) config.setTheme(theme);
        if (language != null) config.setLanguage(language);
        if (zoomLevelStr != null) {
            try {
                config.setZoomLevel(Double.parseDouble(zoomLevelStr));
            } catch (NumberFormatException e) {
            }
        }
        if (centerLatStr != null) {
            try {
                config.setCenterLatitude(Double.parseDouble(centerLatStr));
            } catch (NumberFormatException e) {
            }
        }
        if (centerLonStr != null) {
            try {
                config.setCenterLongitude(Double.parseDouble(centerLonStr));
            } catch (NumberFormatException e) {
            }
        }

        return config;
    }

    private String extractJsonValue(String json, String key) {
        Pattern pattern = Pattern.compile("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
        Matcher matcher = pattern.matcher(json);
        if (matcher.find()) {
            return unescapeJson(matcher.group(1));
        }
        return null;
    }

    private Double extractJsonDouble(String json, String key) {
        Pattern pattern = Pattern.compile("\"" + key + "\"\\s*:\\s*([0-9.]+)");
        Matcher matcher = pattern.matcher(json);
        if (matcher.find()) {
            try {
                return Double.parseDouble(matcher.group(1));
            } catch (NumberFormatException e) {
                return null;
            }
        }
        return null;
    }

    private String extractXmlValue(String xml, String tag) {
        Pattern pattern = Pattern.compile("<" + tag + ">([^<]*)</" + tag + ">");
        Matcher matcher = pattern.matcher(xml);
        if (matcher.find()) {
            return unescapeXml(matcher.group(1));
        }
        return null;
    }

    private String unescapeJson(String value) {
        if (value == null) return null;
        return value.replace("\\\"", "\"")
                .replace("\\n", "\n")
                .replace("\\r", "\r")
                .replace("\\t", "\t")
                .replace("\\\\", "\\");
    }

    private String unescapeXml(String value) {
        if (value == null) return null;
        return value.replace("&apos;", "'")
                .replace("&quot;", "\"")
                .replace("&gt;", ">")
                .replace("&lt;", "<")
                .replace("&amp;", "&");
    }

    public void setOverwriteExisting(boolean overwrite) {
        this.overwriteExisting = overwrite;
    }

    public boolean isOverwriteExisting() {
        return overwriteExisting;
    }

    public void addListener(ImportListener listener) {
        listeners.add(listener);
    }

    public void removeListener(ImportListener listener) {
        listeners.remove(listener);
    }

    private void notifyImported(Workspace workspace, Path sourcePath) {
        for (ImportListener listener : listeners) {
            listener.onImported(workspace, sourcePath);
        }
    }

    public interface ImportListener {
        void onImported(Workspace workspace, Path sourcePath);
    }
}
