package cn.cycle.app.focus;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.control.Control;
import javafx.scene.input.KeyCode;

import java.util.ArrayList;
import java.util.List;

public class FocusManager extends AbstractLifecycleComponent {
    
    private static final FocusManager INSTANCE = new FocusManager();
    
    private final List<Node> focusableNodes = new ArrayList<>();
    private int currentFocusIndex = -1;
    private boolean wrapAround = true;
    
    private FocusManager() {
    }
    
    public static FocusManager getInstance() {
        return INSTANCE;
    }
    
    public void registerFocusable(Node node) {
        if (node != null && !focusableNodes.contains(node)) {
            focusableNodes.add(node);
            node.setFocusTraversable(true);
        }
    }
    
    public void unregisterFocusable(Node node) {
        focusableNodes.remove(node);
    }
    
    public void clearFocusables() {
        focusableNodes.clear();
        currentFocusIndex = -1;
    }
    
    public void focusNext() {
        if (focusableNodes.isEmpty()) {
            return;
        }
        
        int nextIndex = currentFocusIndex + 1;
        if (nextIndex >= focusableNodes.size()) {
            if (wrapAround) {
                nextIndex = 0;
            } else {
                return;
            }
        }
        
        focusNode(nextIndex);
    }
    
    public void focusPrevious() {
        if (focusableNodes.isEmpty()) {
            return;
        }
        
        int prevIndex = currentFocusIndex - 1;
        if (prevIndex < 0) {
            if (wrapAround) {
                prevIndex = focusableNodes.size() - 1;
            } else {
                return;
            }
        }
        
        focusNode(prevIndex);
    }
    
    public void focusFirst() {
        if (focusableNodes.isEmpty()) {
            return;
        }
        focusNode(0);
    }
    
    public void focusLast() {
        if (focusableNodes.isEmpty()) {
            return;
        }
        focusNode(focusableNodes.size() - 1);
    }
    
    private void focusNode(int index) {
        if (index < 0 || index >= focusableNodes.size()) {
            return;
        }
        
        Node node = focusableNodes.get(index);
        if (node != null && node.isFocusTraversable() && !node.isDisabled()) {
            node.requestFocus();
            currentFocusIndex = index;
        }
    }
    
    public void focusNode(Node node) {
        int index = focusableNodes.indexOf(node);
        if (index >= 0) {
            focusNode(index);
        }
    }
    
    public Node getCurrentFocusedNode() {
        if (currentFocusIndex >= 0 && currentFocusIndex < focusableNodes.size()) {
            return focusableNodes.get(currentFocusIndex);
        }
        return null;
    }
    
    public int getCurrentFocusIndex() {
        return currentFocusIndex;
    }
    
    public void setWrapAround(boolean wrap) {
        this.wrapAround = wrap;
    }
    
    public boolean isWrapAround() {
        return wrapAround;
    }
    
    public void scanAndRegisterFocusables(Parent root) {
        if (root == null) {
            return;
        }
        
        for (Node child : root.getChildrenUnmodifiable()) {
            if (child instanceof Control && child.isFocusTraversable()) {
                registerFocusable(child);
            }
            
            if (child instanceof Parent) {
                scanAndRegisterFocusables((Parent) child);
            }
        }
    }
    
    public boolean handleKeyNavigation(KeyCode code) {
        if (code == null) {
            return false;
        }
        
        switch (code) {
            case TAB:
                if (wrapAround) {
                    focusNext();
                }
                return true;
            default:
                return false;
        }
    }
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
        focusableNodes.clear();
        currentFocusIndex = -1;
    }
}
