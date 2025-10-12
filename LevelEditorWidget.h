#pragma once

#include <QWidget>
#include <QPoint>
#include <QSize>
#include <vector>
#include "WaveManager.h"

// --- ǰ������Qt�ؼ��� ---
class QPushButton;
class QRadioButton;
class QLabel;
class QVBoxLayout;

/*
 * ����: LevelEditorWidget
 * ������: P8 - ���ݹ�����ؿ��༭��
 * ˵��: (���ع�) ��ȫʹ��C++���빹��UI��������.ui�ļ���
 * �����˱༭ģʽ�л����ܡ�
 */
class LevelEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit LevelEditorWidget(QWidget *parent = nullptr);
    ~LevelEditorWidget() = default; // ʹ��Ĭ������

    private slots:
        // �ļ������ۺ���
        void onSaveButtonPressed();
    void onLoadButtonPressed();

    // �༭ģʽ�л��ۺ���
    void onModeChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override; // �������ƶ��¼�����Ԥ��

private:
    // ��ʼ��UI���ڹ��캯���е���
    void setupUI();

    // �ļ����л�/�����л�
    void saveLevelToFile(const QString& filePath);
    void loadLevelFromFile(const QString& filePath);

    // --- �༭ģʽ ---
    enum class EditMode {
        PlacePath,
        PlaceTowerBase,
        Erase
    };
    EditMode currentMode;

    // --- �ؿ����� ---
    QString backgroundPath;
    QSize gridSize;
    std::vector<QPoint> path;
    std::vector<QPoint> towerBases;
    std::vector<WaveInfo> waves;

    // --- UI �ؼ�ָ�� ---
    QVBoxLayout* mainLayout; // ������
    // (�˴�������Ӹ���ؼ�, ��ؿ����õ�������)
    QPushButton* loadButton;
    QPushButton* saveButton;
    QRadioButton* pathModeButton;
    QRadioButton* towerModeButton;
    QRadioButton* eraseModeButton;
    QLabel* statusLabel; // ������ʾ��ǰ״̬

    // --- �༭���������� ---
    QPoint currentMouseGridPos; // ��ǰ������ڵ���������
};

