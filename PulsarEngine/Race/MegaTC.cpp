#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Item/Obj/Kumo.hpp>
#include <MarioKartWii/Kart/KartMovement.hpp>
#include <PulsarSystem.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Settings/SettingsParam.hpp>

namespace Pulsar {
namespace Race {
//Mega TC
static bool IsPublicOnlineRoom() {
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::RoomType roomType = controller->roomType;
    const bool isFroom = roomType == RKNet::ROOMTYPE_FROOM_HOST || roomType == RKNet::ROOMTYPE_FROOM_NONHOST;
    return controller->connectionState != RKNet::CONNECTIONSTATE_SHUTDOWN && !isFroom && roomType != RKNet::ROOMTYPE_NONE;
}

void MegaTC(Kart::Movement& movement, int frames, int unk0, int unk1) {
    const System* system = System::sInstance;
    bool isMegaTC = system->IsContext(PULSAR_MEGATC) && !IsPublicOnlineRoom();
    if (system->IsContext(PULSAR_THUNDERCLOUD)) isMegaTC = false;
    if (isMegaTC)
        movement.ActivateMega();
    else
        movement.ApplyLightningEffect(frames, unk0, unk1);
}
kmCall(0x80580630, MegaTC);

void LoadCorrectTCBRRES(Item::ObjKumo& objKumo, const char* mdlName, const char* shadowSrc, u8 whichShadowListToUse,
                        Item::Obj::AnmParam* anmParam) {
    const System* system = System::sInstance;
    bool isMegaTC = system->IsContext(PULSAR_MEGATC) && !IsPublicOnlineRoom();
    if (system->IsContext(PULSAR_THUNDERCLOUD)) isMegaTC = false;
    if (isMegaTC)
        objKumo.LoadGraphics("megaTC.brres", mdlName, shadowSrc, 1, anmParam,
                             static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
    else
        objKumo.LoadGraphicsImplicitBRRES(mdlName, shadowSrc, 1, anmParam, static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr);
}
kmCall(0x807af568, LoadCorrectTCBRRES);


}//namespace Race
}//namespace Pulsar
