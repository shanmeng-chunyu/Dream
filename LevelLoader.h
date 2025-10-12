#pragma once

#include <QString>
#include <vector>
#include <QPoint>  // ʹ�� QPoint ���洢 (col, row) ������
#include <QSize>   // ʹ�� QSize ���洢 (cols, rows)
#include "WaveManager.h"

// ����һ���ṹ�壬���ڷ�װ���ļ��м��ص����йؿ�����
// �Ѹ���Ϊ���񲼾�
struct LevelData {
    QString backgroundPath;
    QSize gridSize; // ����ά��
    std::vector<QPoint> pathPoints; // ��������
    std::vector<QPoint> towerBasePositions; // ��������
    std::vector<WaveInfo> waves;
};

/*
 * ����: LevelLoader
 * ������: P8 - ���ݹ�����ؿ��༭��
 * ˵��: (�Ѹ���) ��̬�����࣬����ӻ��������JSON�ļ��ж�ȡ��
 * �����ؿ����ݡ�
 */
class LevelLoader {
public:
    LevelLoader() = delete; // ��̬�࣬��ֹʵ����

    // ���ݹؿ�ID���ض�Ӧ�Ĺؿ������ļ�
    // levelData: ����������������Ϊ����������񻯹ؿ�����
    // ����ֵ: bool, true ��ʾ���غͽ����ɹ�
    static bool loadForLevel(int levelId, LevelData& levelData);
};

