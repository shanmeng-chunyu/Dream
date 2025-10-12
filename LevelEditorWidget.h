#pragma once

#include <QWidget>
#include <QPoint>
#include <QSize>
#include <vector>
#include "WaveManager.h"

// --- 前向声明Qt控件类 ---
class QPushButton;
class QRadioButton;
class QLabel;
class QVBoxLayout;

/*
 * 类名: LevelEditorWidget
 * 负责人: P8 - 数据管理与关卡编辑器
 * 说明: (已重构) 完全使用C++代码构建UI，不依赖.ui文件。
 * 增加了编辑模式切换功能。
 */
class LevelEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit LevelEditorWidget(QWidget *parent = nullptr);
    ~LevelEditorWidget() = default; // 使用默认析构

    private slots:
        // 文件操作槽函数
        void onSaveButtonPressed();
    void onLoadButtonPressed();

    // 编辑模式切换槽函数
    void onModeChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override; // 添加鼠标移动事件用于预览

private:
    // 初始化UI，在构造函数中调用
    void setupUI();

    // 文件序列化/反序列化
    void saveLevelToFile(const QString& filePath);
    void loadLevelFromFile(const QString& filePath);

    // --- 编辑模式 ---
    enum class EditMode {
        PlacePath,
        PlaceTowerBase,
        Erase
    };
    EditMode currentMode;

    // --- 关卡数据 ---
    QString backgroundPath;
    QSize gridSize;
    std::vector<QPoint> path;
    std::vector<QPoint> towerBases;
    std::vector<WaveInfo> waves;

    // --- UI 控件指针 ---
    QVBoxLayout* mainLayout; // 主布局
    // (此处可以添加更多控件, 如关卡设置的输入框等)
    QPushButton* loadButton;
    QPushButton* saveButton;
    QRadioButton* pathModeButton;
    QRadioButton* towerModeButton;
    QRadioButton* eraseModeButton;
    QLabel* statusLabel; // 用于显示当前状态

    // --- 编辑器辅助变量 ---
    QPoint currentMouseGridPos; // 当前鼠标所在的网格坐标
};

