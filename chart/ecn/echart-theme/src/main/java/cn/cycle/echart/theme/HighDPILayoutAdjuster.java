package cn.cycle.echart.theme;

import javafx.scene.Node;
import javafx.scene.control.Control;
import javafx.scene.control.Labeled;
import javafx.scene.control.TextInputControl;
import javafx.scene.layout.Region;
import javafx.scene.text.Font;

import java.util.ArrayList;
import java.util.List;

/**
 * 高DPI布局调整器。
 * 
 * <p>根据DPI缩放因子自动调整UI组件的尺寸和间距。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class HighDPILayoutAdjuster {

    private final HighDPIScaler scaler;
    private final List<LayoutAdjustmentRule> rules;
    private boolean enabled;

    public HighDPILayoutAdjuster(HighDPIScaler scaler) {
        this.scaler = scaler;
        this.rules = new ArrayList<>();
        this.enabled = true;
        
        initializeDefaultRules();
    }

    private void initializeDefaultRules() {
        rules.add(new LayoutAdjustmentRule("button", 8.0, 24.0, 32.0));
        rules.add(new LayoutAdjustmentRule("label", 4.0, 12.0, 0.0));
        rules.add(new LayoutAdjustmentRule("textfield", 6.0, 12.0, 28.0));
        rules.add(new LayoutAdjustmentRule("combobox", 6.0, 12.0, 28.0));
        rules.add(new LayoutAdjustmentRule("table", 4.0, 12.0, 28.0));
        rules.add(new LayoutAdjustmentRule("tree", 4.0, 12.0, 24.0));
        rules.add(new LayoutAdjustmentRule("menu", 4.0, 12.0, 28.0));
        rules.add(new LayoutAdjustmentRule("toolbar", 4.0, 12.0, 32.0));
        rules.add(new LayoutAdjustmentRule("panel", 8.0, 12.0, 0.0));
    }

    public void adjust(Node node) {
        if (!enabled || node == null) {
            return;
        }
        
        double scale = scaler.getScale();
        
        if (scale == 1.0) {
            return;
        }
        
        adjustNode(node, scale);
    }

    private void adjustNode(Node node, double scale) {
        String nodeType = determineNodeType(node);
        LayoutAdjustmentRule rule = findRule(nodeType);
        
        if (rule != null) {
            applyRule(node, rule, scale);
        }
        
        adjustFont(node, scale);
        adjustPadding(node, scale);
        
        if (node instanceof Region) {
            adjustRegion((Region) node, scale);
        }
        
        if (node instanceof Control) {
            adjustControl((Control) node, scale);
        }
    }

    private String determineNodeType(Node node) {
        String className = node.getClass().getSimpleName().toLowerCase();
        
        if (className.contains("button")) {
            return "button";
        } else if (className.contains("label")) {
            return "label";
        } else if (className.contains("textfield") || className.contains("textarea")) {
            return "textfield";
        } else if (className.contains("combo")) {
            return "combobox";
        } else if (className.contains("table") || className.contains("list")) {
            return "table";
        } else if (className.contains("tree")) {
            return "tree";
        } else if (className.contains("menu")) {
            return "menu";
        } else if (className.contains("toolbar")) {
            return "toolbar";
        } else if (className.contains("panel") || className.contains("pane")) {
            return "panel";
        }
        
        return "default";
    }

    private LayoutAdjustmentRule findRule(String nodeType) {
        for (LayoutAdjustmentRule rule : rules) {
            if (rule.getNodeType().equals(nodeType)) {
                return rule;
            }
        }
        return null;
    }

    private void applyRule(Node node, LayoutAdjustmentRule rule, double scale) {
        if (node instanceof Region) {
            Region region = (Region) node;
            
            if (rule.getMinHeight() > 0) {
                region.setMinHeight(scaler.scaleValue(rule.getMinHeight()));
            }
        }
    }

    private void adjustFont(Node node, double scale) {
        if (node instanceof Labeled) {
            Labeled labeled = (Labeled) node;
            Font font = labeled.getFont();
            if (font != null) {
                double newSize = scaler.scaleFont(font.getSize());
                labeled.setFont(Font.font(font.getFamily(), newSize));
            }
        } else if (node instanceof TextInputControl) {
            TextInputControl textInput = (TextInputControl) node;
            Font font = textInput.getFont();
            if (font != null) {
                double newSize = scaler.scaleFont(font.getSize());
                textInput.setFont(Font.font(font.getFamily(), newSize));
            }
        }
    }

    private void adjustPadding(Node node, double scale) {
        if (node instanceof Region) {
            Region region = (Region) node;
            javafx.geometry.Insets padding = region.getPadding();
            if (padding != null && !isDefaultPadding(padding)) {
                region.setPadding(new javafx.geometry.Insets(
                        scaler.scaleValue(padding.getTop()),
                        scaler.scaleValue(padding.getRight()),
                        scaler.scaleValue(padding.getBottom()),
                        scaler.scaleValue(padding.getLeft())
                ));
            }
        }
    }

    private boolean isDefaultPadding(javafx.geometry.Insets padding) {
        return padding.getTop() == 0 && padding.getRight() == 0 
            && padding.getBottom() == 0 && padding.getLeft() == 0;
    }

    private void adjustRegion(Region region, double scale) {
        if (region.getPrefWidth() > 0 && region.getPrefWidth() < 10000) {
            region.setPrefWidth(scaler.scaleValue(region.getPrefWidth()));
        }
        if (region.getPrefHeight() > 0 && region.getPrefHeight() < 10000) {
            region.setPrefHeight(scaler.scaleValue(region.getPrefHeight()));
        }
    }

    private void adjustControl(Control control, double scale) {
        if (control.getMinWidth() > 0 && control.getMinWidth() < 10000) {
            control.setMinWidth(scaler.scaleValue(control.getMinWidth()));
        }
        if (control.getMinHeight() > 0 && control.getMinHeight() < 10000) {
            control.setMinHeight(scaler.scaleValue(control.getMinHeight()));
        }
    }

    public void adjustAll(Node... nodes) {
        for (Node node : nodes) {
            adjust(node);
        }
    }

    public void adjustChildren(Region parent) {
        if (!enabled || parent == null) {
            return;
        }
        
        for (Node child : parent.getChildrenUnmodifiable()) {
            adjust(child);
        }
    }

    public void addRule(LayoutAdjustmentRule rule) {
        rules.add(rule);
    }

    public void removeRule(String nodeType) {
        rules.removeIf(rule -> rule.getNodeType().equals(nodeType));
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    public boolean isEnabled() {
        return enabled;
    }

    public HighDPIScaler getScaler() {
        return scaler;
    }

    public static class LayoutAdjustmentRule {
        private final String nodeType;
        private final double padding;
        private final double fontSize;
        private final double minHeight;

        public LayoutAdjustmentRule(String nodeType, double padding, double fontSize, double minHeight) {
            this.nodeType = nodeType;
            this.padding = padding;
            this.fontSize = fontSize;
            this.minHeight = minHeight;
        }

        public String getNodeType() {
            return nodeType;
        }

        public double getPadding() {
            return padding;
        }

        public double getFontSize() {
            return fontSize;
        }

        public double getMinHeight() {
            return minHeight;
        }
    }
}
