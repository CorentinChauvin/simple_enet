/**
 * @file
 *
 * \brief  Packet sent accross the network
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#include "packet.hpp"
#include "cereal/archives/portable_binary.hpp"
#include <cereal/types/string.hpp>
#include <sstream>
#include <string>


namespace net
{

Packet::Packet(Packet::Type type, const std::string &data):
  type_(type),
  data_(data)
{

}


Packet::Packet(const std::string &raw_data)
{
  std::istringstream is(raw_data);
  uint8_t _type;

  {
    cereal::PortableBinaryInputArchive iarchive(is);
    iarchive(_type, data_);
  }

  type_ = static_cast<Type>(_type);
}


Packet::Packet(const char *raw_data, int length):
  Packet(std::string(raw_data, length))
{

}


std::string Packet::serialise() const
{
  std::ostringstream os;

  {
    cereal::PortableBinaryOutputArchive oarchive(os); // Create an output archive
    oarchive(static_cast<uint8_t>(type_), data_);
  }

  return os.str();
}


std::string Packet::get_data() const
{
  return data_;
}


Packet::Type Packet::get_type() const
{
  return type_;
}


}  // namespace net
