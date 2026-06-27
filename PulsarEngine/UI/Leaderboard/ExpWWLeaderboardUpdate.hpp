#ifndef _EXP_WW_LEADERBOARD_
#define _EXP_WW_LEADERBOARD_
#include <MarioKartWii/UI/Page/Leaderboard/WWLeaderboardUpdate.hpp>

namespace Pulsar {
namespace UI {
class ExpWWLeaderboardUpdate : public Pages::WWLeaderboardUpdate {
   public:
    void OnUpdate() override;
};
}  // namespace UI
}  // namespace Pulsar
#endif
