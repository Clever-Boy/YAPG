#include "Components/PlatformerComponent.hpp"

namespace game
{
namespace components
{

PlatformerComponent::PlatformerComponent() :
    Component(),
    layer(0),
    maxFallingSpeed(700.f),
    maxJumpingSpeed(900.f),
    currentSpeed(0),
    fallingSpeed(0),
    jumpingSpeed(0),
    acceleration(1500.f),
    deceleration(1500.f),
    maxSpeed(250.f),
    gravity(1000.f),
    canJumpAgain(false),
    groundEntity(),
    oldFloorPosX(0),
    oldFloorPosY(0),
    movementStateCallbacks(Idle)
{

}

PlatformerComponent::~PlatformerComponent()
{

}

std::string PlatformerComponent::getName() const
{
    return "Platformer";
}

void PlatformerComponent::registerComponent(lua::LuaState& state)
{
    meta::MetadataStore::registerType<PlatformerComponent>()
        .declareAttribute("onIdle", &PlatformerComponent::onIdleFunc)
        .declareAttribute("onStartWalking", &PlatformerComponent::onWalkingFunc)
        .declareAttribute("onStartJumping", &PlatformerComponent::onJumpingFunc)
        .declareAttribute("onStartFalling", &PlatformerComponent::onFallingFunc)
        .setExtraLoadFunction([](PlatformerComponent* c, const sol::object& luaObject) {
            c->movementStateCallbacks.registerCallback(State::Idle, c->onIdleFunc);
            c->movementStateCallbacks.registerCallback(State::Walking, c->onWalkingFunc);
            c->movementStateCallbacks.registerCallback(State::Jumping, c->onJumpingFunc);
            c->movementStateCallbacks.registerCallback(State::Falling, c->onFallingFunc);
        });

    state.getState().new_usertype<PlatformerComponent>("platformer_component" //TODO: Replace the name here
        //TODO: Register the properties here
    );
}

std::ostream& operator<<(std::ostream& stream, const PlatformerComponent& component)
{
    operator<<(stream, dynamic_cast<const Component&>(component));
    //TODO: Fill with specific infos
    return stream;
}

}
}
