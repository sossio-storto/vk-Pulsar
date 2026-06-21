#ifndef _VANZAKART_CHANNEL_
#define _VANZAKART_CHANNEL_

#include <kamek.hpp>

namespace Pulsar {

// Signature written by the launcher.
#define RRC_SIGNATURE_ADDRESS 0x817ffff8
#define RRC_ABI_VERSION_ADDRESS 0x817ffffc
#define RRC_SIGNATURE 0xDEADBEEF

// The "ABI version" of the channel
#define RRC_ABI_VERSION 1

// Bitflags for launcher configuration
#define RRC_BITFLAGS_ADDRESS 0x817ffff0
#define RRC_BITFLAG_SEPARATE_SAVEGAME 0x1

#define RRC_LOADED_FROM_RR_EPH_FILE_PATH "/VanzaKartChannel/.lfrr"
#define RRC_CRASH_EPH_FILE_PATH "/VanzaKartChannel/.crash"

bool IsNewChannel();
bool NewChannel_UseSeparateSavegame();
void NewChannel_WriteLoadedFromRREphFile();
void NewChannel_WriteCrashEphFile();
void NewChannel_Init();

}  // namespace Pulsar

#endif
