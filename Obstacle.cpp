#include "Obstacle.h"
#include <QPixmap>

// Obstacle (�ϰ���)
// ������: P2-��Ϸ�����뽻������
// ˵��: ���캯���������ĺ��ġ�������һ������ID��������������ͼƬ��Ϊ�ϰ������ۡ�
//      �������ʹ�����ǿ����ڹؿ�������ֻ��һ�����֣����ܴ����ʽ�������ϰ��
Obstacle::Obstacle(int typeId, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent) {
    QString imagePath;
    // ����typeIdѡ��ͬ��ͼƬ��Դ
    // ʵ�ʵ�·����Ҫ����������Ŀ���������Դ��λ������
    switch (typeId) {
        case 1:
            imagePath = ":/assets/images/obstacles/�鱾�ϰ�����.png";
        break;
        case 2:
            imagePath = ":/assets/images/obstacles/�������ϰ�����.png";
        break;
        case 3:
            imagePath = ":/assets/images/obstacles/�ľ��ϰ�����.png";
        break;
        // ���Ը�����Ҫ��Ӹ����case����Ӧ��ͬ���͵��ϰ���
        default:
            // �ṩһ��Ĭ�ϵ�ͼ���Է�typeId��Ч
                imagePath = ":/assets/images/obstacles/default_obstacle.png";
        break;
    }
    setPixmap(QPixmap(imagePath));
}
