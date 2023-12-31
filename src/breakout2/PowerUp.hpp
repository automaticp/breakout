#pragma once
#include <cstddef>
#include <glm/glm.hpp>
#include <entt/entt.hpp>


enum class PowerUpType : size_t {
    none = 0,
    speed = 1,
    sticky = 2,
    pass_through = 3,
    pad_size_up = 4,
    confuse = 5,
    chaos = 6
};


struct PowerUpComponent {
    PowerUpType type;
};


void make_powerup(entt::registry& registry, class PhysicsSystem& physics, PowerUpType type, glm::vec2 pos);
