/**
 * @file
 *
 * \brief  Base server class
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#ifndef NET_SERVER_HPP
#define NET_SERVER_HPP

#include "net_base.hpp"
#include "enet/enet.h"
#include <vector>


namespace net
{

/// List of all peers handled by the server
class ServerPeers
{
  public:
    /// Internally handles a peer connection
    struct Peer
    {
      /// Status of the connection to the peer
      enum class Status
      {
        VALIDATING,  ///< Waiting to be approved
        CONNECTED    ///< The peer has successfully connected
      };

      ENetPeer *peer;  ///< Signature of the client
      Status status;   ///< Status of the connection
    };

    /// Adds a peer to the list of handled peers and sets its status
    void add_peer(ENetPeer *peer, Peer::Status new_status);

    /// Returns a reference to the handled peer (nullptr if not found)
    Peer* get_peer(ENetPeer *peer);

    /// Removes a peer from the list of handled peers
    void remove_peer(ENetPeer *peer);

  private:
    std::vector<Peer> peers_;  ///< Reference to all peers currently handled
};


/// Base class for all network servers
class NetServer: public NetBase
{
  public:
    /// Initialises networking and the connection, returns whether it was successful
    bool init() override;

  private:
    ServerPeers peers_;  ///< Reference to all peers currently handled

    /// Called when a connection has been established
    void connect_cb(ENetEvent &event) override;

    /// Called when a connection has been ended or has timed out
    void disconnect_cb(ENetEvent &event) override;

    /// Called when a packet has been received
    void receive_cb(ENetEvent &event) override;

    /// Called when no event has occured within the time limit
    void no_event_cb() override;
};

}  // namespace net

#endif
