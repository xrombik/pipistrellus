#ifndef __PIPISTRELLUS_H__
#define __PIPISTRELLUS_H__

/** \file  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/** \file  */

#define swap16(x) ((uint16_t) ((((x) >> 8U) & 0xff) | (((x) & 0xff) << 8U)))  
#define ETH_TYPE_ARP    swap16  (0x0806)   /**< Значение поля mac_addrs::type - arp-протокол */
#define ETH_TYPE_IPV4   swap16  (0x0800)   /**< Значение поля mac_addrs::type - ipv4-протокол */
#define ARP_OPCODE_ASQ  swap16  (    1U)   /**< Значение поля arp_frame::opcode - запрос */
#define ARP_OPCODE_REP  swap16  (    2U)   /**< Значение поля arp_frame::opcode - ответ */
#define ICMP_VERLEN                0x45    /**< Версия и длина */
#define ICMP_PROTO                   1U    /**< Значение поля icmp_frame::proto */
#define ICMP_OPCODE_ASQ              8U    /**< Значение поля icmp_frame::opcode - запрос */
#define ICMP_OPCODE_REP              0U    /**< Значение поля icmp_frame::opcode - ответ */
#define UDP_PROTO                   17U    /**< Значение поля udp_frame::proto */
#define UDP_TTL                     64U    /**< Время жизни пакета */
#define UDP_TOS                      0U    /**< TODO: Заглушка */ 
#define UDP_FLAGS       swap16  (0x4000)   /**< Значение поля udp_frame::flags */         
#define MTU_SIZE                  1500U    /**< Максимальный размер фрейма без фрагментации */

#pragma pack(push, 1)


/** Буфер для выделения области в памяти */
typedef struct 
{
    size_t   size_alloc;     /**< Размер области в байтах выделенный */
    size_t   size_used;      /**< Размер области в байтах занятый */
    uint8_t  data[MTU_SIZE]; /**< Занимаемая область */
} buffer;


/** Буфер размещаемый внутри области */
typedef struct 
{
    size_t   size_used; /**< Размер занимаемой обласи в байтах */
    uint8_t* data;      /**< Указатель на начало занимаемой области */    
} udp_buffer;


/** mac-адреса */
typedef struct
{
    uint8_t  trgt[6U]; /**< mac-адрес получателя */
    uint8_t  sndr[6U]; /**< mac-адрес отправителя */
    uint16_t type;     /**< тип протокола */
} mac_addrs;


/** udp-адрес */
typedef struct
{
    uint16_t port;  /**< Порт */
    uint32_t addr;  /**< Адрес IPv4 */
} udp_addr;


/** Формат icmp-запроса и icmp-ответа для выполнения пинга */
typedef struct
{
    mac_addrs maddrs;    /**< mac-адреса получателя и отправителя */
    uint8_t   verlen;    /**< версия и длина */
    uint8_t   dsc_ecn;   /**< Differentiated Services Codepoint: Default (0)
                                Explicit Congestion Notification: Not ECN-Capable Transport (0) */
    uint16_t  totlen;    /**< Total Length. Длина icmp-данных  + sizeof(icmp_frame) */
    uint16_t  id;        /**< Identification, ответчающий возвращает это значение увеличенным на 1 */
    uint16_t  flags;     /**< Флаги, в ответе все установлены в 0 */
    uint8_t   ttl;       /**< Время жизни пакета */
    uint8_t   proto;     /**< Протокол */
    uint16_t  hdr_csum;  /**< Header checksum */
    uint32_t  sndr_ip;   /**< ip-адрес отправителя */
    uint32_t  trgt_ip;   /**< ip-адрес получателя */
    uint8_t   opcode;    /**< Тип icmp-пакета: запрос или ответ */
    uint8_t   code;      /**< Code, всегда 0 */
    uint16_t  csum;      /**< Контрольная сумма */
    uint16_t  be;        /**< Identifier BE. В ответе повторяет значение из запроса */
    uint16_t  le;        /**< Identifier LE. В ответе повторяет значение из запроса */
    uint8_t   time[8U];  /**< Отметка времени. В ответе повторяет значение из запроса */
                         /**< Здесь начинаются icmp-данные. В ответе повторяет значение из запроса */
} icmp_frame;


/** Формат arp-запроса и arp-ответа */
typedef struct
{
    mac_addrs maddrs;       /**< mac-адреса получателя и отправителя */
    uint16_t  hw_type;      /**< */
    uint16_t  prot_type;    /**< */
    uint8_t   hw_size;      /**< */
    uint8_t   prot_size;    /**< */
    uint16_t  opcode;       /**< Код операции, одно из ARP_OPCODE_ASQ, ARP_OPCODE_REP */
    uint8_t   sndr_mac[6U]; /**< mac-адрес отправителя */
    uint32_t  sndr_ip;      /**< ip-адрес отправителя */
    uint8_t   trgt_mac[6U]; /**< mac-адрес получателя */
    uint32_t  trgt_ip;      /**< ip-адрес получателя */
} arp_frame;


