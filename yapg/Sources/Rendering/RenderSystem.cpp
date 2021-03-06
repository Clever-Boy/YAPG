#include "Rendering/RenderSystem.hpp"

#include <algorithm>

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "Common/PositionComponent.hpp"
#include "Lua/EntityHandle.hpp"
#include "Template/TemplateComponent.hpp"
#include "Platformer/PlatformerHitboxComponent.hpp"
#include "Platformer/PlatformerComponent.hpp"
#include "Player/PlayerComponent.hpp"
#include "Rendering/RenderComponent.hpp"

namespace yapg
{

RenderSystem::RenderSystem(TexturesManager& texturesManager, EntitySpatialGrid& grid, bool cameraFollowPlayers, bool debugHitboxDraw) :
    entityx::System<RenderSystem>(),
    m_cameraFollowPlayers(cameraFollowPlayers),
    m_renderingQueue(),
    m_viewInit(false),
    m_renderingView(),
    m_centerOnPlayer(0),
    m_lastGroundEntity(),
    m_animatedSprites(),
    m_texturesManager(texturesManager),
    m_debugHitboxDraw(debugHitboxDraw),
    m_grid(grid)
{

}

void RenderSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt)
{
    m_renderingQueue.clear();

    if(m_debugHitboxDraw)
    {
        es.each<PositionComponent, PlatformerHitboxComponent>([&](
            entityx::Entity entity,
            PositionComponent& position,
            PlatformerHitboxComponent& hitbox) {
            auto shape = std::make_shared<sf::ConvexShape>(hitbox.getHitbox().getPointsCount());
            shape->setOutlineThickness(1.f);
            shape->setFillColor(sf::Color::Transparent);
            shape->setOutlineColor(sf::Color::Black);
            for(std::size_t i = 0; i < shape->getPointCount(); ++i)
            {
                shape->setPoint(i, sf::Vector2f(
                    hitbox.getHitbox().getPoint(i).x,
                    hitbox.getHitbox().getPoint(i).y
                ));
            }

            addToRenderingQueue(shape, sf::RenderStates(position.getPositionTransform()), 100000.f);
        });
    }

    auto drawFunc = [&](entityx::Entity entity, PositionComponent& position, RenderComponent& render) {
        //Update the animated sprite and put it in the render queue
        auto animatedSprite = getAnimatedSprite(entity);

        animatedSprite->update(dt);
        animatedSprite->setOrigin(sf::Vector2f(0.5f, 0.5f));
        animatedSprite->setPosition(position.x + position.width/2.f, position.y + position.height/2.f);
        animatedSprite->setScale((render.flipped ? (-1) : (1)) * position.width, position.height);

        //Call the lua callback if the animation has just been restarted
        if(animatedSprite->hadRestartedAnimation() && render.onAnimationEndFunc.valid())
            render.onAnimationEndFunc.call(EntityHandle(entity), render.currentAnimation);

        addToRenderingQueue(animatedSprite, sf::RenderStates::Default, position.z); //TODO: Get z position from RenderComponent
    };

    sf::FloatRect viewAABB(
        m_renderingView.getCenter().x - 1024.f/2.f,
        m_renderingView.getCenter().y - 768.f/2.f,
        1024.f,
        768.f
    );

    std::set<entityx::Entity> entitiesToDraw = m_grid.getEntitiesIntersectingAABB(viewAABB);
    for(entityx::Entity entity : entitiesToDraw)
    {
        if(entity.has_component<PositionComponent>() && entity.has_component<RenderComponent>())
        {
            drawFunc(entity, *(entity.component<PositionComponent>().get()), *(entity.component<RenderComponent>().get()));
        }
    }

    if(m_cameraFollowPlayers)
    {
        //Update the camera
        es.each<PlayerComponent, PlatformerComponent, PositionComponent>([&](entityx::Entity entity, PlayerComponent& player, PlatformerComponent& platformer, PositionComponent& position) {
            if(player.playerNumber != m_centerOnPlayer)
                return; //Only center on the selected player

            float newX = position.x + position.width / 2.f;
            float newY = m_renderingView.getCenter().y;

            if(platformer.groundEntity && platformer.groundEntity.has_component<PositionComponent>())
            {
                m_lastGroundEntity = platformer.groundEntity;
            }

            if(m_lastGroundEntity)
            {
                auto groundPosition = m_lastGroundEntity.component<PositionComponent>();

                //If the player is on the ground, update to Y position.
                if(std::abs(newY - groundPosition->y) > 10.f)
                {
                    newY += (newY > groundPosition->y ? -1 : 1) * 150.f * dt;
                }

                //If the player goes beyond some limits, remove the last ground entity and try to center on player
                if(position.y + position.height > newY + 200.f || position.y + position.height < newY - 200.f)
                {
                    m_lastGroundEntity = entityx::Entity();
                }
            }
            else
            {
                newY += (newY > position.y ? -1 : 1) * std::abs(newY - position.y) * 2 * dt + (platformer.fallingSpeed - platformer.jumpingSpeed) * dt;
            }

            m_renderingView.setCenter(sf::Vector2f(newX, newY));
        });
    }
}

void RenderSystem::render(sf::RenderTarget& target)
{
    if(!m_viewInit)
    {
        m_renderingView = target.getView();
        m_viewInit = true;
    }

    sf::View oldView = target.getView();

    target.setView(m_renderingView);

    for(auto it = m_renderingQueue.cbegin(); it != m_renderingQueue.cend(); ++it)
    {
        target.draw(*(it->drawable), it->states);
    }

    target.setView(oldView);
}

void RenderSystem::receive(const AnimationChangedMessage& msg)
{
    auto animatedSprite = getAnimatedSprite(msg.entity);
    auto render = entityx::Entity(msg.entity).component<RenderComponent>();
    if(render->currentAnimation != animatedSprite->getCurrentAnimation())
    {
        std::string oldAnimation = animatedSprite->getCurrentAnimation();
        animatedSprite->setCurrentAnimation(render->currentAnimation);

        if(render->onAnimationChangedFunc.valid())
            render->onAnimationChangedFunc.call(EntityHandle(msg.entity), oldAnimation, render->currentAnimation);
    }
}

void RenderSystem::addToRenderingQueue(std::shared_ptr<sf::Drawable> drawable, sf::RenderStates states, float z)
{
    auto insertionIt = std::lower_bound(m_renderingQueue.begin(), m_renderingQueue.end(), z, [](Renderable& renderable, float z) { return renderable.z < z; });
    m_renderingQueue.insert(insertionIt, Renderable{drawable, states, z});
}

std::shared_ptr<AnimatedSprite> RenderSystem::getAnimatedSprite(entityx::Entity entity)
{
    auto render = entityx::Entity(entity).component<RenderComponent>();
    auto templateComp = entityx::Entity(entity).component<TemplateComponent>();

    if(m_animatedSprites.count(entity) == 0)
    {
        //Create the animated sprite if it doesn't exist
        auto animatedSprite = std::make_shared<AnimatedSprite>(
            m_texturesManager.requestResource(templateComp->assetsPath + "/" + render->textureName),
            render->animations
        );
        animatedSprite->setCurrentAnimation(render->currentAnimation);
        m_animatedSprites[entity] = animatedSprite;
    }

    return m_animatedSprites[entity];
}

}
