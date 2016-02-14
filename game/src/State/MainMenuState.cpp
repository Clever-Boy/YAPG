#include "State/MainMenuState.hpp"

namespace state
{

MainMenuState::MainMenuState(resources::TexturesManager& texturesManager, resources::FontManager& fontManager, settings::SettingsManager& settingsManager) :
    m_texturesManager(texturesManager),
    m_fontManager(fontManager),
    m_settingsManager(settingsManager),
    m_guiResGetter(resources::GuiResourcesGetter::create(m_fontManager))
{

}

void MainMenuState::onStart()
{

}

void MainMenuState::onStop()
{

}

void MainMenuState::onPause()
{

}

void MainMenuState::onUnpause()
{

}

void MainMenuState::processEvent(sf::Event event)
{

}

void MainMenuState::update(sf::Time dt)
{

}

void MainMenuState::render(sf::RenderTarget& target)
{
    target.clear(sf::Color(200, 200, 200));
}

}
