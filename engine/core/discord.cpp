#include "discord.hpp"

#include <cstring>
#include <ctime>
#include <discord-rpc/include/discord_register.h>
#include <discord-rpc/include/discord_rpc.h>

namespace VGED {
    namespace Engine {
        void RPC::init() {
            DiscordEventHandlers handler;
            memset(&handler, 0, sizeof(handler));
            Discord_Initialize("1013896979954290799", &handler, 1, NULL);

            DiscordRichPresence rpc;
            memset(&rpc, 0, sizeof(rpc));
            rpc.state = "Pogging off";
            rpc.details = "Pogging off";
            rpc.startTimestamp = std::time(0);
            rpc.largeImageKey = "VGED Engine";
            rpc.largeImageText = "This is my game engine.";
            Discord_UpdatePresence(&rpc);
        }

        void RPC::update() {
            DiscordRichPresence rpc;
            memset(&rpc, 0, sizeof(rpc));
            rpc.state = "Pogging off";
            rpc.details = "Pogging off";
            rpc.largeImageKey = "chad";
            rpc.smallImageKey = "chad";
            rpc.startTimestamp = std::time(0);
            rpc.largeImageKey = "VGED Engine";
            rpc.largeImageText = "This is my game engine.";
            Discord_UpdatePresence(&rpc);
        }
    }
}