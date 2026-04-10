package cn.cycle.app.util;

import javafx.scene.Node;
import javafx.scene.control.Control;
import javafx.scene.control.Tooltip;
import javafx.scene.AccessibleRole;

public class AccessibilityHelper {
    
    public static void setupAccessible(Node node, String name, String description) {
        node.setAccessibleRole(AccessibleRole.NODE);
        node.setAccessibleText(name);
        node.setAccessibleHelp(description);
    }
    
    public static void setupButton(Control button, String text, String description) {
        button.setAccessibleRole(AccessibleRole.BUTTON);
        button.setAccessibleText(text);
        button.setAccessibleHelp(description);
        button.setTooltip(new Tooltip(description));
    }
    
    public static void setupTextField(Control textField, String name, String value) {
        textField.setAccessibleRole(AccessibleRole.TEXT_FIELD);
        textField.setAccessibleText(name + ": " + value);
    }
    
    public static void setupChartCanvas(Node canvas) {
        canvas.setAccessibleRole(AccessibleRole.IMAGE_VIEW);
        canvas.setAccessibleText(I18nManager.tr("canvas.name"));
        canvas.setAccessibleHelp(I18nManager.tr("canvas.description"));
    }
    
    public static void announce(String message) {
        javafx.application.Platform.runLater(() -> {
            System.out.println("Accessibility: " + message);
        });
    }
    
    public static void focusAndAnnounce(Node node, String message) {
        node.requestFocus();
        announce(message);
    }
}
