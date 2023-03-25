/**
 * @file
 *
 * \brief  Base server class
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#include "server.hpp"
#include "enet/enet.h"
#include <random>
#include <string>
#include <memory>
#include <chrono>
#include <thread>

#include <stdio.h>
#include <cstring>


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


std::string ServerPeers::generate_validation_str(ENetPeer *peer, int length)
{
  static auto& chrs = "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  thread_local static std::mt19937 rg{std::random_device{}()};
  thread_local static std::uniform_int_distribution<int> pick(0, sizeof(chrs) - 2);

  std::string validation_str;
  validation_str.reserve(length);

  while (length--)
    validation_str += chrs[pick(rg)];

  Peer* handled_peer = get_peer(peer);
  handled_peer->validation_str = validation_str;

  return validation_str;
}


// =============================================================================
// NetServer
//
NetServer::NetServer(int port, int validation_str_size, const std::string &validation_salt):
  NetBase(validation_salt),
  port_(port),
  validation_str_size_(validation_str_size)
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

  std::string validation_str = peers_.generate_validation_str(
    event.peer, validation_str_size_
  );
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

    if (packet.get_data() == expected_answer) {
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
  const int port = 1234;
  const int validation_str_size = 128;
  const std::string validation_salt = "Blektr!";

  net::NetServer server(port, validation_str_size, validation_salt);
  server.init();

  while (true) {
    server.handle_events();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}