/** Формат udp-заголовка */
typedef struct
{
    mac_addrs maddrs;    /**< mac-поле, mac-адреса получателя и отправителя */
    uint8_t   verlen;    /**< ip-поле,  */
    uint8_t   tos;       /**< ip-поле,  */
    uint16_t  length;    /**< ip-поле,  TODO: понять чего длина? */
    uint16_t  id;        /**< ip-поле,  */
    uint16_t  flags;     /**< ip-поле,  */
    uint8_t   ttl;       /**< ip-поле,  время жизни */
    uint8_t   proto;     /**< ip-поле,  код протокола */
    uint16_t  xsum;      /**< ip-поле,  контрольная сумма */
    uint32_t  sndr_addr; /**< ip-поле,  ip-адрес отправителя */
    uint32_t  trgt_addr; /**< ip-поле,  ip-адрес получателя */
    uint16_t  sndr_port; /**< udp-поле, ip-порт отправителя */
    uint16_t  trgt_port; /**< udp-поле, ip-порт получателя */
    uint16_t  lendg;	 /**< udp-поле, длинна udp-датаграммы */
    uint16_t  xsumd;	 /**< udp-поле, котрольная сумма udp-датаграммы */
                         /**< udp-поле, здесь начинается пользовательская датаграмма */
} udp_frame;

#pragma pack(pop)


typedef struct
{
    buffer* rx;
    buffer* tx;
    const uint8_t* mac;
    const uint8_t* ip;
} whire;

typedef struct 
{
    uint8_t* mac;
    udp_addr* udp;
} net_node;

/** Размещает буфер внутри udp-фрейма предназначенного для передачи
 \param[in]  rx_buffer Буфер который будет передан как udp-фрейм
 \param[out] udpb Размещаемый udp-буфер
 \return true - если буфер размещён, false - если иначе (недостаточно места) */
bool udp_set_data(buffer* tx_buffer, udp_buffer* udpb);


/** */
bool process_whire(whire* pwhire);


/** Заполняет структуру 
 \param[out] buf Заполняемая структура */
void buffer_init(buffer* buf);


/** */
void udp_addr_clear(udp_addr* udpa);


/** */
void udp_addr_init(udp_addr* udp_addr, const uint8_t* addr, const uint16_t port);


/** Compute Internet Checksum for "count" bytes beginning at location "addr" */
uint16_t get_checksum(const void *data, size_t len, uint32_t sum);


/** Заполняет структуру 
 \param[out] maddr Заполняемая структура
 \param[in]  sndr_mac mac-адрес отправителя
 \param[in]  trgt_mac mac-адрес получателя */
void mac_init_addr(mac_addrs* maddrs, const uint8_t* sndr_mac, const uint8_t* trgt_mac);


/** Заполняет поля mac-адресов в буфере
 \param[out] x_buffer Буфер для размещения mac-адресов
 \param[in]  maddrs mac-адреса
 \return true - если адрес размещён, false - если не размещён (недостаточно места) */
bool mac_set_addr(buffer* x_buffer, const mac_addrs* maddrs);


/** Проверяет, что пакет адресован текущему узлу на mac-уровне
 \param[in] buffer Буфер содержащий данные приятые "с провода"
 \param[in] maddrs mac-адрес текущего узла 
 \return true - если пакет адресован текущему узлу и false - если иначе */
bool mac_receive(const buffer* rx_buffer, const uint8_t* sndr_mac);


/** Проверяет, что icmp-запрос адресован текущему узлу
 \param[in] buffer Буфер содержащий данные приятые "с провода"
 \param[in] ip_addr ip-адрес текущего узла 
 \return true если icmp-запрос адресован текущему узлу и false если иначе */
bool icmp_receive(const buffer* rx_buffer, const uint8_t* ip_addr);


/** Заполняет буфер ответом на icmp-запрос
 \param[out] tx_buffer Буфер в котором будет размещён ответ на icmp-запрос 
 \param[in]  rx_buffer Буфер в котором размещён icmp-запрос
 \return true - если ответ размещён, false - если иначе (недостаточно места) */
bool icmp_send(buffer* tx_buffer, const buffer* rx_buffer);


/** Проверяет, что arp-запрос адресован текущему узлу
 \param[in] rx_buffer Буфер в котором содержится arp-запрос
 \param[in] ip_addr ip-адрес текущего узла 
 \return true - если arp-запрос адресован текущему узлу и false - если иначе */
bool arp_receive(const buffer* rx_buffer, const uint8_t* ip_addr);


/** */
bool arp_send(buffer* tx_buffer, const buffer* rx_buffer, const uint8_t* sndr_mac, const uint8_t* sndr_ip);


/** Размещает буфер внутри udp-фрейма для приёма 
 \param[in]  rx_buffer Буфер в котором содержится принятый udp-фрейм
 \param[out] udpb Размещаемый udp-буфер
 \return true - если буфер размещён, false - если иначе */
bool udp_get_data(buffer* rx_buffer, udp_buffer* udpb);


/** */
bool udp_send(buffer* tx_buffer, const net_node* self, const net_node* trgt, size_t len);

/** Проверяет, что udp-фрейм адресован текущему узлу
 \param[in]     x_buffer Буфер в котором содержится пакет с udp-датаграммой
 \param[in]     trgt Адрес текущего узла
 \param[in,out] sndr Полный адрес узла отправившего udp-фрейм. Используется
 для проверки отправителя сообщения. Вызывающий заполняет используемые поля
 udp_node::udp_addr значениями отличными от 0. Если udp-фрейм адресован
 текущему узлу, то после вызова содержит mac-адрес и udp-адрес отправителя
 \param[in]     netmask Маска подсети текущего узла
 \return true - если udp-фрейм адресован текущему узлу и false - если иначе */
bool udp_receive(const buffer* rx_buffer, const udp_addr* trgt, net_node* sndr, const uint8_t* mask);

#endif // __PIPISTRELLUS_H__
