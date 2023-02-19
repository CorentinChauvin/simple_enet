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
    /// Type of the packet
    enum class Type: uint8_t
    {
      DATA,               ///< Generic data packet
      VALIDATION_STR,     ///< String sent by the server to a newly connected peer for validation
      VALIDATIION_ANSWER  ///< Validation answer of a newly connected peer to the server for validation
    };

    /**
     * \param type  Description of the packet
     * \param data  Data contained in the packet
     */
    Packet(Type type, const std::string &data);

    /**
     * \param raw_data  Serialised data of the packet
     */
    Packet(const std::string &raw_data);

    /**
     * \param raw_data  Serialised data of the packet
     * \param length    Length of the data
     */
    Packet(const char *raw_data, int length);

    /// Get the raw serialised data of the packet
    std::string serialise() const;

    /// Returns the data contained in the packet
    std::string get_data() const;

    /// Returns the packet type
    Type get_type() const;

    /// Appends some data to the packet
    // TODO
    void append(const std::string &new_data) = delete;

  private:
    Type type_;         ///< Description of the packet
    std::string data_;  ///< Data contained in the packet
};

}  // namespace net

#endif
