package cn.cycle.echart.ui.dialog;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.stage.Modality;
import javafx.stage.Window;

public class SettingsDialog extends Dialog<ButtonType> {

    private final ListView<String> categoryList;
    private final StackPane contentPane;

    private final CheckBox autoSaveCheckBox;
    private final Spinner<Integer> autoSaveIntervalSpinner;
    private final Spinner<Integer> cacheSizeSpinner;
    private final CheckBox alarmSoundCheckBox;
    private final Spinner<Integer> aisUpdateIntervalSpinner;
    private final ComboBox<String> localeComboBox;

    private boolean autoSave;
    private int autoSaveInterval;
    private int cacheSize;
    private boolean alarmSound;
    private int aisUpdateInterval;
    private String locale;

    public SettingsDialog() {
        this(true, 300, 100, true, 1000, "zh_CN");
    }

    public SettingsDialog(boolean autoSave, int autoSaveInterval, int cacheSize,
                          boolean alarmSound, int aisUpdateInterval, String locale) {
        this.autoSave = autoSave;
        this.autoSaveInterval = autoSaveInterval;
        this.cacheSize = cacheSize;
        this.alarmSound = alarmSound;
        this.aisUpdateInterval = aisUpdateInterval;
        this.locale = locale;

        categoryList = new ListView<>();
        contentPane = new StackPane();
        autoSaveCheckBox = new CheckBox("启用自动保存");
        autoSaveIntervalSpinner = new Spinner<>(60, 3600, autoSaveInterval, 60);
        cacheSizeSpinner = new Spinner<>(10, 500, cacheSize, 10);
        alarmSoundCheckBox = new CheckBox("启用预警声音");
        aisUpdateIntervalSpinner = new Spinner<>(500, 10000, aisUpdateInterval, 500);
        localeComboBox = new ComboBox<>();

        initializeDialog();
    }

