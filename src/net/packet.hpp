/**
 * @file
 *
 * \brief  Packet sent accross the network
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#ifndef NET__PACKET_HPP
#define NET__PACKET_HPP

#include <string>


namespace net
{

/// Packet sent accross the network
class Packet
{
  public:
    Packet(const std::string &data);

    /// Returns the raw data contained in the packet
    const char* get() const;

    /// Appends some data to the packet
    // TODO
    void append(const std::string &new_data) = delete;

  private:
    std::string data_;  ///< Raw data contained in the packet
};

}  // namespace net

#endif
