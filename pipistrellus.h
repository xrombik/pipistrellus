#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#pragma pack(push, 1)


#define swap16(x)   \
  ((uint16_t) ((((x) >> 8U) & 0xff) | (((x) & 0xff) << 8U)))


#define bswap32(x)  \
  ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8)    \
   | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))


typedef struct
{
  uint32_t size_used;
  uint32_t size_alloc;
  uint8_t* data;
} buffer;


/*! \defgroup mac_addrs ETH-протокол @{ */
#define ETH_TYPE_ARP  swap16  (0x0806)

typedef struct
{
  uint8_t dst[6]; /* mac-адрес назачения (кому) */
  uint8_t src[6]; /* mac-адрес источика (от кого) */
  uint16_t type;  /* тип протокола */
} mac_addrs;
/*! @} */

typedef struct  
{
  union
  {
    uint8_t bytes[2];
    uint16_t word;
  } port;
  
  union
  {
    uint8_t bytes[4];
    uint32_t dword;
  } addr;
} udp_addr;


typedef struct 
{
  buffer* rx_buffer;
  buffer* tx_buffer;
} wire;

/*! \defgroup arp_addrs ARP-протокол @{ */

#define ARP_ASQ   swap16  (     1)       /* Значение arp_frame::opcode - запрос */
#define ARP_REP   swap16  (     2)       /* Значение arp_frame::opcode - ответ */


/** Формат arp-запроса и arp-ответа */
typedef struct arp_frame
{
  mac_addrs maddrs;       /*  */
  uint16_t  hw_type;      /*  */
  uint16_t  prot_type;    /*  */
  uint8_t   hw_size;      /*  */
  uint8_t   prot_size;    /*  */
  uint16_t  opcode;       /*  */
  uint8_t   sndr_mac[6];  /*  */
  uint32_t  sndr_ip;      /*  */
  uint8_t   trgt_mac[6];  /*  */
  uint32_t  trgt_ip;      /*  */
} arp_frame;

/*! @} */


typedef struct
{
	uint8_t     verlen;
	uint8_t     tos;
	uint16_t    lenght;
	uint16_t    id;
	uint16_t    offset;
	uint8_t     ttl;      /* ip поле */
	uint8_t     protocol; /* ip поле */
	uint16_t    xsum;     /* ip поле */
	uint32_t    srcadr;   /* ip поле */
	uint32_t    dstadr;	  /* ip поле */
	uint16_t    srcport;  /* udp поле */
	uint16_t    dstport;	/* udp поле */
	uint16_t    lendg;	  /* udp поле */
	uint16_t    xsumd;	  /* udp поле */
} ip_header;


typedef struct
{
	udp_addr source_address;
	udp_addr dest_address;
	uint8_t place_holder;
	uint8_t protocol;
	uint16_t length;
} ip_pseudoheader;


typedef struct 
{

} icmp_header;

#pragma pack(pop)


bool mac_init_addr(mac_addrs* maddr, const uint8_t* src, const uint8_t* dst);

/*! Заполняет поля x_buffer
\param[out] x_buffer Буфер в котором будет размещён mac-адрес
\return true если адрес размещён, false - если не размещён (недостаточно места) */
bool mac_set_addr(buffer* x_buffer, const mac_addrs* addr);

/** Проверяет, что пакет адресован текущему узлу на mac-уровне */
bool mac_receive(const buffer* rx_buffer, const mac_addrs* addr);

/** Заполняет буфер адресом на mac-уровне */
bool mac_send(buffer* tx_buffer, const mac_addrs* addr);

/** Проверяет, что icmp-запрос предназначен для текущего узла */
bool icmp_receive(const buffer* rx_buffer, const mac_addrs* maddr, uint32_t ip_addr);

/*! Заполняет буфер ответом на icmp-запрос
\param[out] tx_buffer Буфер в котором будет размещён ответ на icmp-запрос 
\param[in] rx_buffer Буфер в котором размещён icmp-запрос
\return true - если ответ размещён, false - если иначе (недостаточно места) */
bool icmp_send(buffer* tx_buffer, const buffer* rx_buffer);

/*! Проверяет, что arp-запрос предназначен для текущего узла
\param[in] rx_buffer Буфер в котором содержится arp-запрос
\param[in] maddr mac-адреса
\param[in] ip_addr ip-адрес текущего узла */
bool arp_receive(const buffer* rx_buffer, const mac_addrs* maddr, uint32_t ip_addr);

/** */
bool arp_send(buffer* tx_buffer, const buffer* rx_buffer, const mac_addrs* mac_addr, uint32_t ip_addr);

/** */
bool udp_init_addr(udp_addr* udp_addr, const uint8_t* addr, const uint16_t port);

/** */
bool udp_get_buffer(const buffer* x_buffer, buffer* udp_buffer);

/** */
bool udp_get_src(const buffer* x_buffer, udp_addr* addr);

/** */
bool udp_get_dst(const buffer* x_buffer, udp_addr* addr);

/** */
bool udp_cmp_src(const buffer* x_buffer, const udp_addr* addr);

/** */
bool udp_cmp_dst(const buffer* x_buffer, const udp_addr* addr);

/** */
bool udp_receive(const buffer* x_buffer, const udp_addr* src, const udp_addr* dst);

/** */
bool udp_set_src(buffer* tx_buffer, udp_addr* addr);

/** */
bool udp_set_dst(buffer* tx_buffer, udp_addr* addr);

/** */
bool udp_set_xsum(buffer* tx_buffer);

/** */
bool udp_send(buffer* tx_buffer, const udp_addr* src, const udp_addr* dst);
