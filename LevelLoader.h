#pragma once // ��ֹͷ�ļ����ظ�����

#include <QString>
#include <vector>
#include <QPointF>
#include <QPixmap>
#include "WaveManager.h" // ��Ҫ�õ� WaveInfo �ṹ��

// ǰ������ (Forward Declaration)
class GameMap;
class WaveManager;

// ����һ���ṹ�壬���ڷ�װ���ļ��м��ص����йؿ�����
// �������ĺô����ü��غ����ķ��ظ�������������
struct LevelData {
    QString backgroundPath;
    std::vector<QPointF> pathPoints;
    std::vector<QPointF> towerBasePositions;
    std::vector<WaveInfo> waves;
};

/*
 * ����: LevelLoader
 * ������: P8 - ���ݹ�����ؿ��༭��
 * ˵��: ����һ����̬�����࣬����������ļ�����˴�ʾ����JSON�ļ�����
 * ��ȡ�ͽ����ؿ����ݡ������ļ�����ת��Ϊ��Ϸ����ģ���������
 * ���ݽṹ�����������ӹؿ��༭������Ϸ������Ĺؼ���Ŧ��
 * ���Ҫ��:
 * - ʹ�þ�̬����: loadForLevel ��һ����̬�������ⲿ�������贴��
 * LevelLoader ��ʵ�����ɵ��ã����� LevelLoader::loadForLevel(1)��
 * - ���ݷ�װ: ʹ�� LevelData �ṹ�����ۺ����йؿ����ݣ�ʹ�ú���
 * �ӿڸ������࣬������չ��
 * - ְ��һ: �����ֻ���𡰼��ء��͡����������������κ���Ϸ�߼���
 */
class LevelLoader {
public:
    // ɾ�����캯������ֹ�ⲿ���ⴴ��ʵ��
    LevelLoader() = delete;

    // ���ݹؿ�ID���ض�Ӧ�Ĺؿ������ļ�
    // levelId: �ؿ��ı�ţ����� 1, 2, 3...
    // levelData: һ�����������������سɹ����˽ṹ�彫������йؿ�����
    // ����ֵ: bool, true ��ʾ���غͽ����ɹ���false ��ʾʧ��
    static bool loadForLevel(int levelId, LevelData& levelData);
};
