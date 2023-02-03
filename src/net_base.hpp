/**
 * @file
 *
 * \brief  Base networking class
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#ifndef NET_BASE_HPP
#define NET_BASE_HPP

#include "enet/enet.h"
#include <memory>


namespace net
{

/// Convenience class handling a host,
class NetHost
{
  public:
   NetHost();
   ~NetHost();

    /**
     * \brief  Creates a host for communicating to peers.
     *
     * \param address            The address at which other peers may connect to this host.  If NULL, then no peers may connect to the host.
     * \param peerCount          The maximum number of peers that should be allocated for the host.
     * \param channelLimit       The maximum number of channels allowed; if 0, then this is equivalent to ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT
     * \param incomingBandwidth  Downstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.
     * \param outgoingBandwidth  Upstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.
     * \return  Whether the host could be created
     *
     * \remarks ENet will strategically drop packets on specific sides of a connection between hosts
     * to ensure the host's bandwidth is not overwhelmed.  The bandwidth parameters also determine
     * the window size of a connection which limits the amount of reliable packets that may be in transit
     * at any given time.
     */
    bool create(
      const ENetAddress *address,
      size_t peerCount,
      size_t channelLimit,
      enet_uint32 incomingBandwidth,
      enet_uint32 outgoingBandwidth
    );

    /// Destroys the host and all resources associated with it
    void destroy();

    /// Returns a reference to the ENet host
    ENetHost* get();

  private:
    bool is_host_created_;  ///< Whether the host has already been created
    ENetHost* host_;        ///< Reference to the ENet host object
};

/// Base networking class using ENet library
class NetBase
{
  public:
    NetBase();

    /// Initialises networking and the connection, returns whether it was successful
    virtual bool init();

    /// Handles events
    void handle_events();

    /// Returns a pointer to the ENet host
    ENetHost* get_host();

  protected:
    NetHost host_;  ///< Host managing connections

    /// Called when a connection has been established
    virtual void connect_cb(ENetEvent &event) = 0;

    /// Called when a connection has been ended or has timed out
    virtual void disconnect_cb(ENetEvent &event) = 0;

    /// Called when a packet has been received
    virtual void receive_cb(ENetEvent &event) = 0;

    /// Called when no event has occured within the time limit
    virtual void no_event_cb() = 0;
};


}  // namespace net

#endif
