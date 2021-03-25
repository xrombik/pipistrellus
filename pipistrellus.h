#pragma once

#include <stdint.h>

struct buffer
{
  uint32_t size;
  uint8_t* data;
};

struct mac_addr
{
  uint8_t dst[6];
  uint8_t src[6];
  uint16 type;
};

struct udp_addr
{
  uint16 port;
  uint32 addr;
};

struct wire
{
  buffer* rx_buffer;
  buffer* tx_buffer;
};

bool mac_set_addr(buffer* x_buffer, const mac_addr* addr);
bool mac_init_addr(mac_addr* addr, const uint8_t* src, const uint8_t* dst);
bool mac_receive(const buffer* x_buffer, const mac_addr* addr);
bool mac_send(buffer* x_buffer, const mac_addr* addr);

bool icmp_receive(const buffer* rx_buffer, const mac_addr* mac_addr, uint32 ip_addr);
bool icmp_send(buffer* tx_buffer, const buffer* rx_buffer);

bool udp_get_buffer(const buffer* x_buffer, buffer* udp_buffer);
bool udp_get_src(const buffer* x_buffer, udp_addr* addr);
bool udp_get_dst(const buffer* x_buffer, udp_addr* addr);
bool udp_cmp_src(const buffer* x_buffer, const udp_addr* addr);
bool udp_cmp_dst(const buffer* x_buffer, const udp_addr* addr);
bool udp_receive(const buffer* x_buffer, const udp_addr* src, const udp_addr* dst);
bool udp_set_src(buffer* tx_buffer, udp_addr* addr);
bool udp_set_dst(buffer* tx_buffer, udp_addr* addr);
bool udp_set_xsum(buffer* tx_buffer);
bool udp_send(buffer* tx_buffer, const udp_addr* src, const udp_addr* dst);
