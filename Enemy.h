#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPointF>
#include <vector>
#include <string>

class LiveCoffee;
class QMovie;
class Enemy : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Enemy(int health, double speed, int damage,
               const std::vector<QPointF>& path, QString type,
               const QString& gifPath,
               const QSize& pixelSize,
               QGraphicsItem* parent = nullptr);

    void takeDamage(int damageAmount);
    int getDamage() const;
    void setAbsolutePath(const std::vector<QPointF>& path);
    QString getType() const{return type;};
    std::vector<QPointF> getAbsolutePath(){return absolutePath;};
    void stopFor(double duration);
    int getHealth() const;
    void heal(int amount);
    void setBaseSpeed(double v);
    int getCurrentPathIndex() const;

    public slots:
        void move();
        void applyVisualEffect(const QPixmap& pixmap, double duration);
private slots:
    void updatePixmapFromMovie(); // 更新当前帧
    signals:
        void reachedEnd(Enemy* enemy);
    void died(Enemy* enemy);
protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
private:
    int damage;
    std::vector<QPointF> absolutePath;
    QString type;
    int m_health;
    double m_speed;
    int m_maxHealth;
    int m_currentPathIndex;
    void applyAuraEffects();
    QList<LiveCoffee*> findCoffeeInRange() const;
    QMovie* m_movie;       // 用于播放GIF
    QSize m_pixelSize;     // 存储敌人的标准像素尺寸

    int m_stunTicksRemainimng = 0;
    double m_baseSpeed;//Ô­Ê¼ËÙ¶È
    bool m_isFlipped;//ÌùÍ¼ÊÇ·ñ·­×ª

    void removeVisualEffect();
    QGraphicsPixmapItem* m_effectItem; // 指向当前特效贴图的指针
    int m_effectTicksRemaining;
};

#endif // ENEMY_H
