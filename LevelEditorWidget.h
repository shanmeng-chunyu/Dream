#ifndef LEVELEDITORWIDGET_H
#define LEVELEDITORWIDGET_H

#include <QWidget>
#include <QMap>

// 前向声明所有需要的Qt控件类，避免在头文件中引入大量头文件
class QLineEdit;
class QPushButton;
class QListWidget;
class QSpinBox;
class QDoubleSpinBox;
class QTabWidget;
class QListWidgetItem;
class QFormLayout;

class LevelEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit LevelEditorWidget(QWidget* parent = nullptr);
    ~LevelEditorWidget() override = default;

    private slots:
        // --- 通用 ---
        void saveLevel();
    void loadLevel();

    // --- 波次编辑器 Tab ---
    void addWave();
    void removeWave();
    void addEnemyToWave();
    void removeEnemyFromWave();
    void onWaveSelectionChanged();
    void onEnemyInWaveSelectionChanged();
    void updateSelectedEnemyInWave();

    // --- 游戏配置 Tab (新) ---
    void addAvailableTower();
    void removeAvailableTower();
    void onAvailableTowerChanged();
    void updateSelectedTower();

    void addAvailableEnemy();
    void removeAvailableEnemy();
    void onAvailableEnemyChanged();
    void updateSelectedEnemy();


private:
    void setupUI();
    void setupWaveTab();
    void setupConfigTab();

    void clearEnemyInWaveDetails();
    void clearTowerDetails();
    void clearEnemyDetails();

    // --- 主布局 ---
    QTabWidget* tabWidget;
    QPushButton* saveButton;
    QPushButton* loadButton;

    // --- 波次编辑器 Tab 控件 ---
    QWidget* waveTab;
    QListWidget* waveListWidget;
    QListWidget* enemyInWaveListWidget;

    QPushButton* addWaveButton;
    QPushButton* removeWaveButton;
    QPushButton* addEnemyToWaveButton;
    QPushButton* removeEnemyFromWaveButton;

    // "波次"中的敌人详情 (只读 数量/间隔)
    QLineEdit* wave_enemyTypeLineEdit;
    QSpinBox* wave_enemyCountSpinBox;
    QDoubleSpinBox* wave_enemyIntervalSpinBox;

    // --- 游戏配置 Tab 控件 ---
    QWidget* configTab;
    QLineEdit* levelNameEdit;

    // 可用防御塔
    QListWidget* availableTowersListWidget;
    QPushButton* addTowerButton;
    QPushButton* removeTowerButton;
    QFormLayout* towerDetailsLayout;
    // 防御塔属性控件
    QMap<QString, QWidget*> towerPropertyWidgets; // 用于快速访问

    // 可用敌人
    QListWidget* availableEnemiesListWidget;
    QPushButton* addEnemyButton;
    QPushButton* removeEnemyButton;
    QFormLayout* enemyDetailsLayout;
    // 敌人属性控件
    QMap<QString, QWidget*> enemyPropertyWidgets; // 用于快速访问
};

#endif // LEVELEDITORWIDGET_H