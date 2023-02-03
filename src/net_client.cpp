/**
 * @file
 *
 * \brief  Base client class
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#include "net_client.hpp"
#include "enet/enet.h"
#include <memory>
#include <cstring>

#include <chrono>
#include <thread>

#include <stdio.h>


namespace net
{

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


void NetClient::connect_cb(ENetEvent &event)
{
  printf(
    "A new client connected from %x:%u.\n",
    event.peer->address.host,
    (unsigned int)event.peer->address.port
  );

  /* Store any relevant client information here. */
  event.peer->data = (char*)"Client information";
}


void NetClient::disconnect_cb(ENetEvent &event)
{
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

  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  ENetAddress address;
  ENetEvent event;
  ENetPeer *peer;

  enet_address_set_host (& address, "localhost");
  address.port = 1234;
  /* Initiate the connection, allocating the two channels 0 and 1. */
  peer = enet_host_connect(client.get_host(), & address, 2, 0);
  if (peer == NULL)
  {
    fprintf (stderr,
              "No available peers for initiating an ENet connection.\n");
    exit (EXIT_FAILURE);
  }
  /* Wait up to 5 seconds for the connection attempt to succeed. */
  if (enet_host_service (client.get_host(), & event, 5000) > 0 &&
    event.type == ENET_EVENT_TYPE_CONNECT)
  {
    puts ("Connection to localhost:1234 succeeded.");
  }
  else
  {
    /* Either the 5 seconds are up or a disconnect event was */
    /* received. Reset the peer in the event the 5 seconds   */
    /* had run out without any significant event.            */
    enet_peer_reset (peer);
    puts ("Connection to slocalhost:1234 failed.");
  }

  /* Create a reliable packet of size 7 containing "packet\0" */
  ENetPacket * packet = enet_packet_create ("packet",
                                            strlen ("packet") + 1,
                                            ENET_PACKET_FLAG_RELIABLE);
  /* Extend the packet so and append the string "foo", so it now */
  /* contains "packetfoo\0"                                      */
  enet_packet_resize (packet, strlen ("packetfoo") + 1);
  strcpy ((char*)(&packet -> data[strlen ("packet")]), "foo");
  /* Send the packet to the peer over channel id 0. */
  /* One could also broadcast the packet by         */
  /* enet_host_broadcast (host, 0, packet);         */
  enet_peer_send(peer, 0, packet);
  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  while (true) {
    client.handle_events();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}
