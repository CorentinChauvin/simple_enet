/**
 * @file
 *
 * \brief  Base client class
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#ifndef NET__CLIENT_HPP
#define NET__CLIENT_HPP

#include "base.hpp"
#include "enet/enet.h"
#include <string>
#include <chrono>


namespace net
{

/**
 * \brief  Base class for all network clients
 *
 * Can handle a connection to a single remote peer
 */
class NetClient: public NetBase
{
  public:
    /**
     * \param validation_salt  Used to scramble the validation string, should be common to all peers
     */
    NetClient(const std::string &validation_salt);


    /// Initialises networking and the connection, returns whether it was successful
    bool init() override;

    /**
     * \brief  Initiates connection to a given host
     *
     * \param host  Hostname or IP address of the host to connect to
     * \param port  Destination port
     * \param timeout  Duration before timing out the connection attempt (in s)
     * \return  Whether the connection could be initiated, or failed
     */
    bool connect(const std::string &host, int port, float timeout);

    /// Handles events
    void handle_events() override;

    /**
     * \brief  Sends a packet to the connected peer
     *
     * \param packet      Message to send
     * \param channel_id  ENet channel on which to send
     */
    void send_packet(const Packet &packet, int channel_id);

  private:
    /// Connection status
    enum class Status
    {
      DISCONNECTED,  ///< Not connected to any peer
      CONNECTING,    ///< Attempting to connect to a peer
      CONNECTED      ///< Connected to a peer
    };

    Status status_;  ///< Connection status
    float timeout_;  ///< Duration before timing out the connection attempt (in s)
    std::chrono::steady_clock::time_point connection_start_time_;  ///< When the connection was initiated
    ENetPeer *peer_;   ///< Connected peer

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
