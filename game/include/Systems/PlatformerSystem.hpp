#ifndef YAPG_GAME_SYSTEMS_PLATFORMERSYSTEM_H
#define YAPG_GAME_SYSTEMS_PLATFORMERSYSTEM_H

#include "entityx/entityx.h"

#include "Messaging/Messaging.hpp"
#include "Systems/HitboxUpdaterSystem.hpp"

namespace systems
{

class PlatformerSystem : public entityx::System<PlatformerSystem>, public messaging::Emitter
{
public:
    PlatformerSystem(HitboxUpdaterSystem::InfiniteQuadTreesGrid& quadtreesGrid);

    virtual void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt);

private:
    HitboxUpdaterSystem::InfiniteQuadTreesGrid& m_quadtreesGrid;
};

}

#endif
