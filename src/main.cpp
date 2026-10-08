#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <fstream>
#include <vector>
#include <string>

using namespace geode::prelude;

namespace aleph {
struct Event { int frame; bool pressed; };

static std::string aurPath() {
    return (Mod::get()->getSaveDir() / "recorded.aur").string();
}

static void saveReplay(std::vector<Event> const& events) {
    std::ofstream out(aurPath(), std::ios::trunc);
    if (!out) return;
    out << "AUR 0.9\nGAME GeometryDash\nTPS 240\nINPUT PRIMARY\n\n";
    for (auto const& e : events)
        out << "FRAME " << e.frame << " " << (e.pressed ? "PRESS" : "RELEASE") << "\n";
}
}

class $modify(AlephBotPlayLayer, PlayLayer) {
    struct Fields {
        int frame = 0;
        bool recording = false;
        bool initialized = false;
        std::vector<aleph::Event> events;
        CCMenu* alephMenu = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setPosition({winSize.width - 48.f, 42.f});
        menu->setID("alephbot-menu");

        auto sprite = ButtonSprite::create("ALEPH", "bigFont.fnt", "GJ_button_04.png", 0.7f);
        sprite->setScale(0.55f);

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(AlephBotPlayLayer::onAlephBotButton)
        );
        button->setID("alephbot-button");

        menu->addChild(button);
        this->addChild(menu, 1000);
        m_fields->alephMenu = menu;

        return true;
    }

    void onAlephBotButton(CCObject*) {
        FLAlertLayer::create(
            "AlephBot v0.9-beta",
            "AUR: <cy>ready</c>\nFrame-based macro system: <cy>240 TPS</c>\n\n"
            "The recorder/player core is being connected now.",
            "OK"
        )->show();
    }

    void update(float dt) {
        PlayLayer::update(dt);
        if (!m_player1 || m_player1->m_isDead || m_hasCompletedLevel)
            return;

        if (!m_fields->initialized) {
            m_fields->initialized = true;
            m_fields->recording = true;
        }

        ++m_fields->frame;
        (void)dt;
    }

    void onExit() {
        if (m_fields->recording)
            aleph::saveReplay(m_fields->events);

        if (m_fields->alephMenu) {
            m_fields->alephMenu->removeFromParent();
            m_fields->alephMenu = nullptr;
        }

        PlayLayer::onExit();
    }
};
