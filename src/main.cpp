#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
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
    };

    void update(float dt) {
        PlayLayer::update(dt);
        if (!m_player1 || m_player1->m_isDead || m_hasCompletedLevel) return;
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
        PlayLayer::onExit();
    }
};
