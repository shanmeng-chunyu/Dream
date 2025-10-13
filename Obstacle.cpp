#include "Obstacle.h"

Obstacle::Obstacle(int health, int resourceValue, const QPixmap& pixmap, QGraphicsItem* parent)
    : QObject(nullptr), // QObject�ĸ������ϵ�ɳ�������������Ϊnullptr
      QGraphicsPixmapItem(pixmap, parent),
      maxHealth(health),
      currentHealth(health),
      resourceValue(resourceValue) {}

void Obstacle::takeDamage(int damage) {
    currentHealth -= damage;
    if (currentHealth <= 0) {
        currentHealth = 0;
        // ���������źţ���GameManager������Դ���ӺͶ���ɾ��
        emit destroyed(this, resourceValue);
    }
}

int Obstacle::getResourceValue() const {
    return resourceValue;
}
