#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QObject>
#include <QGraphicsPixmapItem>

class Obstacle : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Obstacle(int health, int resourceValue, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);

    // �����˺�
    void takeDamage(int damage);
    // ��ȡ�ݻٺ��ṩ����Դֵ
    int getResourceValue() const;

    signals:
        // ���ϰ��ﱻ�ݻ�ʱ�������ź�
        void destroyed(Obstacle* obstacle, int resourceValue);

private:
    int maxHealth;
    int currentHealth;
    int resourceValue;
};

#endif // OBSTACLE_H
