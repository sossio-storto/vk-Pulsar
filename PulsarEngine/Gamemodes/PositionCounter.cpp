#include <Gamemodes/PositionCounter.hpp>
#include <MarioKartWii/3D/Camera/CameraMgr.hpp>
#include <MarioKartWii/Race/RaceData.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>
#include <Gamemodes/KO/KOMgr.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {

u8 PositionCounter::posTrackerAnmFrames[2] = {0, 0};

void PositionCounter::UpdatePositionDisplay(CtrlRaceRankNum& posTracker) {
    const System* system = System::sInstance;
    if (system == nullptr) return;

    const RacedataScenario& scenario = Racedata::sInstance->menusScenario;
    const GameMode mode = scenario.settings.gamemode;
    if (mode == MODE_BATTLE || mode == MODE_PUBLIC_BATTLE || mode == MODE_PRIVATE_BATTLE) return;

    const u8 hudSlotId = posTracker.hudSlotId;
    if (hudSlotId >= 2) return;

    lyt::Picture* posPane = static_cast<nw4r::lyt::Picture*>(posTracker.layout.GetPaneByName("position"));
    if (posPane == nullptr) return;

    ut::Color color = 0xffffffff;
    bool isInDanger = false;
    u8 playerId = 0xFF;

    // Determine player ID based on mode
    const Racedata* racedata = Racedata::sInstance;
    const Raceinfo* raceinfo = Raceinfo::sInstance;
    if (racedata == nullptr || raceinfo == nullptr) return;

    const bool isKO = system->IsContext(PULSAR_MODE_KO);

    if (!isKO) {
        // Not in KO mode, reset and return
        posTrackerAnmFrames[hudSlotId] = 0;
        return;
    }

    // Get player ID
    if (isKO && system->koMgr != nullptr && system->koMgr->isSpectating) {
        const RaceCameraMgr* cameraMgr = RaceCameraMgr::sInstance;
        if (cameraMgr != nullptr) {
            playerId = cameraMgr->focusedPlayerIdx;
        }
    } else {
        playerId = racedata->racesScenario.settings.hudPlayerIds[hudSlotId];
    }

    if (playerId >= 12) {
        posTrackerAnmFrames[hudSlotId] = 0;
        return;
    }

    // Check if player is in danger based on mode
    if (isKO && system->koMgr != nullptr) {
        if (raceinfo->raceFrames > 0) {
            isInDanger = system->koMgr->GetWouldBeKnockedOut(playerId);
        }
    }

    // Update animation and color
    if (isInDanger) {
        s32 increment = posTrackerAnmFrames[hudSlotId] >= 31 ? 8 : -8;
        color.g = posPane->vertexColours[0].g + increment;
        color.b = color.g;
        ++posTrackerAnmFrames[hudSlotId];
        if (posTrackerAnmFrames[hudSlotId] == 62) {
            posTrackerAnmFrames[hudSlotId] = 0;
        }
    } else {
        posTrackerAnmFrames[hudSlotId] = 0;
    }

    // Apply color to all vertices
    posPane->vertexColours[0] = color;
    posPane->vertexColours[1] = color;
    posPane->vertexColours[2] = color;
    posPane->vertexColours[3] = color;
}

void PositionCounter::UpdateAnimationFrame(u8 hudSlotId, bool isInDanger) {
    if (hudSlotId >= 2) return;

    if (isInDanger) {
        ++posTrackerAnmFrames[hudSlotId];
        if (posTrackerAnmFrames[hudSlotId] == 62) {
            posTrackerAnmFrames[hudSlotId] = 0;
        }
    } else {
        posTrackerAnmFrames[hudSlotId] = 0;
    }
}

void PositionCounter::ResetAnimationFrames() {
    posTrackerAnmFrames[0] = 0;
    posTrackerAnmFrames[1] = 0;
}

}  // namespace Pulsar
