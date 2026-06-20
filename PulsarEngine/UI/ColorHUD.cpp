#include <VanzaKart.hpp>
#include <Settings/Settings.hpp>
#include <core/nw4r/lyt/Pane.hpp>

namespace Pulsar {
namespace UI {

static u8 hudR = 255;
static u8 hudG = 255;
static u8 hudB = 255;
static bool colorInitialized = false;

static const u8 hudColors[13][3] = {
    {255, 255, 255}, // White
    {60, 60, 60},    // Dark Gray/Black
    {198, 0, 0},     // Red
    {240, 136, 10},   // Orange
    {245, 200, 20},   // Yellow
    {2, 95, 2},     // Green
    {8, 39, 205},    // Blue
    {70, 15, 150},   // Purple
    {235, 105, 210}, // Pink
    {230, 0, 230},   // Magenta
    {36, 167, 240},  // Cyan
    {0, 160, 145},   // Teal
    {207, 160, 45}   // Gold
};

void UpdateHUDColor() {
    static u8 lastSetting = 255;
    u8 setting = Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_MENU, SETTINGMENU_SCROLL_HUDCOLOR);
    
    if (!colorInitialized || setting != lastSetting) {
        if (setting > 12) setting = 0;
        hudR = hudColors[setting][0];
        hudG = hudColors[setting][1];
        hudB = hudColors[setting][2];
        lastSetting = setting;
        colorInitialized = true;
    }
}

void GetHUDColor(void* self, RGBA16* c0, RGBA16* c1) {
    UpdateHUDColor();
    c0->red = hudR;
    c0->green = hudG;
    c0->blue = hudB;
    c0->alpha = 0xFD;
    c1->red = hudR;
    c1->green = hudG;
    c1->blue = hudB;
    c1->alpha = 0xFD;
}
kmBranch(0x805f03dc, GetHUDColor);
kmBranch(0x805f0440, GetHUDColor);

void GetHUDBaseColor(void* self, RGBA16* c) {
    UpdateHUDColor();
    c->red = 0;
    c->green = 0;
    c->blue = 0;
    c->alpha = 0x46;
}
kmBranch(0x805f04d8, GetHUDBaseColor);

void GetHUDRaceColor(nw4r::lyt::Pane* _this, u32 idx, nw4r::ut::Color color) {
    UpdateHUDColor();
    if (idx < 2) {
        color.r = hudR;
        color.g = hudG;
        color.b = hudB;
        color.a = 0xFD;
    } else {
        color.r = hudR > 20 ? hudR - 20 : 0;
        color.g = hudG > 20 ? hudG - 20 : 0;
        color.b = hudB > 20 ? hudB - 20 : 0;
        color.a = 0xFD;
    }
    _this->SetVtxColor(idx, color);
}
kmCall(0x807ec1dc, GetHUDRaceColor);

}
}
