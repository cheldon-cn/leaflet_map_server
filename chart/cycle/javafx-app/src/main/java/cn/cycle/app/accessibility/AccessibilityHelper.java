package cn.cycle.app.accessibility;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.scene.Node;
import javafx.scene.control.Control;
import javafx.scene.control.Label;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;
import javafx.scene.Parent;

import java.util.List;

public class AccessibilityHelper extends AbstractLifecycleComponent {
    
    private static final AccessibilityHelper INSTANCE = new AccessibilityHelper();
    
    private AccessibilityHelper() {
    }
    
    public static AccessibilityHelper getInstance() {
        return INSTANCE;
    }
    
    public void setAccessibleText(Node node, String text) {
        if (node == null) {
            return;
        }
        node.setAccessibleText(text);
    }
    
    public void setAccessibleHelp(Node node, String help) {
        if (node == null) {
            return;
        }
        node.setAccessibleHelp(help);
    }
    
    public void setAccessibleRole(Node node, String role) {
        if (node == null) {
            return;
        }
        node.setAccessibleRoleDescription(role);
    }
    
    public void setupButton(Button button, String text, String description) {
        if (button == null) {
            return;
        }
        button.setAccessibleText(text);
        button.setAccessibleHelp(description);
        button.setFocusTraversable(true);
    }
    
    public void setupTextField(TextField textField, String label, String placeholder) {
        if (textField == null) {
            return;
        }
        textField.setAccessibleText(label);
        textField.setAccessibleHelp(placeholder);
        textField.setFocusTraversable(true);
    }
    
    public void setupLabel(Label label, String text) {
        if (label == null) {
            return;
        }
        label.setAccessibleText(text);
        label.setLabelFor(label);
    }
    
    public void setFocusTraversable(Node node, boolean traversable) {
        if (node == null) {
            return;
        }
        node.setFocusTraversable(traversable);
    }
    
    public void requestFocus(Node node) {
        if (node != null && node.isFocusTraversable()) {
            node.requestFocus();
        }
    }
    
    public void announceForAccessibility(Node node, String message) {
        if (node == null) {
            return;
        }
        node.setAccessibleText(message);
    }
    
    public void setupContainer(Parent container) {
        if (container == null) {
            return;
        }
        
        for (Node child : container.getChildrenUnmodifiable()) {
            setupNodeAccessibility(child);
        }
    }
    
    private void setupNodeAccessibility(Node node) {
        if (node instanceof Button) {
            Button button = (Button) node;
            if (button.getAccessibleText() == null) {
                button.setAccessibleText(button.getText());
            }
            button.setFocusTraversable(true);
        } else if (node instanceof TextField) {
            TextField textField = (TextField) node;
            textField.setFocusTraversable(true);
        } else if (node instanceof Label) {
            Label label = (Label) node;
            if (label.getAccessibleText() == null) {
                label.setAccessibleText(label.getText());
            }
        } else if (node instanceof Parent) {
            setupContainer((Parent) node);
        }
    }
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
    }
}
