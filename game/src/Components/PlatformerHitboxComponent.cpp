#include "Components/PlatformerHitboxComponent.hpp"

#include "Lua/EntityHandle.hpp"

namespace components
{

PlatformerHitboxComponent::PlatformerHitboxComponent(entityx::Entity entity) :
    Component(entity)
{

}

PlatformerHitboxComponent::~PlatformerHitboxComponent()
{

}

std::string PlatformerHitboxComponent::getName() const
{
    return "PlatformerHitbox";
}

void PlatformerHitboxComponent::registerComponent(lua::LuaState& state)
{
    meta::MetadataStore::registerClass<PlatformerHitboxComponent>("PlatformerHitboxComponent")
        .declareAttribute("polygon", &PlatformerHitboxComponent::m_polygon);

    lua::EntityHandle::declareComponent<PlatformerHitboxComponent>("platformer_hitbox");

    state.getState().new_usertype<PlatformerHitboxComponent>("platformer_hitbox_component",
        "polygon", &PlatformerHitboxComponent::m_polygon
    );
    state.declareComponentGetter<PlatformerHitboxComponent>("platformer_hitbox");
}

std::ostream& operator<<(std::ostream& stream, const PlatformerHitboxComponent& component)
{
    operator<<(stream, dynamic_cast<const Component&>(component));
    //TODO: Fill with specific infos
    return stream;
}

}
