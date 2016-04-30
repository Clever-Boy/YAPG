#include "Systems/HitboxUpdaterSystem.hpp"

#include <SFML/System/Vector2.hpp>

namespace c = components;

namespace systems
{

HitboxUpdaterSystem::HitboxUpdaterSystem() :
    m_quadtreesGrid(500.f, 500.f, 16, 8)
{

}

void HitboxUpdaterSystem::initWithExistingEntities(entityx::EntityManager &es)
{
    es.each<c::PositionComponent, c::HitboxComponent>([&](
        entityx::Entity entity,
        c::PositionComponent& position,
        c::HitboxComponent& hitbox)
    {
        tryToAddEntityToQuadTree(entity);
    });
}

void HitboxUpdaterSystem::configure(entityx::EventManager &event_manager)
{
    event_manager.subscribe<entityx::ComponentAddedEvent<c::PositionComponent>>(*this);
    event_manager.subscribe<entityx::ComponentAddedEvent<c::HitboxComponent>>(*this);

    event_manager.subscribe<entityx::ComponentRemovedEvent<c::PositionComponent>>(*this);
    event_manager.subscribe<entityx::ComponentRemovedEvent<c::HitboxComponent>>(*this);
    event_manager.subscribe<entityx::EntityDestroyedEvent>(*this);
}

void HitboxUpdaterSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt)
{
    es.each<c::PositionComponent, c::HitboxComponent>([&](
        entityx::Entity entity,
        c::PositionComponent& position,
        c::HitboxComponent& hitbox)
    {
        if(hitbox.getHitbox().GetOrigin() == sf::Vector2f(position.x, position.y))
        {
            return;
        }

        hitbox.getHitbox().SetOrigin(sf::Vector2f(position.x, position.y));
        hitbox.getHitbox().ComputeGlobalVertices();
        hitbox.getHitbox().ComputeGlobalEdges();
        hitbox.getHitbox().ComputeGlobalCenter();

        m_quadtreesGrid.update(entity);
    });
}

void HitboxUpdaterSystem::receive(const entityx::ComponentAddedEvent<components::PositionComponent>& event)
{
    //tryToAddEntityToQuadTree(event.entity);
}

void HitboxUpdaterSystem::receive(const entityx::ComponentAddedEvent<components::HitboxComponent>& event)
{
    //tryToAddEntityToQuadTree(event.entity);
}

void HitboxUpdaterSystem::receive(const entityx::ComponentRemovedEvent<components::PositionComponent>& event)
{
    tryToRemoveEntityFromQuadTree(event.entity);
}

void HitboxUpdaterSystem::receive(const entityx::ComponentRemovedEvent<components::HitboxComponent>& event)
{
    tryToRemoveEntityFromQuadTree(event.entity);
}

void HitboxUpdaterSystem::receive(const entityx::EntityDestroyedEvent& event)
{
    tryToRemoveEntityFromQuadTree(event.entity);
}

void HitboxUpdaterSystem::tryToAddEntityToQuadTree(entityx::Entity entity)
{
    if(entity.has_component<c::PositionComponent>() && entity.has_component<c::HitboxComponent>())
    {
        m_quadtreesGrid.insert(entity);
    }
}

void HitboxUpdaterSystem::tryToRemoveEntityFromQuadTree(entityx::Entity entity)
{
    m_quadtreesGrid.erase(entity);
}

}
