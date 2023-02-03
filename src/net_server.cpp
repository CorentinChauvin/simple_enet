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


namespace net
{

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
    "A new client connected from %x:%u.\n",
    event.peer->address.host,
    (unsigned int)event.peer->address.port
  );

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
}


void NetServer::no_event_cb()
{

}



}  // namespace enet


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
