#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Other/GlobeSearch.hpp>
#include <PulsarSystem.hpp>
#include <UI/UI.hpp>
#include <MarioKartWii/3D/GlobeMgr.hpp>

kmWrite32(0x80609268, 0x7f63db78);
namespace Pulsar {
namespace UI {

void PatchGlobeSearchBMG(Pages::GlobeSearch* globeSearch) {
    globeSearch->countdown.Update();
    if(System::sInstance->netMgr.deniesCount >= 3) globeSearch->messageWindow.LayoutUIControl::SetMessage(UI::BMG_TOO_MANY_DENIES);

    GlobeMgr* globeMgr = GlobeMgr::sInstance;
    if (globeMgr != nullptr && globeMgr->earthmodel != nullptr) {
        if (globeMgr->earthmodel->isMiiShown) {
            globeMgr->earthmodel->isMiiShown = false;
            globeMgr->ResetGlobeMii();
        }
    }
}
kmCall(0x8060926c, PatchGlobeSearchBMG);

}//namespace UI
}//namespace Pulsar