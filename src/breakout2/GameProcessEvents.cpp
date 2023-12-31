#include "FXType.hpp"
#include "Game.hpp"
#include "Events.hpp"
#include "PowerUp.hpp"
#include "Tile.hpp"
#include "Transform2D.hpp"
#include <entt/entt.hpp>
#include <type_traits>


void Game::process_events() {
    process_input_events();
    process_tile_collision_events();
    process_powerup_collision_events();
    process_fx_state_updates();
}


void Game::process_input_events() {

    auto& imc = registry_.get<InputMoveComponent>(player_);

    while (!events.input.empty()) {
        auto event = events.input.pop();

        switch (event) {
            using enum InputEvent;
            case lmove:
                imc.wants_move_left = true;
                break;
            case lstop:
                imc.wants_move_left = false;
                break;
            case rmove:
                imc.wants_move_right = true;
                break;
            case rstop:
                imc.wants_move_right = false;
                break;
            case launch_ball:
                this->launch_ball();
                break;
            case exit:
                window_.setShouldClose(true);
                break;
            default:
                break;
        }
    }
}

void Game::process_tile_collision_events() {
    while (!events.tile_collision.empty()) {
        auto event = events.tile_collision.pop();

        auto tile_entity = event.tile_entity;

        const TileType type = registry_.get<TileComponent>(tile_entity).type;

        if (type != TileType::solid) {
            const glm::vec2 position = registry_.get<Transform2D>(tile_entity).position;

            trash_.destroy_later(tile_entity);

            // FIXME: Maybe send a 'create powerup' event instead?
            powerup_generator_.try_generate_random_at(registry_, physics_, position);

        } else /* solid */ {
            fx_manager_.enable_for(FXType::shake, 0.05f);
        }
    }
}



static FXType powerup_to_fx(PowerUpType powerup_type) noexcept {
    // FIXME: Fragile conversion depends on the underlying values.
    return FXType{ std::underlying_type_t<PowerUpType>(powerup_type) };
}


void Game::process_powerup_collision_events() {
    while (!events.powerup_collision.empty()) {
        auto event = events.powerup_collision.pop();

        if (event.type == PowerUpCollisionType::with_paddle) {

            const PowerUpType powerup_type =
                registry_.get<PowerUpComponent>(event.powerup_entity).type;

            fx_manager_.enable(powerup_to_fx(powerup_type));
        }

        trash_.destroy_later(event.powerup_entity);
    }

}


void Game::process_fx_state_updates() {
    while (!events.fx_toggle.empty()) {
        auto event = events.fx_toggle.pop();

        switch (event.type) {
            case FXType::speed:
            {
                auto& imc = registry_.get<InputMoveComponent>(player_);
                if (event.toggle_type == FXToggleType::enable) {
                    imc.max_velocity = player_base_speed * 1.3f;
                } else {
                    imc.max_velocity = player_base_speed;
                }
            } break;
            case FXType::pad_size_up: // TODO
            case FXType::pass_through: // TODO
            case FXType::sticky: // TODO
            // Visual effect states are queried directly
            // from FXStateManager in the render() call.
            // So no updates here.
            case FXType::shake:
            case FXType::chaos:
            case FXType::confuse:
            default:
                break;
        }
    }

}



void Game::launch_ball() {
    // TODO: move the joint into something better than a raw ptr member var
    if (sticky_joint_) {
        physics_.unweld(sticky_joint_);
        sticky_joint_ = nullptr;

        auto& player_p = registry_.get<const PhysicsComponent>(player_);

        registry_.patch<PhysicsComponent>(ball_, [&](PhysicsComponent& p) {
            p.set_velocity(
                ball_base_speed * glm::normalize(
                    glm::vec2{ 0.f, ball_base_speed } + player_p.get_velocity()
                )
            );
        });

    }
}


