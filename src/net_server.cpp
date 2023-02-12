/**
 * @file
 *
 * \brief  Base server class
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#include "net_server.hpp"
#include "enet/enet.h"
#include <memory>

#include <chrono>
#include <thread>

#include <stdio.h>
#include <cstring>
#include <string>


namespace net
{

// =============================================================================
// ServerPeers
//
void ServerPeers::add_peer(ENetPeer *peer, ServerPeers::Peer::Status new_status)
{
  if (get_peer(peer) == nullptr) {
    Peer new_peer = {
      .peer = peer,
      .status = new_status
    };

    peers_.push_back(new_peer);
  }
}


ServerPeers::Peer* ServerPeers::get_peer(ENetPeer *peer)
{
  for (Peer& handled_peer: peers_) {
    if (handled_peer.peer == peer)
      return &handled_peer;
  }

  return nullptr;
}


void ServerPeers::remove_peer(ENetPeer *peer)
{
  for (unsigned int k = 0; k < peers_.size(); k++) {
    if (peers_[k].peer == peer) {
      peers_.erase(peers_.begin() + k);
      break;
    }
  }
}


// =============================================================================
// NetServer
//
bool NetServer::init()
{
  if (!NetBase::init())
    return false;


  ENetAddress address;
  // Bind the server to the default localhost.
  // A specific host address can be specified by
  // enet_address_set_host (& address, "x.x.x.x");
  address.host = ENET_HOST_ANY;
  address.port = 1234;  // Bind the server to port 1234.

  bool success = host_.create(
    &address,  // the address to bind the server host to
    32,        // allow up to 32 clients and/or outgoing connections
    2,         // allow up to 2 channels to be used, 0 and 1
    0,         // assume any amount of incoming bandwidth
    0          // assume any amount of outgoing bandwidth
  );

  if (!success) {
    fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
    return false;
  }

  return true;
}


void NetServer::connect_cb(ENetEvent &event)
{
  printf(
    "Client attempting to connect %x:%u.\n",
    event.peer->address.host,
    (unsigned int)event.peer->address.port
  );

  // Validate the client
  // TODO: send validation puzzle
  // TODO: solve validation puzzle
  // TODO: wait for client's answer
  // TODO: compare client's answer
  peers_.add_peer(event.peer, ServerPeers::Peer::Status::VALIDATING);




  /* Store any relevant client information here. */
  event.peer->data = (char*)"Client information";
}


void NetServer::disconnect_cb(ENetEvent &event)
{
  printf("%s disconnected.\n", (char*)event.peer->data);
}


void NetServer::receive_cb(ENetEvent &event)
{
  printf(
    "A packet of length %u containing %s was received from %s on channel %u.\n",
    (unsigned int)event.packet->dataLength,
    (char*)event.packet->data,
    (char*)event.peer->data,
    (unsigned int)event.channelID
  );

  // TODO: handle validation answer message
  // TODO: discard messages if not validated
  bool validated = true;

  if (!validated) {
    enet_peer_disconnect(event.peer, 0/*TODO*/);
  }

  send_packet(event.peer, Packet("I received your packet"), 0);
}


void NetServer::no_event_cb()
{

}


}  // namespace enet



// =============================================================================
// Main
//
int main()
{
  net::NetServer server;
  server.init();

  while (true) {
    server.handle_events();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}
