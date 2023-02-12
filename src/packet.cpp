/**
 * @file
 *
 * \brief  Packet sent accross the network
 * \author Corentin Chauvin-Hameau
 * \date   2023
 */

#include "packet.hpp"
#include <string>


namespace net
{

Packet::Packet(const std::string &data):
  data_(data)
{

}


const char* Packet::get() const
{
  return data_.c_str();
}


}  // namespace net
