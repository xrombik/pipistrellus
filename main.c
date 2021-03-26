#include <stdint.h>

#include "pipistrellus.h"
#include "main.h"

#define BUFFER_SIZE 1500

extern const uint8_t arp_asq_0[60];
extern const uint8_t arp_rep_0[60];
extern uint8_t MAC_BROADCAST[6];

/* mac-адрес этого узла */
const uint8_t MAC_SRC[] = {0x70, 0x4d, 0x7b, 0x65, 0x2a, 0xd0};

/* ip-aдрес этого узла */
const uint8_t IP_ADDR[] = {172, 16, 2, 10};
const uint16_t IP_PORT = 50090;

/* */
const uint8_t NULL_ADDR[] = {0x00, 0x00, 0x00, 0x00};


#define TEST_CASES_COUNT  1
const void* test_cases_data[TEST_CASES_COUNT];
uint32_t test_cases_len[TEST_CASES_COUNT];
uint32_t test_cases_i = 0;

int main(int argc, char** argv)
{
  /* Настройка оборудования.
     Нетопырю всё равно что здесь происходит
     ... */

  /*! \defgroup init Размещение памяти
      \brief Здесь пользователь сообщает нетопырю
      адреса и размеры буферов для приёма и передачи @{ */
  buffer rx_buffer;
  uint8_t rx_data[BUFFER_SIZE] = {0};
  rx_buffer.data = rx_data;
  rx_buffer.size_alloc = sizeof rx_data;

  buffer tx_buffer;
  uint8_t tx_data[BUFFER_SIZE] = {0};
  tx_buffer.data = tx_data;
  tx_buffer.size_alloc = sizeof tx_data;
  /*! @} */

  mac_addrs maddr;
  mac_init_addr(&maddr, MAC_SRC, MAC_BROADCAST);

  udp_addr self_addr;
  udp_init_addr(&self_addr, IP_ADDR, IP_PORT);
  
  udp_addr trgt_addr;
  udp_init_addr(&trgt_addr, NULL_ADDR, 0);

  /* Заполнение тестовых случаев */
  test_cases_data[0] = arp_asq_0;
  test_cases_len[0] = sizeof arp_asq_0;

  /* Здесь оборудование должно быть готово для приёма и передачи данных */
  for (uint32_t i = 0U; i < sizeof test_cases_data / sizeof test_cases_data[0]; i ++)
  {
    /* Получить пакет "с провода" */
    rx_buffer.size_used = hw_receive(rx_buffer.data, rx_buffer.size_alloc);
    if (!rx_buffer.size_used)
      continue;
    
    /* Здесь нетопырь ожидает, что rx_buffer.data будет содержать mac-адреса,
    тип, IP-заголовок и данные, а rx_buffer.size длину eth-фрейма */
    
    /* Здесь нетопырь обработает запросы ICMP */
    if (icmp_receive(&rx_buffer, &maddr, self_addr.addr.dword))
    {
      icmp_send(&tx_buffer, &rx_buffer);
    }

    /* Здесь нетопырь обработает запросы ARP */
    else if (arp_receive(&rx_buffer, &maddr, self_addr.addr.dword))
    {
      arp_send(&tx_buffer, &rx_buffer, &maddr, self_addr.addr.dword);
    }
    
    /* Здесь нетопырь обработает запросы UDP */
    else if (udp_receive(&rx_buffer, &trgt_addr, &self_addr))
    {
      buffer udp_rx_buffer;
      udp_rx_buffer.size_used = 0U;
      udp_get_buffer(&rx_buffer, &udp_rx_buffer);

      /* Здесь пользователь может обработать udp-датаграмму и отправить ответ
      внешнему узлу например так: */
      if (udp_rx_buffer.size_used > 20U)
      {
        printf("data[8]:%u\n", udp_rx_buffer.data[8]);
        /* Здесь пользователь может отправить данные в сеть, например так: */
        buffer udp_tx_buffer;
        udp_get_buffer(&tx_buffer, &udp_tx_buffer);
        strncpy(udp_tx_buffer.data, "pipistrellus", udp_tx_buffer.size_alloc);
        udp_tx_buffer.size_used = strlen(udp_tx_buffer.data) + sizeof '\0';
        udp_send(&tx_buffer, &self_addr, &trgt_addr);
        mac_send(&tx_buffer, &maddr);
      }
    }
    hw_transmit(tx_buffer.data, tx_buffer.size_used);
    test_cases_i ++;
  }
  return EXIT_SUCCESS;
}


uint32_t hw_receive(uint8_t* data, uint32_t size)
{
  /* Здесь выполняют работу с оборудованием, для получения данных "с провода".
     Данные из оборудования заносят в buffer */
  memcpy(data, test_cases_data[test_cases_i], sizeof arp_asq_0);
  return sizeof arp_asq_0;
}


uint32_t hw_transmit(uint8_t* data, uint32_t size)
{
  /* Здесь выполняют работу с оборудованием, для выставления данных "на провод"
  Данные из data передают в оборудование */
  if (size != test_cases_len[test_cases_i])
  for (uint32_t i = 0; i < size; i ++)
  {
    printf("[%02u] %02x : %02x\n", i, arp_rep_0[i], data[i]);
    if (arp_rep_0[i] != data[i])
      exit(EXIT_FAILURE);
  }
  return size;
}
