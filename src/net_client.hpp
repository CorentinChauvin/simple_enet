/**
 * @file
 *
 * \brief  Base client class
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#ifndef NET_CLIENT_HPP
#define NET_CLIENT_HPP

#include "net_base.hpp"
#include "enet/enet.h"


namespace net
{

/// Base class for all network servers
class NetClient: public NetBase
{
  public:
    /// Initialises networking and the connection, returns whether it was successful
    bool init() override;

  private:
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
