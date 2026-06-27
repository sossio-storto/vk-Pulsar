#include <kamek.hpp>
#include <PulsarSystem.hpp>
#include <runtimeWrite.hpp>
#include <Dolphin/DolphinIOS.hpp>

namespace VanzaKart {

// Hide the channel button on the title screen / main menu on emulator only
kmRuntimeUse(0x80625E1C);
void HideChannelButton() {
    kmRuntimeWrite32A(0x80625E1C, 0x38800004);
    if (Dolphin::IsEmulator()) {
        kmRuntimeWrite32A(0x80625E1C, 0x38800003);
    }
}
static SectionLoadHook hideChannelButton(HideChannelButton);

} // namespace VanzaKart

