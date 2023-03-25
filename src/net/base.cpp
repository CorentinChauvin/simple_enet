/**
 * @file
 *
 * \brief  Base networking class
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#include "base.hpp"
#include "packet.hpp"
#include "enet/enet.h"
#include <string>

#include <cstring>

#include <iostream>
using std::cout;
using std::endl;


namespace net
{

// =============================================================================
// NetHost
//
NetHost::NetHost():
  is_host_created_(false)
{

}

NetHost::~NetHost()
{
  if (is_host_created_)
    destroy();
}


bool NetHost::create(
  const ENetAddress *address,
  size_t peerCount,
  size_t channelLimit,
  enet_uint32 incomingBandwidth,
  enet_uint32 outgoingBandwidth
)
{
  if (is_host_created_)
    destroy();

  host_ = enet_host_create(
    address, peerCount, channelLimit, incomingBandwidth, outgoingBandwidth
  );

  if (host_ != nullptr) {
    is_host_created_ = true;
    return true;
  }

  return false;
}


void NetHost::destroy()
{
  if (is_host_created_) {
    enet_host_destroy(host_);
    is_host_created_ = false;
  }
}


ENetHost* NetHost::get()
{
  return host_;
}


// =============================================================================
// NetBase
//
NetBase::NetBase(const std::string &validation_salt):
  validation_salt_(validation_salt)
{

}


bool NetBase::init()
{
  // Initialise ENet
  if (enet_initialize () != 0)
  {
    cout << "An error occurred while initializing ENet" << endl;
    return false;
  }
  atexit(enet_deinitialize);

  return true;
}


void NetBase::handle_events()
{
  ENetEvent event;

  while (enet_host_service(host_.get(), &event, 0) > 0)
  // while (enet_host_service(host_.get(), &event, 0) > 0)
  {
    switch (event.type)
    {
      case ENET_EVENT_TYPE_CONNECT:
        connect_cb(event);
        break;

      case ENET_EVENT_TYPE_RECEIVE:
        receive_cb(event);
        enet_packet_destroy(event.packet);
        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        disconnect_cb(event);
        event.peer->data = nullptr;
        break;

      case ENET_EVENT_TYPE_NONE:
        no_event_cb();
        break;

      default:
        break;
    }
  }
}


ENetHost* NetBase::get_host()
{
  return host_.get();
}


void NetBase::send_packet(ENetPeer *peer, const Packet &packet, int channel_id)
{
  std::string data = packet.serialise();
  ENetPacket *enet_packet = enet_packet_create(
    data.c_str(), data.size() + 1, ENET_PACKET_FLAG_RELIABLE
  );
  enet_peer_send(peer, channel_id, enet_packet);
}


std::string NetBase::solve_validation_puzzle(const std::string &validation_str) const
{
  std::string salt = validation_salt_;
  std::string solution;
  auto n = validation_str.size();
  auto m = salt.size();
  solution.reserve(n);

  std::string chrs = "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  for (int k = 0; k < (int)n; k++) {
    int index = (validation_str[k] << 3) ^ (validation_str[k] | salt[k % m]);
    solution += chrs[index % (int)chrs.size()];
  }

  return solution;
}


}  // namespace net
