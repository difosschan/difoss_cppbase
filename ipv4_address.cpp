#include "ipv4_address.h"

#include "str_util.h"
#include "ctime.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;
//////////////////////////////////////////////////////////////////////////
Ipv4Address::Ipv4Address()
: ip(0), port(0) {}

Ipv4Address::Ipv4Address(uint32_t ip_, uint16_t port_)
: ip(ip_), port(port_) {}


Ipv4Address::Ipv4Address( const char* cstr )
: ip(0), port(0) {
    set(cstr);
}

bool Ipv4Address::set( const char* cstr ) {
    string ip_port(cstr);
    vector<string> v;
    StrUtil::Split(cstr, ":", v);
    switch (v.size())
    {
    case 1:
        StrUtil::ReplaceAll(v[0], "*", "255");
        ip = ::inet_network(v[0].c_str());
        port = 0;
        break;
    case 2:
        StrUtil::ReplaceAll(v[0], "*", "255");
        ip = ::inet_network(v[0].c_str());
        port = StrUtil::ToInt(v[1].c_str());
        break;
    default:
        break;
    }
    return true;
}
Ipv4Address& Ipv4Address::operator= ( const Ipv4Address& other ) {
    if (&other != this) {
        this->ip = other.ip;
        this->port = other.port;
    }
    return *this;
}

ostream& Ipv4Address::operator<<( ostream& os ) const
{
    in_addr ip_net;
    ip_net.s_addr = htonl(ip);
    os << "ip: " << ip << "/" << inet_ntoa(ip_net) << ", port: " << port;
    return os;
}

bool Ipv4Address::operator==( const Ipv4Address& other ) const
{
    if (&other == this)
        return true;
    return (this->ip == other.ip) && (this->port == other.port);
}

bool Ipv4Address::operator!=( const Ipv4Address& other ) const {
    return !(*this == other);
}

bool Ipv4Address::IsGroupAddress( uint32_t ip )
{
    uint32_t net_ip = htonl(ip);
    return (net_ip &= 0xE0) == 0xE0;
}

std::string Ipv4Address::ToString( uint32_t ip )
{
    in_addr ip_net;
    ip_net.s_addr = htonl(ip);
    return std::string(inet_ntoa(ip_net));
}



ostream& operator<<(ostream & os, const Ipv4Address& one) {
    one.operator<<(os);
    return os;
}
