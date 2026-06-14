#ifndef _PUL_WFC_
#define _PUL_WFC_
#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Other/WFCMenu.hpp>
#include <Settings/UI/SettingsPanel.hpp>

//Extends WFCMainMenu to add a settings button
namespace Pulsar {
namespace UI {
class ExpWFCMain : public Pages::WFCMainMenu {
public:
    ExpWFCMain();
    void OnInit() override;
private:
    void OnSettingsButtonClick(PushButton& PushButton, u32 r5);
    void ExtOnButtonSelect(PushButton& pushButton, u32 hudSlotId);
    PtmfHolder_2A<ExpWFCMain, void, PushButton&, u32> onSettingsClick;
    PushButton settingsButton;
public:
    PulPageId topSettingsPage;
};

class ExpWFCModeSel : public Pages::WFCModeSelect {
public:
    enum SubmenuState {
        STATE_MAIN = 0,
        STATE_VS_WW = 1,
        STATE_OTHER_VS = 2
    };

    ExpWFCModeSel();
    static void OnActivatePatch();
    void SetMenuTextAndRatings();
private:
    void OnModeButtonSelect(PushButton& modeButton, u32 hudSlotId); //8064c718
    void OnModeButtonClick(PushButton& PushButton, u32 r5);
    void OnBackButtonClick(PushButton& backButton, u32 hudSlotId);
    void OnBackPress(u32 hudSlotId);
    
    u32 lastClickedButton;
    u32 submenuState;
};
}//namespace UI
}//namespace Pulsar

#endif