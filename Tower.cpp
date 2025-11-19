#include "Tower.h"
#include "Bullet.h"
#include "Enemy.h"
#include <QLineF>
#include <QMovie>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "LiveCoffee.h"
#include "FriendCompanion.h"
#include "GameManager.h"


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
    m_pixelSize(pixelSize),
    m_auraItem(nullptr){

    m_movie = new QMovie(gif_path, QByteArray(), this);
    m_movie->setCacheMode(QMovie::CacheAll);
    connect(m_movie, &QMovie::frameChanged, this, &Tower::updatePixmapFromMovie);
    m_movie->start();
    if (m_movie->isValid()) {
        updatePixmapFromMovie();
    }
    setZValue(10);
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
        if (type == "WarmMemories") {
            updateVisualState(true);
        }
        if (currentTarget && targetIsInRange())
        {
            regretEffect();
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
    const QList<Tower*>& allTowers = GameManager::instance()->getTowers();
    for(auto& item :allTowers)
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
    const QList<Tower*>& allTowers = GameManager::instance()->getTowers();
    for(auto& item :allTowers)
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

QList<Enemy*> Tower::findRegretInRange()
{
    QList<Enemy*> regretEnemies;
    const QList<Enemy*>& allEnemies = GameManager::instance()->getEnemies();
    for(auto& item : allEnemies)
    {
        // 检查它是否是一个 Enemy
        Enemy* enemy = dynamic_cast<Enemy*>(item);
        // 检查它是否是 "regret" 类型
        if(enemy && enemy->getType() == "regret")
        {
            QLineF line(pos(), enemy->pos());
            // 假设 "regret" 的光环范围是 200 (你可以自定义)
            if(line.length() <= 200.0)
                regretEnemies.append(enemy);
        }
    }
    return regretEnemies;
}

void Tower::regretEffect()
{
    // 1. (重要) 先调用 stop 函数，
    //    这可以确保如果敌人走出了范围，塔能恢复正常攻速
    slowAttackStop();

    QList<Enemy*> auraEnemies = findRegretInRange();

    // 2. 如果找到了光环敌人
    if(!auraEnemies.isEmpty())
    {
        // 假设减速因子是 1.5 (即攻速降低 50%)
        // 你可以从 enemy_data.json 中读取这个值
        const double slowFactor = 1.5;
        slowAttack(slowFactor);
    }
}

void Tower::updateVisualState(bool isCharged) {
    if (type != "WarmMemories") {
        return; // 仅对 WarmMemory 执行此逻辑
    }

    QString targetPath;
    if (upgraded) {
        targetPath = isCharged ? m_upgradedGifPath : m_upgradedChargeGifPath;
    } else {
        targetPath = isCharged ? m_baseGifPath : m_chargeGifPath;
    }

    if (targetPath.isEmpty()) {
        qWarning() << "WarmMemory: Target GIF path is empty for state (Charged:" << isCharged << ", Upgraded:" << upgraded << ")";
        return;
    }

    // 检查是否已经是目标路径，避免重复加载
    if (m_movie && m_movie->fileName() == targetPath) {
        return;
    }

    // 停止当前电影
    if (m_movie) {
        m_movie->stop();
    }

    // 加载新 GIF 或 Pixmap
    if (targetPath.endsWith(".gif", Qt::CaseInsensitive)) {
        // 如果是 GIF，用 QMovie 替换
        if (m_movie) {
            m_movie->setFileName(targetPath);
        } else {
            // 理论上不会发生，因为 m_movie 在构造函数中创建
            m_movie = new QMovie(targetPath, QByteArray(), this);
            connect(m_movie, &QMovie::frameChanged, this, &Tower::updatePixmapFromMovie);
        }
        m_movie->start();
    } else {
        // 如果是 PNG/Pixmap，直接设置 Pixmap
        QPixmap staticPixmap(targetPath);
        if (staticPixmap.isNull()) {
            qWarning() << "WarmMemory: Failed to load static pixmap:" << targetPath;
            return;
        }
        QPixmap scaledPixmap = staticPixmap.scaled(m_pixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setPixmap(scaledPixmap);
    }
}

void Tower::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // 检查是否是鼠标右键
    if (event->button() == Qt::RightButton) {
        // 切换范围显示的可见性
        if (m_rangeCircle) {
            bool isVisible = m_rangeCircle->isVisible();
            showRange(!isVisible); // 取反：如果是显示的就隐藏，隐藏的就显示
        }
        event->accept(); // 标记事件已处理，不再传递
    }
    else {
        // 如果是左键（或其他键），我们必须【忽略】它
        // 这样事件才能“穿透”防御塔，传递给底下的 TowerBaseItem
        // 从而触发 MainWindow 中的升级/出售菜单逻辑
        event->ignore();

        // 或者调用基类实现（通常基类也会忽略左键，除非你设置了Flag）
        QGraphicsPixmapItem::mousePressEvent(event);
    }
}