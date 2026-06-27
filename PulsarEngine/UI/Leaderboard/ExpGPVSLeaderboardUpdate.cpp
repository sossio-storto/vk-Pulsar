#include <UI/Leaderboard/ExpGPVSLeaderboardUpdate.hpp>
#include <UI/Leaderboard/LeaderboardDisplay.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace UI {

void ExpGPVSLeaderboardUpdate::OnUpdate() {
    if (checkLeaderboardDisplaySwapInputs()) {
        nextLeaderboardDisplayType();
        fillLeaderboardResults(GetRowCount(), this->results);
    }
}

void ExpGPVSLeaderboardUpdate::BeforeEntranceAnimations() {
    setLeaderboardDisplayType(LEADERBOARD_DISPLAY_NAMES);

    if (System::sInstance->IsContext(PULSAR_MODE_OTT)) {
        setLeaderboardDisplayType(LEADERBOARD_DISPLAY_TIMES);
    }

    fillLeaderboardResults(GetRowCount(), this->results);
}

PageId ExpGPVSLeaderboardUpdate::GetNextPage() const {
    return Pages::GPVSLeaderboardUpdate::GetNextPage();
}

}  // namespace UI
}  // namespace Pulsar
