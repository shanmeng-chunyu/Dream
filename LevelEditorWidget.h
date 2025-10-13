#ifndef LEVELEDITORWIDGET_H
#define LEVELEDITORWIDGET_H

#include <QWidget>

// ǰ������������Ҫ��Qt�ؼ��࣬������ͷ�ļ����������ͷ�ļ�
class QLineEdit;
class QPushButton;
class QListWidget;
class QSpinBox;
class QDoubleSpinBox;
class QTabWidget;
class QListWidgetItem;

class LevelEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit LevelEditorWidget(QWidget* parent = nullptr);
    ~LevelEditorWidget() override = default;

    private slots:
        void saveLevel();
    void loadLevel();
    void addWave();
    void removeWave();
    void addEnemyToWave();
    void removeEnemyFromWave();
    void onWaveSelectionChanged();
    void onEnemySelectionChanged();
    void updateSelectedEnemy();

private:
    void setupUI();
    void clearEnemyDetails();

    // ������
    QTabWidget* tabWidget;

    // --- ���α༭�� Tab ---
    QWidget* waveTab;
    QListWidget* waveListWidget;
    QListWidget* enemyInWaveListWidget;

    QPushButton* addWaveButton;
    QPushButton* removeWaveButton;
    QPushButton* addEnemyToWaveButton;
    QPushButton* removeEnemyFromWaveButton;

    // ��������༭��
    QLineEdit* enemyTypeLineEdit;
    QSpinBox* enemyCountSpinBox;
    QDoubleSpinBox* enemyIntervalSpinBox;

    // --- �ؿ����� Tab ---
    QWidget* configTab;
    // (������չ�˴�������ؿ����õķ������͵���)
    QLineEdit* levelNameEdit;


    // �ײ���ť
    QPushButton* saveButton;
    QPushButton* loadButton;
};

#endif // LEVELEDITORWIDGET_H
