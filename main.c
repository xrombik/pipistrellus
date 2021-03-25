#include <stdint.h>

#include "pipistrellus.h"
#include "main.h"

#define BUFFER_SIZE 1500

const uint8_t* MAC_SRC = {0x01, 0x02, 0x03, 0x04, 0x05};
const uint8_t* MAC_DST = {0xff, 0xff, 0xff, 0xff, 0xff};


int main(int argc, char** argv)
{
  /* Настройка оборудования.
     Нетопырю всё равно что здесь происходит
     ... */
  

  struct mac_addrs maddr;
  mac_init(&maddr, MAC_SRC, MAC_DST);
  
  /*! \defgroup init Размещение памяти
      \brief Здесь пользователь сообщает нетопырю
      адреса и размеры буферов для приёма и передачи @{ */
  struct buffer rx_buffer;
  uint8_t rx_data[BUFFER_SIZE];
  rx_buffer.data = rx_data;
  rx_buffer.size_alloc = sizeof rx_data;

  struct buffer tx_buffer;
  uint8_t tx_data[BUFFER_SIZE];
  tx_buffer.data = tx_data;
  tx_buffer.size_alloc = sizeof tx_data;
  /*! @} */

  struct mac_addrs maddr;
  mac_init_addr(&maddr, MAC_SRC, MAC_DST);

  /* UDP-адрес этого узла в сети */
  const struct udp_addr self_addr = { 0U, 0U }; 
  
  /* UDP-адрес внешнего узла в сети */
  struct udp_addr trgt_addr = { 1U, 0U };

  /* Здесь оборудование должно быть готово для приёма и передачи данных */
  while(1)
  {
    /* Получить пакет "с провода" */
    rx_buffer.size_used = hw_receive(rx_buffer.data, rx_buffer.size_alloc);
    if (!rx_buffer.size_used)
      continue;
    
    /* Здесь нетопырь ожидает, что rx_buffer.data будет содержать mac-адреса,
    тип, IP-заголовок и данные, а rx_buffer.size длину eth-фрейма */
    
    /* Здесь нетопырь обработает запросы ICMP */
    if (icmp_receive(&rx_buffer, &maddr, self_addr.addr))
    {
      icmp_send(&tx_buffer, &rx_buffer);
      hw_transmit(tx_buffer.data, tx_buffer.size_used);
      continue;
    }

    /* Здесь нетопырь обработает запросы ARP */
    if (arp_receive(&rx_buffer, &maddr))
    {
      arp_send(&tx_buffer, &maddr);
      hw_transmit(tx_buffer.data, tx_buffer.size_used);
      continue;
    }
    
    /* Здесь нетопырь обработает запросы UDP */
    if (udp_receive(&rx_buffer, &trgt_addr, &self_addr))
    {
      struct buffer udp_rx_buffer;
      udp_rx_buffer.size_used = 0U;
      udp_get_data(&rx_buffer, &udp_rx_buffer);

      /* Здесь пользователь может обработать udp-датаграмму и отправить ответ
      внешнему узлу например так: */
      if (udp_rx_buffer.size_used > 20U)
      {
        printf("data[8]:%u\n", udp_rx_buffer.data[8]);
        /* Здесь пользователь может отправить данные в сеть, например так: */
        struct buffer udp_tx_buffer;
        udp_get_data(&tx_buffer, &udp_tx_buffer);
        udp_tx_buffer.size_used = strncpy(udp_tx_buffer.data, "pipistrellus", udp_tx_buffer.size_alloc) + sizeof '\0';
        udp_send(&tx_buffer, &self_addr, &trgt_addr);
        mac_send(&tx_buffer, &maddr);
        hw_transmit(tx_buffer.data, tx_buffer.size_used);
      }
    }
  }
}


uint32_t hw_receive(uint8_t* data, uint32_t size)
{
  /* Здесь выполняют работу с оборудованием, для получения данных "с провода".
     Данные из оборудования заносят в buffer, например так: */
  uint32_t i;
  for (i = 0; i < size; i ++)
  {
    // if (...) break;
    // data[i] = ...;
  }
  return i;
}


uint32_t hw_transmit(uint8_t* data, uint32_t size)
{
  /* Здесь выполняют работу с оборудованием, для выставления данных "на провод"
  Данные из data передают в оборудование, например так:
  uint32_t i;
  for (i = 0; i < size; i ++)
  {
    if(...) break;
    ... = data[i];
  }
  */
  return size;
}
