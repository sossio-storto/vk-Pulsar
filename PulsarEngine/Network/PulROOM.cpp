#include <kamek.hpp>
#include <MarioKartWii/RKNet/ROOM.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Settings/UI/SettingsPanel.hpp>
#include <Settings/Settings.hpp>
#include <Network/Network.hpp>
#include <Network/PacketExpansion.hpp>

namespace Pulsar {
namespace Network {

//Implements the ability for a host to send a message, allowing for custom host settings

//If we are in a room, we are guaranteed to be in a situation where Pul packets are being sent
//however, no reason to send the settings outside of START packets and if we are not the host, this is easily changed by just editing the check

static void ConvertROOMPacketToData(const PulROOM& packet) {
    System* system = System::sInstance;
    system->netMgr.hostContext = packet.hostSystemContext;
    system->netMgr.racesPerGP = packet.raceCount;
}

static void WriteBlockedTracksToPacket(PulROOM* packet) {
    System* system = System::sInstance;
    if (!system) return;

    const Network::Mgr& netMgr = system->netMgr;
    const u32 blockingCount = system->GetInfo().GetTrackBlocking();

    const u32 writeCount = (blockingCount < MAX_TRACK_BLOCKING) ? blockingCount : MAX_TRACK_BLOCKING;
    packet->blockedTrackCount = static_cast<u8>(writeCount);
    packet->curBlockingArrayIdx = netMgr.curBlockingArrayIdx;
    packet->lastGroupedTrackPlayed = netMgr.lastGroupedTrackPlayed;

    for (u32 i = 0; i < writeCount; ++i) {
        packet->blockedTracks[i] = (netMgr.lastTracks != nullptr && netMgr.lastTracks[i] != PULSARID_NONE) ? static_cast<u16>(netMgr.lastTracks[i]) : 0xFFFF;
    }
    for (u32 i = writeCount; i < MAX_TRACK_BLOCKING; ++i) {
        packet->blockedTracks[i] = 0xFFFF;
    }
}

static void BeforeROOMSend(RKNet::PacketHolder<PulROOM>* packetHolder, PulROOM* src, u32 len) {
    packetHolder->Copy(src, len); //default

    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    Pulsar::System* system = Pulsar::System::sInstance;
    PulROOM* destPacket = packetHolder->packet;
    if (destPacket->messageType == 1 && sub.localAid == sub.hostAid) {
        packetHolder->packetSize += sizeof(PulROOM) - sizeof(RKNet::ROOMPacket); //this has been changed by copy so it's safe to do this

        // Save original message before remapping.
        // Messages 4 and 5 are OPT WW and OTT WW starts (added by ExpFroomMessages).
        // Remap them to 0 or 1 so the base game handles them as a normal VS / Team VS start.
        const u8 originalMessage = destPacket->message;
        const Settings::Mgr& settings = Settings::Mgr::Get();
        const bool isStartMogi = settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_RADIO_MOGI) == 1;

        if (isStartMogi) {
            destPacket->message = 0; // Force Solo VS
        } else if (originalMessage >= 4 && originalMessage <= 8) {
            if (originalMessage == 8) { // Item Rain Team VS (8)
                destPacket->message = 1; // Team VS
            } else {
                destPacket->message = 0; // VS
            }
        }

        const u8 isStartVKWW = !isStartMogi && (originalMessage == 4);
        const u8 isStartOTWW  = !isStartMogi && (originalMessage == 5);
        const u8 isStartItemRainWW = !isStartMogi && (originalMessage == 6);
        const u8 isStartItemRainVS = !isStartMogi && (originalMessage == 7);
        const u8 isStartItemRainTeamVS = !isStartMogi && (originalMessage == 8);

        const u8 koSetting = settings.GetSettingValue(Settings::SETTINGSTYPE_KO, SETTINGKO_ENABLED) && destPacket->message == 0; //KO only enabled for normal GPs
        const u8 koFinal = settings.GetSettingValue(Settings::SETTINGSTYPE_KO, SETTINGKO_FINAL) == KOSETTING_FINAL_ALWAYS;
        //invert mii setting as the first button is enabled, not disabled, so a value of 1 indicates disabled
        const u8 ottOnline = settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_ONLINE);
        destPacket->hostSystemContext = (ottOnline != OTTSETTING_OFFLINE_DISABLED) << PULSAR_MODE_OTT //ott
            | (ottOnline == OTTSETTING_ONLINE_FEATHER) << PULSAR_FEATHER //ott feather
            | (settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_ALLOWUMTS) ^ true) << PULSAR_UMTS //ott umts
            | koSetting << PULSAR_MODE_KO
            | koFinal << PULSAR_KOFINAL
            | (!isStartMogi && (settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_ALLOW_MIIHEADS) ^ true)) << PULSAR_MIIHEADS
            | (!isStartMogi && settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_RADIO_HOSTWINS)) << PULSAR_HAW
            | (isStartMogi || (settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_RADIO_THUNDERCLOUD) == THUNDERCLOUD_NORMAL)) << PULSAR_THUNDERCLOUD
            | isStartVKWW << PULSAR_STARTVKWW   // OPT WW start from friend room
            | isStartOTWW  << PULSAR_STARTOTTWW  // OTT WW start from friend room
            | isStartItemRainWW << PULSAR_STARTITEMRAIN
            | (!isStartMogi && (isStartItemRainWW || isStartItemRainVS || isStartItemRainTeamVS)) << PULSAR_ITEMMODERAIN
            | isStartMogi << PULSAR_STARTMOGI;

        u8 raceCount;
        if (koSetting == KOSETTING_ENABLED) raceCount = 0xFE;
        else if (isStartMogi) raceCount = 3;
        else switch (settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_SCROLL_GP_RACES)) {
        case(0): // 4 races
            raceCount = 3;
            break;
        case(1): // 8 races
            raceCount = 7;
            break;
        case(2): // 12 races
            raceCount = 11;
            break;
        case(3): // 24 races
            raceCount = 23;
            break;
        case(4): // 32 races
            raceCount = 31;
            break;
        case(5): // 64 races
            raceCount = 63;
            break;
        case(6): // 2 races
            raceCount = 1;
            break;
        default:
            raceCount = 3;
        }
        destPacket->raceCount = raceCount;
        WriteBlockedTracksToPacket(destPacket);
        ConvertROOMPacketToData(*destPacket);
        system->SetContext(destPacket->hostSystemContext);
    }
}
kmCall(0x8065b15c, BeforeROOMSend);

