#include "Level/LevelState.hpp"

#include "State/StateEngine.hpp"

#include "Systems/CustomBehaviorSystem.hpp"
#include "Systems/HitboxUpdaterSystem.hpp"
#include "Systems/PlatformerSystem.hpp"
#include "Systems/PlayerSystem.hpp"
#include "Systems/RenderSystem.hpp"

namespace level
{

LevelState::LevelState(state::StateEngine& stateEngine, std::string path, resources::AllResourcesManagers& resourcesManager, settings::SettingsManager& settingsManager) :
    state::State(stateEngine),
    m_luaState(),
    m_level(m_luaState),
    m_systemMgr(m_level.getEntityManager(), m_level.getEventManager())
{
    m_systemMgr.add<systems::RenderSystem>(resourcesManager.getTextures());
    m_systemMgr.add<systems::CustomBehaviorSystem>();
    m_systemMgr.add<systems::PlatformerSystem>();
    m_systemMgr.add<systems::HitboxUpdaterSystem>();
    m_systemMgr.add<systems::PlayerSystem>(settingsManager);

    m_systemMgr.configure();

    m_level.LoadFromFile(path);
}

void LevelState::onStop()
{

}

void LevelState::onPause()
{

}

void LevelState::onUnpause()
{

}

void LevelState::processEvent(sf::Event event, sf::RenderTarget &target)
{

}

void LevelState::render(sf::RenderTarget& target)
{
    target.clear(sf::Color(0, 180, 255));
    m_systemMgr.system<systems::RenderSystem>()->render(target);
}

void LevelState::doStart()
{

}

void LevelState::doUpdate(sf::Time dt, sf::RenderTarget &target)
{
    m_systemMgr.update<systems::PlayerSystem>(dt.asSeconds());
    m_systemMgr.update<systems::HitboxUpdaterSystem>(dt.asSeconds());
    m_systemMgr.update<systems::PlatformerSystem>(dt.asSeconds());
    m_systemMgr.update<systems::CustomBehaviorSystem>(dt.asSeconds());
    m_systemMgr.update<systems::RenderSystem>(dt.asSeconds());
}

}