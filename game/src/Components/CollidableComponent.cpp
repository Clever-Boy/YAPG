#include "Components/CollidableComponent.hpp"

#include "Lua/EntityHandle.hpp"

namespace components
{

CollidableComponent::CollidableComponent(entityx::Entity entity) :
    Component(entity)
{

}

CollidableComponent::~CollidableComponent()
{

}

std::string CollidableComponent::getName() const
{
    return "Collidable";
}

void CollidableComponent::registerComponent(lua::LuaState& state)
{
    meta::MetadataStore::registerClass<CollidableComponent>()
        .declareAttribute("on_collision_begin", &CollidableComponent::onCollisionBegin)
        .declareAttribute("collides", &CollidableComponent::collides)
        .declareAttribute("on_collision_end", &CollidableComponent::onCollisionEnd);

    lua::EntityHandle::declareComponent<CollidableComponent>("collidable");

    state.getState().new_usertype<CollidableComponent>("collidable_component",
        "on_collision_begin", &CollidableComponent::onCollisionBegin,
        "collides", &CollidableComponent::collides,
        "on_collision_end", &CollidableComponent::onCollisionEnd
    );
    state.declareComponentGetter<CollidableComponent>("collidable");
}

std::ostream& operator<<(std::ostream& stream, const CollidableComponent& component)
{
    operator<<(stream, dynamic_cast<const Component&>(component));
    //TODO: Fill with specific infos
    return stream;
}

}
