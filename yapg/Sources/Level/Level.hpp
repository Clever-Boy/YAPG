#ifndef YAPG_GAME_LEVEL_LEVEL_H
#define YAPG_GAME_LEVEL_LEVEL_H

#include <boost/any.hpp>

#include <SFML/System/Vector2.hpp>

#include "entityx/entityx.h"

#include "Level/SerializedEntityGetter.hpp"
#include "Lua/EntityHandle.hpp"
#include "Lua/LuaState.hpp"

namespace yapg
{

class Level
{
public:
    enum class LevelMode
    {
        PlayMode,
        EditMode
    };

    Level(LuaState& luaState, LevelMode levelMode = LevelMode::PlayMode);

    void LoadFromFile(const std::string& path);
    void SaveToFile(const std::string& path);

    sf::Vector2f getSpawnPosition() const { return m_spawnPosition; }
    void setSpawnPosition(sf::Vector2f spawnPosition) { m_spawnPosition = spawnPosition; }

    const std::vector<std::string>& getPlayersTemplates() const { return m_playersTemplates; }
    std::vector<std::string>& getPlayersTemplates() { return m_playersTemplates; }

    const entityx::EventManager& getEventManager() const { return m_eventMgr; }
    entityx::EventManager& getEventManager() { return m_eventMgr; }

    const entityx::EntityManager& getEntityManager() const { return m_entityMgr; }
    entityx::EntityManager& getEntityManager() { return m_entityMgr; }

    entityx::Entity createNewEntity(const std::string& templateName, bool templateComponent = false);

    static void registerClass(LuaState& luaState);

private:
    LuaState& m_luaState;

    entityx::EventManager m_eventMgr;
    entityx::EntityManager m_entityMgr;

    sf::Vector2f m_spawnPosition;
    std::vector<std::string> m_playersTemplates;

    LevelMode m_levelMode;

    int m_nextId;
};

}

#endif