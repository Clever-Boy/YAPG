#ifndef YAPG_GAME_MESSAGING_LEVELMESSAGES_H
#define YAPG_GAME_MESSAGING_LEVELMESSAGES_H

#include "Messaging/Message.hpp"

namespace messaging
{

struct PlayerFinishedMessage
{
    int playerNumber;
};

struct AllPlayersFinishedMessage
{

};

}

#endif
