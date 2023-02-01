#include "enet/enet.h"
#include <iostream>
#include <stdio.h>
#include <iostream>
#include <cstring>

#include <chrono>
#include <thread>

using std::cout;
using std::endl;


void handle_events(ENetHost *host)
{
  ENetEvent event;

  while (enet_host_service(host, &event, 0) > 0)
  {
    switch (event.type)
    {
      case ENET_EVENT_TYPE_CONNECT:
        printf ("A new client connected from %x:%u.\n",
                event.peer->address.host,
                (unsigned int)event.peer->address.port);
        /* Store any relevant client information here. */
        event.peer -> data = (char*)"Client information";
        break;

      case ENET_EVENT_TYPE_RECEIVE:
        printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                (unsigned int)event.packet->dataLength,
                (char*)event.packet->data,
                (char*)event.peer->data,
                (unsigned int)event.channelID);
        /* Clean up the packet now that we're done using it. */
        enet_packet_destroy (event.packet);
        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        printf ("%s disconnected.\n", (char*)event.peer->data);
        /* Reset the peer's client information. */
        event.peer -> data = NULL;
        break;

      case ENET_EVENT_TYPE_NONE:
        cout << "no event" << endl;
        break;

      default:
        break;
    }
  }
}


int main()
{
  cout << "Hellou :)" << endl;

  // Initialise Enet
  if (enet_initialize () != 0)
  {
    fprintf (stderr, "An error occurred while initializing ENet.\n");
    return EXIT_FAILURE;
  }
  atexit(enet_deinitialize);

  // Create client
  ENetHost *client;
  client = enet_host_create(
    nullptr,  // create a client host
    1,        // only allow 1 outgoing connection
    2,        // allow up 2 channels to be used, 0 and 1
    0,        // assume any amount of incoming bandwidth
    0        // assume any amount of outgoing bandwidth
  );

  if (client == nullptr)
  {
    fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
    exit(EXIT_FAILURE);
  }

  ENetAddress address;
  ENetEvent event;
  ENetPeer *peer;
  /* Connect to some.server.net:1234. */
  enet_address_set_host (& address, "localhost");
  address.port = 1234;
  /* Initiate the connection, allocating the two channels 0 and 1. */
  peer = enet_host_connect (client, & address, 2, 0);
  if (peer == NULL)
  {
    fprintf (stderr,
              "No available peers for initiating an ENet connection.\n");
    exit (EXIT_FAILURE);
  }
  /* Wait up to 5 seconds for the connection attempt to succeed. */
  if (enet_host_service (client, & event, 5000) > 0 &&
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

  // Listen for events
  while (true)
  {
    handle_events(client);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  // Cleanup
  enet_host_destroy(client);

  return 0;
}