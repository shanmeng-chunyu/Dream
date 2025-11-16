#include "Obstacle.h"

Obstacle::Obstacle(int health, int resourceValue, const QPixmap& pixmap, const QRectF& relativeRect, QGraphicsItem* parent)
    : QObject(nullptr), // QObject的父对象关系由场景管理，这里设为nullptr
      QGraphicsPixmapItem(pixmap, parent),
      maxHealth(health),
      currentHealth(health),
      resourceValue(resourceValue),
      m_relativeRect(relativeRect)
{}

void Obstacle::takeDamage(int damage) {
    currentHealth -= damage;
    if (currentHealth <= 0) {
        currentHealth = 0;
        // 发射销毁信号，由GameManager处理资源增加和对象删除
        emit destroyed(this, resourceValue);
    }
}

int Obstacle::getResourceValue() const {
    return resourceValue;
}

QRectF Obstacle::getRelativeRect() const {
    return m_relativeRect;
}