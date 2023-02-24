/**
 * @file
 *
 * \brief  Base server class
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#include "server.hpp"
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
      .status = new_status,
      .validation_str = ""
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


std::string ServerPeers::generate_validation_str(ENetPeer *peer)
{
  Peer* handled_peer = get_peer(peer);
  handled_peer->validation_str = "Super validation string seed";  // TODO

  return handled_peer->validation_str;
}


// =============================================================================
// NetServer
//
NetServer::NetServer(int port):
  NetBase(),
  port_(port)
{

}


bool NetServer::init()
{
  if (!NetBase::init())
    return false;

  ENetAddress address;
  address.host = ENET_HOST_ANY;
  address.port = port_;

  bool success = host_.create(
    &address,  // the address to bind the server host to
    32,        // allow up to 32 clients and/or outgoing connections
    2,         // allow up to 2 channels to be used, 0 and 1
    0,         // assume any amount of incoming bandwidth
    0          // assume any amount of outgoing bandwidth
  );

  if (!success) {
    printf("An error occurred while trying to create an ENet server host.\n");
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

  std::string validation_str = peers_.generate_validation_str(event.peer);
  Packet packet(Packet::Type::VALIDATION_STR, validation_str);
  send_packet(event.peer, packet, 0);

  // TODO: store any relevant client information here.
  event.peer->data = (char*)"Client information";
}


void NetServer::disconnect_cb(ENetEvent &event)
{
  printf("%s disconnected.\n", (char*)event.peer->data);
}


void NetServer::receive_cb(ENetEvent &event)
{
  Packet packet;
  packet.load_serialised((char*)event.packet->data, event.packet->dataLength);

  printf(
    "A packet of length %u containing %s was received from %s on channel %u.\n",
    (unsigned int)event.packet->dataLength,
    (char*)packet.get_data().c_str(),
    (char*)event.peer->data,
    (unsigned int)event.channelID
  );

  ServerPeers::Peer* peer = peers_.get_peer(event.peer);

  if (peer == nullptr) {
    printf("ERROR: received message from unknown peer\n");
    return;
  }

  // Handle validation answer from newly connected peers
  if (packet.get_type() == Packet::Type::VALIDATIION_ANSWER) {
    std::string expected_answer = solve_validation_puzzle(peer->validation_str);

    if (packet.get_data() == peer->validation_str) {
      peer->status = ServerPeers::Peer::Status::CONNECTED;
      printf("Peer validated!\n");
    } else {
      enet_peer_disconnect(event.peer, 0/*TODO*/);
      printf("Peer failed validation puzzle. Disconnecting\n");
    }

    return;
  }

  // Disconnect unauthorised peers
  if (peer->status != ServerPeers::Peer::Status::CONNECTED) {
    printf("Received message from unauthorised peer! Disconnecting\n");
    enet_peer_disconnect(event.peer, 0/*TODO*/);
  }

  // Handle messages from authorised peers
  // TODO
  send_packet(event.peer, Packet(Packet::Type::DATA, "I received your packet"), 0);
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
  net::NetServer server(1234);
  server.init();

  while (true) {
    server.handle_events();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}
