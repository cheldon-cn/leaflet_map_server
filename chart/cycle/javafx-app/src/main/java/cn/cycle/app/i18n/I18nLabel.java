package cn.cycle.app.i18n;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.Tooltip;

public class I18nLabel extends Label {
    
    private final StringProperty i18nKey = new SimpleStringProperty();
    
    public I18nLabel() {
        super();
        i18nKey.addListener((obs, oldKey, newKey) -> updateText());
    }
    
    public I18nLabel(String key) {
        this();
        setI18nKey(key);
    }
    
    public String getI18nKey() {
        return i18nKey.get();
    }
    
    public void setI18nKey(String key) {
        i18nKey.set(key);
    }
    
    public StringProperty i18nKeyProperty() {
        return i18nKey;
    }
    
    private void updateText() {
        String key = i18nKey.get();
        if (key != null && !key.isEmpty()) {
            setText(I18nManager.getInstance().get(key));
        }
    }
    
    public void refresh() {
        updateText();
    }
}
