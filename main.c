#include <stdint.h>

#include "pipistrellus.h"
#include "main.h"

#include "test_data.c"

/** \file */

#define BUFFER_SIZE 1500

extern uint8_t MAC_BROADCAST[6];
const uint8_t  NULL_ADDR[] = {0x00, 0x00, 0x00, 0x00};

/* mac-адрес этого узла */
const uint8_t MAC_SRC[] = {0x70, 0x4d, 0x7b, 0x65, 0x2a, 0xd0};

const uint8_t  IP_MASK[] = {255, 255, 255,   0};  /**< маска подсети */
const uint8_t  IP_ADDR[] = {172,  16,   1,  10};  /**< ip-адрес этого узла */
const uint16_t IP_PORT   = 50090;                 /**< порт назначения */

const void* test_asq_data  [TEST_CASES_COUNT];
uint16_t    test_asq_len   [TEST_CASES_COUNT];
uint16_t    test_case_i  = 0;

const void* test_repl_data [TEST_CASES_COUNT];
uint16_t    test_repl_len  [TEST_CASES_COUNT];
uint16_t    test_repl_i  = 0;

int main(int argc, char** argv)
{
    /* Настройка оборудования.
       Нетопырю всё равно что здесь происходит
       ... */

    /* Здесь пользователь сообщает нетопырю адреса
       и размеры буферов для приёма и передачи */
    buffer rx_buffer;
    uint8_t rx_data[BUFFER_SIZE] = {0};
    rx_buffer.data = rx_data;
    rx_buffer.size_alloc = sizeof rx_data;

    buffer tx_buffer;
    uint8_t tx_data[BUFFER_SIZE] = {0};
    tx_buffer.data = tx_data;
    tx_buffer.size_alloc = sizeof tx_data;
  
    mac_addrs maddr;
    mac_init_addr(&maddr, MAC_SRC, MAC_BROADCAST);

    udp_addr self_addr;
    udp_init_addr(&self_addr, IP_ADDR, IP_PORT);

    udp_addr trgt_addr;
    udp_init_addr(&trgt_addr, NULL_ADDR, 0);

    const uint32_t* netmask = (const uint32_t*) IP_MASK;

    /* Заполнение тестовых случаев */
    test_asq_data[0]  = arp_asq_0;
    test_asq_len[0]   = sizeof arp_asq_0;
    test_repl_data[0] = arp_rep_0;
    test_repl_len[0]  = sizeof arp_rep_0;

    test_asq_data[1]  = icmp_asq_0;
    test_asq_len[1]   = sizeof icmp_asq_0;
    test_repl_data[1] = icmp_rep_0;
    test_repl_len[1]  = sizeof icmp_rep_0;
  
    /* Здесь оборудование должно быть готово для приёма и передачи данных */
    for (test_case_i = 1U; test_case_i < sizeof test_asq_data / sizeof test_asq_data[0]; test_case_i ++)
    {
        /* Получить пакет "с провода" */
        rx_buffer.size_used = hw_receive(rx_buffer.data, rx_buffer.size_alloc);

        if (!mac_receive(&rx_buffer, &maddr))
            continue;

        /* Здесь нетопырь ожидает, что rx_buffer.data будет содержать mac-адреса,
           тип, IP-заголовок и данные, а rx_buffer.size длину eth-фрейма */

        /* Здесь нетопырь обработает запросы ICMP */
        if (icmp_receive(&rx_buffer, self_addr.addr))
        {
            icmp_send(&tx_buffer, &rx_buffer);
        }

        /* Здесь нетопырь обработает запросы ARP */
        else if (arp_receive(&rx_buffer, &maddr, self_addr.addr))
        {
            arp_send(&tx_buffer, &rx_buffer, &maddr, self_addr.addr);
        }

        /* Здесь нетопырь обработает запросы UDP */
        else if (udp_receive(&rx_buffer, &trgt_addr, &self_addr))
        {
            buffer udp_rx_buffer;
            udp_rx_buffer.size_used = 0U;
            udp_get_data(&rx_buffer, &udp_rx_buffer);

            /* Здесь пользователь может обработать udp-датаграмму и отправить ответ
               внешнему узлу например так: */
            if (udp_rx_buffer.size_used > 20U)
            {
                printf("data[8]:%u\n", udp_rx_buffer.data[8]);
                /* Здесь пользователь может отправить данные в сеть, например так: */
                buffer udp_tx_buffer;
                udp_get_data(&tx_buffer, &udp_tx_buffer);
                strncpy(udp_tx_buffer.data, "pipistrellus", udp_tx_buffer.size_alloc);
                udp_tx_buffer.size_used = strlen(udp_tx_buffer.data) + sizeof '\0';
                udp_send(&tx_buffer, &self_addr, &trgt_addr);
                mac_set_addr(&tx_buffer, &maddr);
            }
        }
        else
        {
            tx_buffer.size_used = 0;
        }
        hw_transmit(tx_buffer.data, tx_buffer.size_used);
    }
    return EXIT_SUCCESS;
}


uint16_t hw_receive(uint8_t* data, uint16_t size)
{
  /* Здесь выполняют работу с оборудованием, для получения
     данных "с провода". Данные из оборудования заносят в buffer */
  memcpy(data, test_asq_data[test_case_i], test_asq_len[test_case_i]);
  return test_asq_len[test_case_i];
}


uint16_t hw_transmit(uint8_t* data, uint16_t size)
{
  /* Здесь выполняют работу с оборудованием, для
     выставления данных "на провод". Данные из
     data передают в оборудование */
  printf("len ref:%u fact:%u\n", test_repl_len[test_case_i], size);
  if (size != test_repl_len[test_case_i])
    exit(EXIT_FAILURE);
  for (uint16_t i = 0; i < size; i ++)
  {
    uint8_t byte_ref = ((uint8_t*)test_repl_data[test_case_i])[i];
    uint8_t byte_fact = data[i];
    printf("[%02u] %02x : %02x\n", i, byte_ref, byte_fact);
    if (byte_ref != byte_fact)
      exit(EXIT_FAILURE);
  }
  return size;
}
