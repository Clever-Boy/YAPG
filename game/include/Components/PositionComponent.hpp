#ifndef YAPG_GAME_COMPONENTS_POSITIONCOMPONENT_H
#define YAPG_GAME_COMPONENTS_POSITIONCOMPONENT_H

#include "Components/Component.hpp"

namespace game
{
namespace components
{

class PositionComponent
{
public:
    PositionComponent();
    virtual ~PositionComponent();

    virtual std::string getName() const;

    virtual void loadFromLua(sel::Selector& luaSelector);

    int x;
    int y;
    int width;
    int height;
};

std::ostream& operator<<(std::ostream& stream, const PositionComponent& component);

}
}

#endif
