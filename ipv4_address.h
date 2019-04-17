#pragma once

#include <ostream>
#ifndef _MSC_VER
#else
#   include <WinSock2.h>
#endif
#include "types.h"

struct Ipv4Address {

#ifndef _MSC_VER
    /*
     * Internet address (old style... should be updated)
     */
    struct In_Addr {
        union {
                struct { uint8_t s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { uint16_t s_w1,s_w2; } S_un_w;
                uint32_t S_addr;
        } S_un;
    }; // In_Addr
#else
    typedef ::in_addr In_Addr;
#endif

    Ipv4Address();
    Ipv4Address(uint32_t ip, uint16_t port);

    // format: ip(string):port
    // eg: 192.168.0.1:22
    Ipv4Address(const char*);

    Ipv4Address& operator=(const Ipv4Address&);

    bool operator==(const Ipv4Address&) const;
    bool operator!=(const Ipv4Address&) const;

    bool set(const char*);

    std::ostream& operator<< (std::ostream & os) const;

    static bool IsGroupAddress(uint32_t ip);

    static std::string ToString(uint32_t ip);

    uint32_t ip;        // host order byte
    uint16_t port;
};

std::ostream& operator<<(std::ostream & os, const Ipv4Address& one);
