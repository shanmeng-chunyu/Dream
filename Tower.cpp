#include "Tower.h"
#include "Bullet.h"
#include "Enemy.h"
#include <QLineF>
#include <QMovie>
#include <QGraphicsScene>
#include "LiveCoffee.h"
#include "FriendCompanion.h"

Tower::Tower(int damage, double range, double fireRate,int cost,int upgradeCost,const QString &gif_path, const QSize& pixelSize, QGraphicsItem* parent)
    : QObject(nullptr),
    QGraphicsPixmapItem(parent),
    damage(damage),
    range(range),
    fireRate(fireRate),
    cost(cost),
    upgradeCost(upgradeCost),
    upgraded(false),
    currentTarget(nullptr),
    fireInterval(static_cast<int>(fireRate * 60)),fireCount(fireInterval),
    originalFireInterval(fireInterval),originalDamage(damage),
    m_pixelSize(pixelSize){

    m_movie = new QMovie(gif_path, QByteArray(), this);
    m_movie->setCacheMode(QMovie::CacheAll);
    connect(m_movie, &QMovie::frameChanged, this, &Tower::updatePixmapFromMovie);
    m_movie->start();
    if (m_movie->isValid()) {
        updatePixmapFromMovie();
    }
    // 3. 启用鼠标悬停事件
    setAcceptHoverEvents(true);
    m_rangeCircle = new QGraphicsEllipseItem(this);

    // 4. 设置范围圆的样式
    QPen pen(QColor(255, 255, 255, 120));
    pen.setWidth(2);
    m_rangeCircle->setPen(pen);
    m_rangeCircle->setBrush(QColor(255, 255, 255, 30));
    m_rangeCircle->setZValue(-1);

    // 5. 将默认可见性设置为 true
    m_rangeCircle->setVisible(false); // <-- 修改为 true

    // 6. 设置范围圆的位置和大小
    qreal r = range;
    qreal center = 76.0 / 2.0;
    m_rangeCircle->setRect(center - r, center - r, r * 2, r * 2);
}

void Tower::setTarget(QGraphicsPixmapItem* target) {
    currentTarget = target;
}

bool Tower::targetIsInRange() const {
    if (!currentTarget) {
        return false;
    }
    QLineF line(pos(), currentTarget->pos());//tower和enemy之间的距离
    return line.length() <= range;
}

void Tower::findAndAttackTarget() {
    fireCount--;
    if(fireCount<=0)
    {
        if (currentTarget && targetIsInRange())
        {
            coffeeEffect();
            friendEffect();
            attack();
        }
        fireCount=fireInterval;
    }
    //如果没有目标或者目标超出范围，GameManager会负责为其寻找新目标
}

void Tower::attack() {
    if (currentTarget) {
        emit newBullet(this, currentTarget);
    }
}

void Tower::destroy(){
     emit towerDestroyed(this);
}

void Tower::slowAttack(double slowFactor){//传入的slowFactor应该是一个大于1的数字
    //GameManager会负责检测周围是否有回忆怪物
    fireInterval= static_cast<int>(fireInterval * slowFactor);
}

void Tower::slowAttackStop()
{
    fireInterval=originalFireInterval;
}

QList<LiveCoffee*> Tower::findCoffeeInRange()
{
    QList<LiveCoffee*>coffeeTowers;
    QList<QGraphicsItem*> items = scene()->items();
    for(auto& item :items)
    {
        LiveCoffee* tower=dynamic_cast<LiveCoffee*>(item);
        if(tower&&tower!=this)
        {
            QLineF line(pos(),tower->pos());
            if(line.length()<=tower->getRange())
                coffeeTowers.append(tower);
        }
    }
    return coffeeTowers;
}
QList<FriendCompanion*>Tower:: findFriendInRange()
{
    QList<FriendCompanion*>friendTowers;
    QList<QGraphicsItem*> items = scene()->items();
    for(auto& item :items)
    {
       FriendCompanion* tower=dynamic_cast<FriendCompanion*>(item);
        if(tower&&tower!=this)
        {
            QLineF line(pos(),tower->pos());
            if(line.length()<=tower->getRange())
                friendTowers.append(tower);
        }
    }
    return friendTowers;
}

void Tower::coffeeEffect()
{
    fireInterval=originalFireInterval;
    QList<LiveCoffee*>auraTowers=findCoffeeInRange();
    for(auto& tower:auraTowers)
    {
        fireInterval = static_cast<int>(fireInterval * tower->getTowerBuffFactor());
    }
    if (fireCount > fireInterval) {
        fireCount = fireInterval;
    }
}

void Tower::friendEffect()
{
    damage=originalDamage;
    QList<FriendCompanion*>auraTowers=findFriendInRange();
    for(auto& tower:auraTowers)
    {
        damage*=tower->getFactor();
    }
}

void Tower::upgrade(){
    if (upgraded) {

    }
}

void Tower::setRange(double newrange) {
    range=newrange;
    if (m_rangeCircle) {
        qreal r = range;
        qreal center = 76.0 / 2.0;
        m_rangeCircle->setRect(center - r, center - r, r * 2, r * 2);
    }
}

void Tower::showRange(bool show)
{
    if (m_rangeCircle) {
        m_rangeCircle->setVisible(show);
    }
}

void Tower::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    showRange(true); // 显示范围
    QGraphicsPixmapItem::hoverEnterEvent(event);
}

void Tower::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    showRange(false); // 隐藏范围
    QGraphicsPixmapItem::hoverLeaveEvent(event);
}

void Tower::updatePixmapFromMovie()
{
    // 关键：从 QMovie 获取当前帧，并 *缩放* 到我们期望的尺寸
    QPixmap scaledFrame = m_movie->currentPixmap().scaled(m_pixelSize,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation);
    // 将 QGraphicsPixmapItem 的贴图设置为这一帧
    setPixmap(scaledFrame);
}

void Tower::pauseAnimation()
{
    // 检查 m_movie 是否存在，并且正在运行
    if (m_movie && m_movie->state() == QMovie::Running) {
        m_movie->setPaused(true);
    }
}

void Tower::resumeAnimation()
{
    // 检查 m_movie 是否存在，并且之前是暂停状态
    if (m_movie && m_movie->state() == QMovie::Paused) {
        m_movie->setPaused(false);
    }
}