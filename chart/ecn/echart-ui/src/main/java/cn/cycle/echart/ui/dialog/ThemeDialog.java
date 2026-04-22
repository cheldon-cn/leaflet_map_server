package cn.cycle.echart.ui.dialog;

import cn.cycle.echart.theme.Theme;
import cn.cycle.echart.theme.ThemeManager;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Rectangle;
import javafx.stage.Modality;
import javafx.stage.Window;

public class ThemeDialog extends Dialog<ButtonType> {

    private final ThemeManager themeManager;
    private final ListView<ThemeItem> themeListView;
    private final VBox previewBox;
    private final Label descriptionLabel;

    private Theme selectedTheme;
    private Theme originalTheme;

    public ThemeDialog(ThemeManager themeManager) {
        this.themeManager = themeManager;
        this.themeListView = new ListView<>();
        this.previewBox = new VBox(15);
        this.descriptionLabel = new Label();

        this.originalTheme = themeManager != null ? themeManager.getCurrentTheme() : null;
        this.selectedTheme = originalTheme;

        initializeDialog();
    }

    private void initializeDialog() {
        setTitle("主题设置");
        setHeaderText(null);
        initModality(Modality.APPLICATION_MODAL);

        BorderPane mainLayout = new BorderPane();
        mainLayout.setPrefSize(550, 400);

        VBox leftPanel = createThemeList();
        leftPanel.setPrefWidth(180);
        mainLayout.setLeft(leftPanel);

        VBox rightPanel = createPreviewPanel();
        mainLayout.setCenter(rightPanel);

        getDialogPane().setContent(mainLayout);
        getDialogPane().getButtonTypes().addAll(ButtonType.OK, ButtonType.CANCEL, createResetButtonType());

        setResultConverter(buttonType -> {
            if (buttonType == ButtonType.OK) {
                return ButtonType.OK;
            } else if (buttonType != null && buttonType.getButtonData() == ButtonBar.ButtonData.OTHER) {
                resetToDefault();
                return null;
            }
            selectedTheme = originalTheme;
            return buttonType;
        });

        loadThemes();
    }

    private ButtonType createResetButtonType() {
        return new ButtonType("重置为默认", ButtonBar.ButtonData.OTHER);
    }

