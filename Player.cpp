#include "Player.h"

Player::Player(QObject *parent)
    : QObject(parent), m_dreamStability(0), m_resource(0) {
}

void Player::decreaseStability(int amount) {
    m_dreamStability -= amount;
    emit stabilityChanged(max(m_dreamStability, 0));
    if (m_dreamStability <= 0) {
        emit noStability();
    }
}

void Player::increaseStability(int amount) {
    m_dreamStability += amount;
    emit stabilityChanged(m_dreamStability);
}

void Player::addResource(int amount) {
    m_resource += amount;
    emit resourceChanged(m_resource);
}

bool Player::spendResource(int amount) {
    if (m_resource >= amount) {
        m_resource -= amount;
        emit resourceChanged(m_resource);
        return true;
    }
    return false;
}

int Player::getStability() const {
    return m_dreamStability;
}

int Player::getResource() const {
    return m_resource;
}

void Player::setInitialState(int stability, int resource) {
    m_dreamStability = stability;
    m_resource = resource;
    emit stabilityChanged(m_dreamStability);
    emit resourceChanged(m_resource);
}