    private void initializeDialog() {
        setTitle("系统设置");
        setHeaderText(null);
        initModality(Modality.APPLICATION_MODAL);

        BorderPane mainLayout = new BorderPane();
        mainLayout.setPrefSize(600, 400);

        VBox leftPanel = createCategoryPanel();
        leftPanel.setPrefWidth(150);
        mainLayout.setLeft(leftPanel);

        contentPane.setPadding(new Insets(15));
        mainLayout.setCenter(contentPane);

        categoryList.getItems().addAll("常规", "显示", "预警", "AIS");
        categoryList.getSelectionModel().select(0);
        categoryList.getSelectionModel().selectedItemProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal != null) {
                showCategory(newVal);
            }
        });

        showCategory("常规");

        getDialogPane().setContent(mainLayout);
        getDialogPane().getButtonTypes().addAll(ButtonType.OK, ButtonType.CANCEL);

        setResultConverter(buttonType -> {
            if (buttonType == ButtonType.OK) {
                applySettings();
            }
            return buttonType;
        });

        loadCurrentSettings();
    }

    private VBox createCategoryPanel() {
        VBox panel = new VBox();
        panel.setPadding(new Insets(10));
        panel.setStyle("-fx-background-color: derive(-fx-base, -5%);");

        Label titleLabel = new Label("设置分类");
        titleLabel.setStyle("-fx-font-weight: bold; -fx-padding: 0 0 10 0;");

        categoryList.setCellFactory(param -> new ListCell<String>() {
            @Override
            protected void updateItem(String item, boolean empty) {
                super.updateItem(item, empty);
                if (empty || item == null) {
                    setText(null);
                    setGraphic(null);
                } else {
                    setText(item);
                    setStyle("-fx-padding: 8 12 8 12; -fx-cursor: hand;");
                }
            }
        });

        VBox.setVgrow(categoryList, Priority.ALWAYS);
        panel.getChildren().addAll(titleLabel, categoryList);

        return panel;
    }

    private void showCategory(String category) {
        contentPane.getChildren().clear();

        VBox content = new VBox(15);

        switch (category) {
            case "常规":
                content.getChildren().addAll(createGeneralSection());
                break;
            case "显示":
                content.getChildren().addAll(createDisplaySection());
                break;
            case "预警":
                content.getChildren().addAll(createAlarmSection());
                break;
            case "AIS":
                content.getChildren().addAll(createAisSection());
                break;
        }

        contentPane.getChildren().add(content);
    }

    private VBox createGeneralSection() {
        VBox section = new VBox(15);

        Label titleLabel = new Label("常规设置");
        titleLabel.setStyle("-fx-font-weight: bold; -fx-font-size: 14px;");

        VBox workspaceBox = new VBox(10);
        workspaceBox.setPadding(new Insets(10));
        workspaceBox.setStyle("-fx-background-color: derive(-fx-base, 5%); -fx-background-radius: 4;");

        autoSaveCheckBox.setSelected(autoSave);

        HBox intervalBox = new HBox(10);
        intervalBox.setAlignment(Pos.CENTER_LEFT);
        autoSaveIntervalSpinner.setPrefWidth(100);
        autoSaveIntervalSpinner.setEditable(true);
        autoSaveIntervalSpinner.getValueFactory().setValue(autoSaveInterval);
        intervalBox.getChildren().addAll(new Label("保存间隔 (秒):"), autoSaveIntervalSpinner);

        workspaceBox.getChildren().addAll(autoSaveCheckBox, intervalBox);

        VBox localeBox = new VBox(10);
        localeBox.setPadding(new Insets(10));
        localeBox.setStyle("-fx-background-color: derive(-fx-base, 5%); -fx-background-radius: 4;");

        HBox localeRow = new HBox(10);
        localeRow.setAlignment(Pos.CENTER_LEFT);
        localeComboBox.getItems().addAll("简体中文 (zh_CN)", "English (en_US)");
        localeComboBox.setPrefWidth(200);
        if ("en_US".equals(locale)) {
            localeComboBox.getSelectionModel().select(1);
        } else {
            localeComboBox.getSelectionModel().select(0);
        }
        localeRow.getChildren().addAll(new Label("界面语言:"), localeComboBox);
        localeBox.getChildren().add(localeRow);

        section.getChildren().addAll(titleLabel, workspaceBox, localeBox);
        return section;
    }

    private VBox createDisplaySection() {
        VBox section = new VBox(15);

        Label titleLabel = new Label("显示设置");
        titleLabel.setStyle("-fx-font-weight: bold; -fx-font-size: 14px;");

        VBox cacheBox = new VBox(10);
        cacheBox.setPadding(new Insets(10));
        cacheBox.setStyle("-fx-background-color: derive(-fx-base, 5%); -fx-background-radius: 4;");

        HBox cacheRow = new HBox(10);
        cacheRow.setAlignment(Pos.CENTER_LEFT);
        cacheSizeSpinner.setPrefWidth(100);
        cacheSizeSpinner.setEditable(true);
        cacheSizeSpinner.getValueFactory().setValue(cacheSize);
        cacheRow.getChildren().addAll(new Label("海图缓存大小:"), cacheSizeSpinner, new Label("张"));
        cacheBox.getChildren().add(cacheRow);

        section.getChildren().addAll(titleLabel, cacheBox);
        return section;
    }

    private VBox createAlarmSection() {
        VBox section = new VBox(15);

        Label titleLabel = new Label("预警设置");
        titleLabel.setStyle("-fx-font-weight: bold; -fx-font-size: 14px;");

        VBox soundBox = new VBox(10);
        soundBox.setPadding(new Insets(10));
        soundBox.setStyle("-fx-background-color: derive(-fx-base, 5%); -fx-background-radius: 4;");

        alarmSoundCheckBox.setSelected(alarmSound);
        soundBox.getChildren().add(alarmSoundCheckBox);

        section.getChildren().addAll(titleLabel, soundBox);
        return section;
    }

    private VBox createAisSection() {
        VBox section = new VBox(15);

        Label titleLabel = new Label("AIS设置");
        titleLabel.setStyle("-fx-font-weight: bold; -fx-font-size: 14px;");

        VBox updateBox = new VBox(10);
        updateBox.setPadding(new Insets(10));
        updateBox.setStyle("-fx-background-color: derive(-fx-base, 5%); -fx-background-radius: 4;");

        HBox updateRow = new HBox(10);
        updateRow.setAlignment(Pos.CENTER_LEFT);
        aisUpdateIntervalSpinner.setPrefWidth(100);
        aisUpdateIntervalSpinner.setEditable(true);
        aisUpdateIntervalSpinner.getValueFactory().setValue(aisUpdateInterval);
        updateRow.getChildren().addAll(new Label("AIS数据更新间隔:"), aisUpdateIntervalSpinner, new Label("毫秒"));
        updateBox.getChildren().add(updateRow);

        section.getChildren().addAll(titleLabel, updateBox);
        return section;
    }

    private void loadCurrentSettings() {
        autoSaveCheckBox.setSelected(autoSave);
        autoSaveIntervalSpinner.getValueFactory().setValue(autoSaveInterval);
        cacheSizeSpinner.getValueFactory().setValue(cacheSize);
        alarmSoundCheckBox.setSelected(alarmSound);
        aisUpdateIntervalSpinner.getValueFactory().setValue(aisUpdateInterval);
    }

    private void applySettings() {
        autoSave = autoSaveCheckBox.isSelected();
        autoSaveInterval = autoSaveIntervalSpinner.getValue();
        cacheSize = cacheSizeSpinner.getValue();
        alarmSound = alarmSoundCheckBox.isSelected();
        aisUpdateInterval = aisUpdateIntervalSpinner.getValue();

        String selectedLocale = localeComboBox.getSelectionModel().getSelectedItem();
        if (selectedLocale != null) {
            if (selectedLocale.contains("en_US")) {
                locale = "en_US";
            } else {
                locale = "zh_CN";
            }
        }
    }

    public boolean isAutoSaveEnabled() {
        return autoSave;
    }

    public int getAutoSaveInterval() {
        return autoSaveInterval;
    }

    public int getCacheSize() {
        return cacheSize;
    }

    public boolean isAlarmSoundEnabled() {
        return alarmSound;
    }

    public int getAisUpdateInterval() {
        return aisUpdateInterval;
    }

    public String getLocale() {
        return locale;
    }

    public void setOwner(Window owner) {
        initOwner(owner);
    }
}
