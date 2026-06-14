#ifndef _PUL_NETWORK_
#define _PUL_NETWORK_

#include <Config.hpp>

namespace Pulsar {
namespace Network {

enum DenyType {
    DENY_TYPE_NORMAL,
    DENY_TYPE_BAD_PACK,
    DENY_TYPE_OTT,
    DENY_TYPE_KICK,
};

static const u32 MAX_TRACK_BLOCKING = 12; // Maximum number of blocked tracks synced via packets

class Mgr { //Manages network related stuff within Pulsar
public:
    Mgr() : racesPerGP(3), curBlockingArrayIdx(0), lastGroupedTrackPlayed(false) {}
    u32 hostContext;
    DenyType denyType;
    u8 deniesCount;
    u8 ownStatusData;
    u8 statusDatas[30];
    u8 curBlockingArrayIdx;
    u8 racesPerGP;
    bool lastGroupedTrackPlayed;
    u8 padding[1];
    PulsarId* lastTracks;
};

}//namespace Network
}//namespace Pulsar

#endif