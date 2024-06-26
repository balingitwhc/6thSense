//
// Created by David Burchill on 2023-09-27.
//

#include "Entity.h"

sf::Sprite              m_sprite;

Entity::Entity(size_t id, const std::string &tag)
        : m_tag(tag), m_id(id) {

}

void Entity::destroy() {
    m_active = false;
}

const size_t &Entity::getId() const {
    return m_id;
}

const std::string &Entity::getTag() const {
    return m_tag;
}

bool Entity::isActive() const {
    return m_active;
}