    private VBox createThemeList() {
        VBox panel = new VBox(5);
        panel.setPadding(new Insets(10));
        panel.setStyle("-fx-background-color: derive(-fx-base, -5%);");

        Label titleLabel = new Label("可用主题");
        titleLabel.setStyle("-fx-font-weight: bold; -fx-padding: 0 0 10 0;");

        themeListView.setCellFactory(param -> new ThemeListCell());
        themeListView.getSelectionModel().selectedItemProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal != null) {
                selectedTheme = newVal.getTheme();
                updatePreview();
            }
        });

        VBox.setVgrow(themeListView, Priority.ALWAYS);
        panel.getChildren().addAll(titleLabel, themeListView);

        return panel;
    }

    private VBox createPreviewPanel() {
        VBox panel = new VBox(15);
        panel.setPadding(new Insets(15));

        Label titleLabel = new Label("主题预览");
        titleLabel.setStyle("-fx-font-weight: bold; -fx-font-size: 14px;");

        HBox colorPreviewBox = new HBox(15);
        colorPreviewBox.setPadding(new Insets(15));
        colorPreviewBox.setStyle("-fx-background-color: derive(-fx-base, 5%); -fx-background-radius: 8;");
        colorPreviewBox.setAlignment(Pos.CENTER_LEFT);

        VBox colorSection = new VBox(8);
        Label colorTitle = new Label("颜色预览");
        colorTitle.setStyle("-fx-font-size: 11px; -fx-text-fill: derive(-fx-text-background-color, -30%);");

        HBox colorSwatches = new HBox(8);
        colorSwatches.setAlignment(Pos.CENTER_LEFT);
        String[] colorKeys = {"background", "accent.primary", "success", "warning", "error"};
        for (String key : colorKeys) {
            Circle circle = new Circle(14);
            circle.getStyleClass().add("color-swatch-" + key.replace('.', '-'));
            colorSwatches.getChildren().add(circle);
        }
        colorSection.getChildren().addAll(colorTitle, colorSwatches);

        VBox sampleSection = new VBox(10);
        sampleSection.setAlignment(Pos.CENTER_LEFT);

        Button sampleButton = new Button("示例按钮");
        sampleButton.getStyleClass().add("theme-sample-button");

        HBox sampleInput = new HBox();
        sampleInput.setStyle("-fx-background-color: white; -fx-border-color: #E0E0E0; " +
                "-fx-border-width: 1; -fx-border-radius: 3; -fx-background-radius: 3;");
        sampleInput.setPadding(new Insets(6, 12, 6, 12));
        Label inputLabel = new Label("示例输入框");
        inputLabel.setStyle("-fx-text-fill: #757575;");
        sampleInput.getChildren().add(inputLabel);

        sampleSection.getChildren().addAll(sampleButton, sampleInput);

        colorPreviewBox.getChildren().addAll(colorSection, sampleSection);

        VBox infoBox = new VBox(8);
        infoBox.setPadding(new Insets(10));
        infoBox.setStyle("-fx-background-color: derive(-fx-base, -3%); -fx-background-radius: 4;");

        descriptionLabel.setWrapText(true);
        descriptionLabel.setStyle("-fx-font-size: 12px;");
        infoBox.getChildren().add(descriptionLabel);

        VBox.setVgrow(infoBox, Priority.ALWAYS);
        panel.getChildren().addAll(titleLabel, colorPreviewBox, infoBox);

        return panel;
    }

    private void loadThemes() {
        if (themeManager == null) {
            return;
        }

        for (Theme theme : themeManager.getAvailableThemes()) {
            ThemeItem item = new ThemeItem(theme);
            themeListView.getItems().add(item);
            if (theme == originalTheme) {
                themeListView.getSelectionModel().select(item);
            }
        }

        updatePreview();
    }

    private void updatePreview() {
        if (selectedTheme == null) {
            return;
        }

        HBox colorPreviewBox = (HBox) previewBox.lookup(".color-preview-box");
        if (colorPreviewBox != null) {
            HBox colorSwatches = (HBox) colorPreviewBox.lookup(".color-swatches");
            if (colorSwatches != null) {
                String[] colorKeys = {"background", "accent.primary", "success", "warning", "error"};
                int i = 0;
                for (javafx.scene.Node node : colorSwatches.getChildren()) {
                    if (node instanceof Circle && i < colorKeys.length) {
                        String color = selectedTheme.getColor(colorKeys[i]);
                        if (color != null) {
                            try {
                                ((Circle) node).setFill(Color.web(color));
                            } catch (Exception e) {
                                ((Circle) node).setFill(Color.GRAY);
                            }
                        }
                        i++;
                    }
                }
            }
        }

        StringBuilder desc = new StringBuilder();
        desc.append("名称: ").append(selectedTheme.getDisplayName()).append("\n");
        desc.append("版本: ").append(selectedTheme.getVersion()).append("\n");
        desc.append("作者: ").append(selectedTheme.getAuthor()).append("\n\n");
        desc.append(selectedTheme.getDescription());
        descriptionLabel.setText(desc.toString());
    }

    private void resetToDefault() {
        if (themeManager != null) {
            themeManager.resetToDefault();
            selectedTheme = themeManager.getCurrentTheme();
            themeListView.getItems().clear();
            loadThemes();
            updatePreview();
        }
    }

    public Theme getSelectedTheme() {
        return selectedTheme;
    }

    public void setOwner(Window owner) {
        initOwner(owner);
    }

    private static class ThemeItem {
        private final Theme theme;

        public ThemeItem(Theme theme) {
            this.theme = theme;
        }

        public Theme getTheme() {
            return theme;
        }

        @Override
        public String toString() {
            return theme.getDisplayName();
        }
    }

    private static class ThemeListCell extends ListCell<ThemeItem> {
        @Override
        protected void updateItem(ThemeItem item, boolean empty) {
            super.updateItem(item, empty);

            if (empty || item == null) {
                setGraphic(null);
                setText(null);
            } else {
                Theme theme = item.getTheme();

                HBox content = new HBox(10);
                content.setAlignment(Pos.CENTER_LEFT);

                Rectangle colorRect = new Rectangle(24, 24);
                colorRect.setArcWidth(4);
                colorRect.setArcHeight(4);
                String bgColor = theme.isDark() ? "#2c3e50" : "#f5f5f5";
                colorRect.setFill(Color.web(bgColor));
                colorRect.setStroke(theme.isDark() ? Color.web("#1a252f") : Color.web("#e0e0e0"));

                VBox textBox = new VBox(2);
                Label nameLabel = new Label(theme.getDisplayName());
                nameLabel.setStyle("-fx-font-weight: bold; -fx-font-size: 12px;");

                Label typeLabel = new Label(theme.isDark() ? "暗色" : "亮色");
                typeLabel.setStyle("-fx-font-size: 10px; -fx-text-fill: derive(-fx-text-background-color, -30%);");

                textBox.getChildren().addAll(nameLabel, typeLabel);
                content.getChildren().addAll(colorRect, textBox);

                setGraphic(content);
                setText(null);
            }
        }
    }
}
