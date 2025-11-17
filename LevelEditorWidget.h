#ifndef LEVELEDITORWIDGET_H
#define LEVELEDITORWIDGET_H

#include <QWidget>
#include <QMap>
#include <QJsonObject>

// 前向声明
class QLineEdit;
class QPushButton;
class QListWidget;
class QSpinBox;
class QComboBox;
class QLabel;
class QListWidgetItem;
class QSplitter;
class QGroupBox;

/**
 * @brief 关卡编辑器 (LevelEditorWidget)
 * @note v2.0 重构版:
 * - 移除了 QTabWidget，将波次编辑器和防御塔选择器合并到单一视图。
 * - 波次编辑器简化：敌人类型使用 QComboBox 选择，数量可调，间隔固定。
 * - 防御塔选择器：固定4个槽位，使用 QComboBox 选择，并检查唯一性。
 * - 数据驱动：敌方和塔的"原型"数据从 master JSON 文件加载，编辑器只负责"配置"。
 */
class LevelEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit LevelEditorWidget(QWidget* parent = nullptr);
    ~LevelEditorWidget() override = default;
    void loadLevelForEditing(const QString& filePath);

protected:
    // 重写 paintEvent 以绘制自定义背景
    void paintEvent(QPaintEvent* event) override;
signals:
    void levelEditingFinished(const QString& savedLevelPath);
private slots:
    // --- 通用 ---
    void saveLevel();
    void loadLevel();

    // --- 波次编辑器 (上半部分) ---
    void addWave();
    void removeWave();
    void onWaveSelectionChanged();

    void addEnemyToWave();
    void removeEnemyFromWave();
    void onEnemyInWaveSelectionChanged();

    void onWaveEnemyTypeChanged(int index);
    void onWaveEnemyCountChanged(int count);

    // --- 防御塔选择器 (下半部分) ---
    void onTowerSlotSelectionChanged();
    void onTowerTypeChanged(int index);



private:
    // --- UI 构建 ---
    void setupUI();
    QGroupBox* createWaveEditorGroup();
    QGroupBox* createTowerSelectionGroup();

    // --- 数据加载 ---
    void loadPrototypes();

    // --- UI 更新辅助 ---
    void updateWaveEnemyDetailsUI(QListWidgetItem* item);
    void updateTowerDetailsUI(QListWidgetItem* item);
    void updateTowerTypeComboBox();
    QString getPixmapPath(const QMap<QString, QJsonObject>& prototypes, const QString& type);
    void updateWaveItemData(QListWidgetItem* waveItem, int enemyRow, const QJsonObject& enemyData);

    // --- 顶级布局 ---
    QSplitter* mainSplitter;
    QLineEdit* levelNameEdit;
    QPushButton* saveButton;
    QPushButton* loadButton;

    // --- 波次编辑器控件 ---
    QListWidget* waveListWidget;
    QListWidget* enemyInWaveListWidget;
    QPushButton* addWaveButton;
    QPushButton* removeWaveButton;
    QPushButton* addEnemyToWaveButton;
    QPushButton* removeEnemyFromWaveButton;

    // 波次中的敌人详情
    QComboBox* wave_enemyTypeComboBox;
    QSpinBox* wave_enemyCountSpinBox;
    QLabel* wave_enemyThumbnailLabel;
    QLabel* wave_enemyDescriptionLabel;

    // --- 防御塔选择器控件 ---
    QListWidget* availableTowersListWidget; // 固定4个槽位
    QComboBox* tower_typeComboBox;
    QLabel* tower_thumbnailLabel;
    QLabel* tower_warningLabel;
    QLabel* tower_statsLabel;
    QLabel* tower_descriptionLabel;

    // --- 原型数据 (从 enemy_data.json 和 tower_data.json 加载) ---
    QMap<QString, QJsonObject> m_enemyPrototypes;
    QMap<QString, QJsonObject> m_towerPrototypes;
    QString m_firstEnemyType; // 用于"添加敌人"时的默认值
    QList<QString> m_enemyTypeOrder;
    QList<QString> m_towerTypeOrder;

    QPixmap m_backgroundPixmap; // 存储背景图片
    QString m_fontFamily;//存储自定义字体名称
};

#endif // LEVELEDITORWIDGET_H