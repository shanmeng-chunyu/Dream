#ifndef DREAM_PLAYER_H
#define DREAM_PLAYER_H

#include <QObject>

class Player : public QObject {
    Q_OBJECT

    public:
    explicit Player(QObject *parent = nullptr);

    void decreaseStability(int amount);

    void addResource(int amount);

    bool spendResource(int amount);

    int getStability() const;

    int getResource() const;

    void setInitialState(int stability, int resource);

    signals:

        void stabilityChanged(int newStability);

    void resourceChanged(int newResource);

    void noStability();

private:
    int m_dreamStability;
    int m_resource;
};

#endif //DREAM_PLAYER_H
