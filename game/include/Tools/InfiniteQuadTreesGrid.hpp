#ifndef YAPG_GAME_TOOLS_INFINITEQUADTREESGRID_H
#define YAPG_GAME_TOOLS_INFINITEQUADTREESGRID_H

#include <cmath>
#include <iostream>
#include <map>

#include "entityx/entityx.h"

#include "Components/PositionComponent.hpp"

namespace tools
{

class EntitySpatialGrid
{

public:
    EntitySpatialGrid(float gridWidth, float gridHeight) :
        m_gridWidth(gridWidth),
        m_gridHeight(gridHeight),
        m_grids()
    {

    }

    /**
     * Insert an object into the quadtrees-grid
     */
    bool insert(const entityx::Entity& value)
    {
        //Test if the object is already in the collection.
        if(m_assignments.count(value) >= 1)
            return false;

        sf::Rect<std::ptrdiff_t> gridIndexes = getGridIndexesFromAABB(getAABB(value));
        for(std::ptrdiff_t xIndex = gridIndexes.left; xIndex < gridIndexes.left + gridIndexes.width; ++xIndex)
        {
            for(std::ptrdiff_t yIndex = gridIndexes.top; yIndex < gridIndexes.top + gridIndexes.height; ++yIndex)
            {
                safeAt(xIndex, yIndex).insert(value);
                m_assignments[value].insert(&safeAt(xIndex, yIndex));
            }
        }

        return true;
    }

    /**
     * Tells the quadtrees-grid that an object AABB (position and size) was updated.
     */
    void update(const entityx::Entity& value)
    {
        //If not already in objects collection, insert it
        if(m_assignments.count(value) == 0)
        {
            insert(value);
            return;
        }

        sf::FloatRect newAABB = getAABB(value);

        //Make a copy. Avoids undefined behavior if the list of assigned quadtress changes during updates
        while(m_assignments[value].size() > 0)
        {
            (*(m_assignments[value].begin()))->erase(value);
            m_assignments[value].erase(m_assignments[value].begin());
        }

        //Create the non-existing quadtrees if needed
        sf::Rect<std::ptrdiff_t> gridIndexes = getGridIndexesFromAABB(newAABB);
        for(std::ptrdiff_t xIndex = gridIndexes.left; xIndex < gridIndexes.left + gridIndexes.width; ++xIndex)
        {
            for(std::ptrdiff_t yIndex = gridIndexes.top; yIndex < gridIndexes.top + gridIndexes.height; ++yIndex)
            {
                safeAt(xIndex, yIndex).insert(value); //This will create the quadtrees if needed.
                m_assignments[value].insert(&safeAt(xIndex, yIndex));
            }
        }
    }

    void erase(const entityx::Entity& value)
    {
        if(m_assignments.count(value) == 0)
        {
            return;
        }

        sf::Rect<std::ptrdiff_t> gridIndexes = getGridIndexesFromAABB(getAABB(value));
        for(std::ptrdiff_t xIndex = gridIndexes.left; xIndex < gridIndexes.left + gridIndexes.width; ++xIndex)
        {
            for(std::ptrdiff_t yIndex = gridIndexes.top; yIndex < gridIndexes.top + gridIndexes.height; ++yIndex)
            {
                safeAt(xIndex, yIndex).erase(value);
                m_assignments[value].erase(&safeAt(xIndex, yIndex));
            }
        }
    }

    bool contains(const entityx::Entity& value)
    {
        return m_assignments.count(value) >= 1;
    }

    std::set<entityx::Entity> getEntitiesIntersectingAABB(sf::FloatRect AABB) const
    {
        std::set<entityx::Entity> objects;

        sf::Rect<std::ptrdiff_t> gridIndexes = getGridIndexesFromAABB(AABB);
        for(std::ptrdiff_t xIndex = gridIndexes.left; xIndex < gridIndexes.left + gridIndexes.width; ++xIndex)
        {
            for(std::ptrdiff_t yIndex = gridIndexes.top; yIndex < gridIndexes.top + gridIndexes.height; ++yIndex)
            {
                if(m_grids.count(std::make_pair(xIndex, yIndex)) == 1)
                {
                    for(entityx::Entity object : m_grids.at(std::make_pair(xIndex, yIndex)))
                    {
                        if(getAABB(object).intersects(AABB))
                        {
                            objects.insert(object);
                        }
                    }
                }
            }
        }

        return objects;
    }

    void printContent()
    {
        std::cout << "Content of the Grid:" << std::endl;
        std::cout << "====================" << std::endl;
        /*for(const auto& quadtree : m_grids)
        {
            quadtree.second.printContent(2);
        }*/
    }

private:
    std::set<entityx::Entity>& safeAt(std::ptrdiff_t xIndex, std::ptrdiff_t yIndex)
    {
        return m_grids[std::make_pair(xIndex, yIndex)];
    }

    sf::Rect<std::ptrdiff_t> getGridIndexesFromAABB(sf::FloatRect AABB) const
    {
        return sf::Rect<std::ptrdiff_t>(
            static_cast<std::ptrdiff_t>(std::floor(AABB.left / m_gridWidth)),
            static_cast<std::ptrdiff_t>(std::floor(AABB.top / m_gridHeight)),
            static_cast<std::ptrdiff_t>(std::ceil((AABB.left + AABB.width) / m_gridWidth)) - static_cast<std::ptrdiff_t>(std::floor(AABB.left / m_gridWidth)),
            static_cast<std::ptrdiff_t>(std::ceil((AABB.top + AABB.height) / m_gridHeight)) - static_cast<std::ptrdiff_t>(std::floor(AABB.top / m_gridHeight))
        );
    }

    static sf::FloatRect getAABB(entityx::Entity entity)
    {
        return sf::FloatRect(
            entity.component<const components::PositionComponent>()->x,
            entity.component<const components::PositionComponent>()->y,
            entity.component<const components::PositionComponent>()->width,
            entity.component<const components::PositionComponent>()->height
        );
    }

    const float m_gridWidth;
    const float m_gridHeight;

    std::map<std::pair<std::ptrdiff_t, std::ptrdiff_t>, std::set<entityx::Entity>> m_grids;
    std::map<entityx::Entity, std::set<std::set<entityx::Entity>*>> m_assignments;
};

}

#endif
