#pragma once

#include <QGraphicsPixmapItem>

// Obstacle (�ϰ���)
// ������: P2-��Ϸ�����뽻������
// ˵��: ����һ�������ͼ�Ͼ�̬�ϰ�����ࡣ���̳���QGraphicsPixmapItem�Ա����ڳ�������ʾ��
//      �ϰ����Ǿ�̬�ģ������û�и��ӵ��߼���Ҳ����Ҫ��̬��������ÿ֡���¡�
//      ���Ĵ����ͷ�����GameManager�ڼ��ص�ͼʱͳһ����
class Obstacle : public QGraphicsPixmapItem {
public:
    // ���캯������һ���ϰ�������ID��һ����ѡ�ĸ���
    // ʵ����Ŀ��, ����Ը���typeId���ز�ͬ���ϰ���ͼƬ
    explicit Obstacle(int typeId, QGraphicsItem* parent = nullptr);

    // Obstacleû���Լ���update�߼�, ���Բ���Ҫ������ʵ��update()����
};
