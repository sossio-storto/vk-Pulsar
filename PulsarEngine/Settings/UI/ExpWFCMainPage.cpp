#include <MarioKartWii/UI/Page/Other/GlobeSearch.hpp>
#include <MarioKartWii/RKSYS/RKSYSMgr.hpp>
#include <Settings/UI/ExpWFCMainPage.hpp>
#include <UI/UI.hpp>

namespace Pulsar {
namespace Network {
    extern u32 REGIONID;
}
namespace UI {
//EXPANDED WFC, keeping WW button and just hiding it in case it is ever needed...

kmWrite32(0x8064b984, 0x60000000); //nop the InitControl call in the init func
kmWrite24(0x80899a36, 'PUL'); //8064ba38
kmWrite24(0x80899a5B, 'PUL'); //8064ba90

ExpWFCMain::ExpWFCMain() {
    this->onSettingsClick.subject = this;
    this->onSettingsClick.ptmf = &ExpWFCMain::OnSettingsButtonClick;
    this->onButtonSelectHandler.ptmf = &ExpWFCMain::ExtOnButtonSelect;
}

void ExpWFCMain::OnInit() {
    Network::REGIONID = System::sInstance->GetInfo().GetWiimmfiRegion();
    this->InitControlGroup(6); //5 controls usually + settings button
    WFCMainMenu::OnInit();
    this->AddControl(5, settingsButton, 0);

    this->settingsButton.Load(UI::buttonFolder, "PULiMenuSingleTop", "Settings", 1, 0, false);
    this->settingsButton.buttonId = 5;
    this->settingsButton.SetOnClickHandler(this->onSettingsClick, 0);
    this->settingsButton.SetOnSelectHandler(this->onButtonSelectHandler);

    this->topSettingsPage = SettingsPanel::id;
}

void ExpWFCMain::OnSettingsButtonClick(PushButton& pushButton, u32 r5) {
    ExpSection::GetSection()->GetPulPage<SettingsPanel>()->prevPageId = PAGE_WFC_MAIN;
    this->nextPageId = static_cast<PageId>(this->topSettingsPage);
    this->EndStateAnimated(0, pushButton.GetAnimationFrameSize());
}

void ExpWFCMain::ExtOnButtonSelect(PushButton& button, u32 hudSlotId) {
    if(button.buttonId == 5) {
        u32 bmgId = BMG_SETTINGS_BOTTOM + 1;
        if(this->topSettingsPage == PAGE_VS_TEAMS_VIEW) bmgId += 1;
        else if(this->topSettingsPage == PAGE_BATTLE_MODE_SELECT) bmgId += 2;
        this->bottomText.SetMessage(bmgId, 0);
    }
    else this->OnButtonSelect(button, hudSlotId);
}

//ExpWFCModeSel

ExpWFCModeSel::ExpWFCModeSel() : lastClickedButton(0), submenuState(STATE_MAIN) {
    this->onButtonSelectHandler.ptmf = &ExpWFCModeSel::OnModeButtonSelect;
    this->onModeButtonClickHandler.ptmf = &ExpWFCModeSel::OnModeButtonClick;
    this->onBackButtonClickHandler.ptmf = &ExpWFCModeSel::OnBackButtonClick;
    this->onBackPressHandler.ptmf = &ExpWFCModeSel::OnBackPress;
}



static void SetTextBoxMessageSafe(LayoutUIControl& control, const char* textBoxName, u32 bmgId, const Text::Info* textInfo = nullptr) {
    if (control.layout.GetPaneByName(textBoxName) != nullptr) {
        control.SetTextBoxMessage(textBoxName, bmgId, textInfo);
    }
}

void ExpWFCModeSel::SetMenuTextAndRatings() {
    Text::Info vrInfo;
    memset(&vrInfo, 0, sizeof(Text::Info));
    Text::Info brInfo;
    memset(&brInfo, 0, sizeof(Text::Info));
    RKSYS::Mgr* rksysMgr = RKSYS::Mgr::sInstance;
    u32 vr = 0;
    u32 br = 5000;
    if(rksysMgr->curLicenseId >= 0) {
        RKSYS::LicenseMgr& license = rksysMgr->licenses[rksysMgr->curLicenseId];
        vr = license.vr.points;
        br = license.br.points;
    }

    wchar_t vrBuffer[32];
    swprintf(vrBuffer, 32, L"%u VR", vr);
    vrInfo.strings[0] = vrBuffer;

    wchar_t brBuffer[32];
    swprintf(brBuffer, 32, L"%u BR", br);
    brInfo.strings[0] = brBuffer;

    Pages::GlobeSearch* search = SectionMgr::sInstance->curSection->Get<Pages::GlobeSearch>();
    const u32 searchType = search != nullptr ? search->searchType : 0;

    if (searchType == 0) {
        SetTextBoxMessageSafe(this->vsButton, "text", BMG_VS_RESTORE); // VS
        SetTextBoxMessageSafe(this->vsButton, "text_light_01", BMG_VS_RESTORE);
        SetTextBoxMessageSafe(this->vsButton, "text_light_02", BMG_VS_RESTORE);
        SetTextBoxMessageSafe(this->vsButton, "go", BMG_TEXT, &vrInfo); // VR
        SetTextBoxMessageSafe(this->battleButton, "text", BMG_BATTLE_RESTORE); // Battle
        SetTextBoxMessageSafe(this->battleButton, "text_light_01", BMG_BATTLE_RESTORE);
        SetTextBoxMessageSafe(this->battleButton, "text_light_02", BMG_BATTLE_RESTORE);
        SetTextBoxMessageSafe(this->battleButton, "go", BMG_TEXT, &brInfo); // BR
    } else {
        if (this->submenuState == STATE_MAIN) {
            SetTextBoxMessageSafe(this->vsButton, "text", BMG_VS_WW_MENU); // VS WW
            SetTextBoxMessageSafe(this->vsButton, "text_light_01", BMG_VS_WW_MENU);
            SetTextBoxMessageSafe(this->vsButton, "text_light_02", BMG_VS_WW_MENU);
            SetTextBoxMessageSafe(this->vsButton, "go", BMG_TEXT, &vrInfo); // VR
            SetTextBoxMessageSafe(this->battleButton, "text", BMG_OTHER_VS_MENU); // Other VS
            SetTextBoxMessageSafe(this->battleButton, "text_light_01", BMG_OTHER_VS_MENU);
            SetTextBoxMessageSafe(this->battleButton, "text_light_02", BMG_OTHER_VS_MENU);
            SetTextBoxMessageSafe(this->battleButton, "go", BMG_TEXT, &vrInfo); // VR
        } else if (this->submenuState == STATE_VS_WW) {
            SetTextBoxMessageSafe(this->vsButton, "text", BMG_VANZA_VS); // Vanza VS
            SetTextBoxMessageSafe(this->vsButton, "text_light_01", BMG_VANZA_VS);
            SetTextBoxMessageSafe(this->vsButton, "text_light_02", BMG_VANZA_VS);
            SetTextBoxMessageSafe(this->vsButton, "go", BMG_TEXT, &vrInfo); // VR
            SetTextBoxMessageSafe(this->battleButton, "text", BMG_200CC_VANZA_VS); // 200cc Vanza VS
            SetTextBoxMessageSafe(this->battleButton, "text_light_01", BMG_200CC_VANZA_VS);
            SetTextBoxMessageSafe(this->battleButton, "text_light_02", BMG_200CC_VANZA_VS);
            SetTextBoxMessageSafe(this->battleButton, "go", BMG_TEXT, &vrInfo); // VR
        } else if (this->submenuState == STATE_OTHER_VS) {
            SetTextBoxMessageSafe(this->vsButton, "text", BMG_OTT_BUTTON); // OnlineTT
            SetTextBoxMessageSafe(this->vsButton, "text_light_01", BMG_OTT_BUTTON);
            SetTextBoxMessageSafe(this->vsButton, "text_light_02", BMG_OTT_BUTTON);
            SetTextBoxMessageSafe(this->vsButton, "go", BMG_TEXT, &vrInfo); // VR
            SetTextBoxMessageSafe(this->battleButton, "text", BMG_ITEMRAIN_WW_START_MESSAGE); // Item Rain WW
            SetTextBoxMessageSafe(this->battleButton, "text_light_01", BMG_ITEMRAIN_WW_START_MESSAGE);
            SetTextBoxMessageSafe(this->battleButton, "text_light_02", BMG_ITEMRAIN_WW_START_MESSAGE);
            SetTextBoxMessageSafe(this->battleButton, "go", BMG_TEXT, &vrInfo); // VR
        }
    }
}

void ExpWFCModeSel::OnActivatePatch() {
    register ExpWFCModeSel* page;
    asm(mr page, r29;);
    register Pages::GlobeSearch* search;
    asm(mr search, r30;);

    page->submenuState = STATE_MAIN;

    if (search->searchType == 0) {
        Network::REGIONID = System::sInstance->GetInfo().GetWiimmfiRegion();
        System::sInstance->netMgr.ownStatusData = false;
        page->lastClickedButton = 1;
        page->WFCModeSelect::OnModeButtonClick(page->vsButton, 0);
        return;
    }

    // VK Worldwide (search->searchType == 1)
    page->vsButton.isHidden = false;
    page->vsButton.manipulator.inaccessible = false;
    page->battleButton.isHidden = false;
    page->battleButton.manipulator.inaccessible = false;

    page->nextPage = PAGE_NONE;

    page->SetMenuTextAndRatings();
    page->vsButton.SelectInitial(0);
}
kmCall(0x8064c5f0, ExpWFCModeSel::OnActivatePatch);

void ExpWFCModeSel::OnModeButtonSelect(PushButton& modeButton, u32 hudSlotId) {
    Pages::GlobeSearch* search = SectionMgr::sInstance->curSection->Get<Pages::GlobeSearch>();
    const u32 searchType = search != nullptr ? search->searchType : 0;

    if (searchType == 0) {
        WFCModeSelect::OnModeButtonSelect(modeButton, hudSlotId);
    } else {
        if (this->submenuState == STATE_MAIN) {
            if (modeButton.buttonId == 1) {
                this->bottomText.SetMessage(BMG_VS_WW_MENU);
            } else {
                this->bottomText.SetMessage(BMG_OTHER_VS_MENU);
            }
        } else if (this->submenuState == STATE_VS_WW) {
            if (modeButton.buttonId == 1) {
                this->bottomText.SetMessage(0x10da);
            } else {
                this->bottomText.SetMessage(BMG_200CC_VANZA_VS_DESC);
            }
        } else if (this->submenuState == STATE_OTHER_VS) {
            if (modeButton.buttonId == 1) {
                this->bottomText.SetMessage(BMG_OTT_WW_BOTTOM);
            } else {
                this->bottomText.SetMessage(BMG_ITEMRAIN_WW_START_MESSAGE);
            }
        }
    }
}

void ExpWFCModeSel::OnModeButtonClick(PushButton& modeButton, u32 hudSlotId) {
    Pages::GlobeSearch* search = SectionMgr::sInstance->curSection->Get<Pages::GlobeSearch>();
    const u32 searchType = search != nullptr ? search->searchType : 0;

    if (searchType == 0) {
        WFCModeSelect::OnModeButtonClick(modeButton, hudSlotId);
        return;
    }

    const u32 clickedId = modeButton.buttonId;

    if (this->submenuState == STATE_MAIN) {
        if (clickedId == 1) {
            this->submenuState = STATE_VS_WW;
            this->SetMenuTextAndRatings();
            this->vsButton.SelectInitial(0);
        } else {
            this->submenuState = STATE_OTHER_VS;
            this->SetMenuTextAndRatings();
            this->vsButton.SelectInitial(0);
        }
    } else if (this->submenuState == STATE_VS_WW) {
        if (clickedId == 1) {
            Network::REGIONID = System::sInstance->GetInfo().GetWiimmfiRegion();
            System::sInstance->netMgr.ownStatusData = false;
            WFCModeSelect::OnModeButtonClick(modeButton, hudSlotId);
        } else {
            Network::REGIONID = 0x0C;
            System::sInstance->netMgr.ownStatusData = false;
            modeButton.buttonId = 1;
            WFCModeSelect::OnModeButtonClick(modeButton, hudSlotId);
            modeButton.buttonId = clickedId;
        }
    } else if (this->submenuState == STATE_OTHER_VS) {
        if (clickedId == 1) {
            Network::REGIONID = 0x69;
            System::sInstance->netMgr.ownStatusData = true;
            modeButton.buttonId = 1;
            WFCModeSelect::OnModeButtonClick(modeButton, hudSlotId);
            modeButton.buttonId = clickedId;
        } else {
            Network::REGIONID = 0x0D;
            System::sInstance->netMgr.ownStatusData = false;
            modeButton.buttonId = 1;
            WFCModeSelect::OnModeButtonClick(modeButton, hudSlotId);
            modeButton.buttonId = clickedId;
        }
    }
}

void ExpWFCModeSel::OnBackButtonClick(PushButton& backButton, u32 hudSlotId) {
    Pages::GlobeSearch* search = SectionMgr::sInstance->curSection->Get<Pages::GlobeSearch>();
    if (search != nullptr && search->searchType == 1 && this->submenuState != STATE_MAIN) {
        this->submenuState = STATE_MAIN;
        this->SetMenuTextAndRatings();
        this->vsButton.SelectInitial(0);
    } else {
        WFCModeSelect::OnBackButtonClick(static_cast<CtrlMenuBackButton&>(backButton), hudSlotId);
    }
}

void ExpWFCModeSel::OnBackPress(u32 hudSlotId) {
    Pages::GlobeSearch* search = SectionMgr::sInstance->curSection->Get<Pages::GlobeSearch>();
    if (search != nullptr && search->searchType == 1 && this->submenuState != STATE_MAIN) {
        this->submenuState = STATE_MAIN;
        this->SetMenuTextAndRatings();
        this->vsButton.SelectInitial(0);
    } else {
        WFCModeSelect::OnBackPress(hudSlotId);
    }
}

}//namespace UI
}//namespace Pulsar