/**
 * @file
 *
 * \brief  Base client class
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#include "net_client.hpp"
#include "net_base.hpp"
#include "packet.hpp"
#include "enet/enet.h"
#include <string>

#include <chrono>
#include <thread>

#include <cstring>
#include <stdio.h>


namespace net
{

NetClient::NetClient():
  status_(NetClient::Status::DISCONNECTED),
  peer_(nullptr)
{

}


bool NetClient::init()
{
  if (!NetBase::init())
    return false;

  bool success = host_.create(
    nullptr,   // create a client host
    1,         // only allow 1 outgoing connection
    2,         // allow up 2 channels to be used, 0 and 1
    0,         // assume any amount of incoming bandwidth
    0          // assume any amount of outgoing bandwidth
  );

  if (!success) {
    fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
    return false;
  }

  return true;
}


bool NetClient::connect(const std::string &host, int port, float timeout)
{
  ENetAddress address;
  enet_address_set_host(&address, host.c_str());
  address.port = port;

  // Initiate the connection, allocating the two channels 0 and 1.
  peer_ = enet_host_connect(get_host(), &address, 2, 0);

  if (peer_ == nullptr)
    return false;

  timeout_ = timeout;
  connection_start_time_ = std::chrono::steady_clock::now();
  status_ = Status::CONNECTING;

  return true;
}


void NetClient::handle_events()
{
  if (status_ == Status::CONNECTING) {
    auto t = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = t - connection_start_time_;

    if (duration.count() > timeout_) {
      enet_peer_reset(peer_);
      status_ = Status::DISCONNECTED;

      printf("Connection to localhost:1234 failed.\n");
    }
  }

  NetBase::handle_events();
}


void NetClient::send_packet(const Packet &packet, int channel_id)
{
  if (status_ == Status::CONNECTED)
    NetBase::send_packet(peer_, packet, channel_id);
}


void NetClient::connect_cb(ENetEvent &event)
{
  status_ = Status::CONNECTED;

  printf(
    "A new client connected from %x:%u.\n",
    event.peer->address.host,
    (unsigned int)event.peer->address.port
  );

  /* Store any relevant client information here. */
  event.peer->data = (char*)"Client information";

  // TODO: remove
  send_packet(net::Packet("Hellou :)"), 0);
}


void NetClient::disconnect_cb(ENetEvent &event)
{
  status_ = Status::DISCONNECTED;

  printf("%s disconnected.\n", (char*)event.peer->data);
}


void NetClient::receive_cb(ENetEvent &event)
{
  printf(
    "A packet of length %u containing %s was received from %s on channel %u.\n",
    (unsigned int)event.packet->dataLength,
    (char*)event.packet->data,
    (char*)event.peer->data,
    (unsigned int)event.channelID
  );
}


void NetClient::no_event_cb()
{

}



}  // namespace enet


int main()
{
  net::NetClient client;
  client.init();
  client.connect("localhost", 1234, 3.0);

  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  while (true) {
    client.handle_events();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}
