/**
 * @file
 *
 * \brief  Base networking class
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#include "net_base.hpp"
#include "packet.hpp"
#include "enet/enet.h"

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
NetBase::NetBase()
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
  const char *data = packet.get();
  ENetPacket *enet_packet = enet_packet_create(
    data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE
  );
  enet_peer_send(peer, channel_id, enet_packet);
}


}  // namespace net