kmWrite32(0x8065add0, 0x60000000);
static void AfterROOMReception(const RKNet::PacketHolder<PulROOM>* packetHolder, const PulROOM& src, u32 len) {
    register RKNet::ROOMPacket* packet;
    asm(mr packet, r28;);
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    Pulsar::System* system = Pulsar::System::sInstance;
    //START msg sent by the host, size check should always be guaranteed in theory
    if (src.messageType == 1 && sub.localAid != sub.hostAid && packetHolder->packetSize == sizeof(PulROOM)) {
        ConvertROOMPacketToData(src);

        // Apply host context locally on all non-host clients.
        const u32 hostContext = src.hostSystemContext;
        system->SetContext(hostContext);

        // Sync host's blocked tracks to the client
        const u32 localBlockingCount = system->GetInfo().GetTrackBlocking();
        if (localBlockingCount > 0 && system->netMgr.lastTracks != nullptr && src.blockedTrackCount > 0) {
            const u32 copyCount = (src.blockedTrackCount < localBlockingCount) ? src.blockedTrackCount : localBlockingCount;
            for (u32 i = 0; i < copyCount; ++i) {
                u16 track = src.blockedTracks[i];
                system->netMgr.lastTracks[i] = (track == 0xFFFF) ? PULSARID_NONE : static_cast<PulsarId>(track);
            }
            system->netMgr.curBlockingArrayIdx = src.curBlockingArrayIdx % localBlockingCount;
            system->netMgr.lastGroupedTrackPlayed = src.lastGroupedTrackPlayed;
        }

        //Also exit the settings page to prevent weird graphical artefacts
        Page* topPage = SectionMgr::sInstance->curSection->GetTopLayerPage();
        PageId topId = topPage->pageId;
        if (topId == UI::SettingsPanel::id) {
            UI::SettingsPanel* panel = static_cast<UI::SettingsPanel*>(topPage);
            panel->OnBackPress(0);
        }
    }
    memcpy(packet, &src, sizeof(RKNet::ROOMPacket)); //default
}
kmCall(0x8065add8, AfterROOMReception);

//Implements that setting
kmCall(0x806460B8, System::GetRaceCount);
kmCall(0x8064f51c, System::GetRaceCount);
}//namespace Network
}//namespace Pulsar
