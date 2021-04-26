#include <stdint.h>

#include "pipistrellus.h"
#include "main.h"

#include "test_data.c"

/** \file */

extern uint8_t MAC_BROADCAST[6];
const uint8_t  NULL_ADDR[] = {0x00, 0x00, 0x00, 0x00};

/* mac-адрес этого узла */
const uint8_t SELF_MAC[]  = {0x70, 0x4d, 0x7b, 0x65, 0x2a, 0xd0};

const uint8_t  IP_MASK[] = {255, 255, 255,   0};  /**< маска подсети */
const uint8_t  SELF_IP_ADDR[] = {172,  16,   1,  10};  /**< ip-адрес этого узла */
const uint16_t IP_PORT   = 50090;                 /**< порт назначения */

const void* test_asq_data  [TEST_CASES_COUNT];
uint16_t    test_asq_len   [TEST_CASES_COUNT];
uint16_t    test_case_i  = 0U;

const void* test_repl_data [TEST_CASES_COUNT];
uint16_t    test_repl_len  [TEST_CASES_COUNT];
uint16_t    test_repl_i  = 0U;

int main(int argc, char** argv)
{
    /* Настройка оборудования.
       Нетопырю всё равно что здесь происходит
       ... */

    /* Здесь пользователь сообщает нетопырю адреса
       и размеры буферов для приёма и передачи */
    buffer rx_buffer;
    buffer_init(&rx_buffer);

    buffer tx_buffer;
    buffer_init(&tx_buffer);
  
    mac_addrs maddr;
    mac_init_addr(&maddr, SELF_MAC, MAC_BROADCAST);

    /* Заполнение тестовых случаев */
    test_asq_data [0] = arp_asq_0;
    test_asq_len  [0] = sizeof arp_asq_0;
    test_repl_data[0] = arp_rep_0;
    test_repl_len [0] = sizeof arp_rep_0;

    test_asq_data [1] = icmp_asq_0;
    test_asq_len  [1] = sizeof icmp_asq_0;
    test_repl_data[1] = icmp_rep_0;
    test_repl_len [1] = sizeof icmp_rep_0;
  
    /* Здесь оборудование должно быть готово для приёма и передачи данных */
    for (test_case_i = 0U; test_case_i < sizeof test_asq_data / sizeof test_asq_data[0]; test_case_i ++)
    {
        /* Получить пакет "с провода" */
        rx_buffer.size_used = hw_receive(rx_buffer.data, rx_buffer.size_alloc);

        if (!mac_receive(&rx_buffer, SELF_MAC))
            continue;

        /* Здесь нетопырь ожидает, что rx_buffer.data будет содержать mac-адреса,
           тип, IP-заголовок и данные, а rx_buffer.size длину eth-фрейма */

        /* Здесь нетопырь обработает запросы ICMP */
        if (icmp_receive(&rx_buffer, SELF_IP_ADDR))
        {
            icmp_send(&tx_buffer, &rx_buffer);
        }
        /* Здесь нетопырь обработает запросы ARP */
        else if (arp_receive(&rx_buffer, SELF_IP_ADDR))
        {
            arp_send(&tx_buffer, &rx_buffer, SELF_MAC, SELF_IP_ADDR);
        }
        else
        {
            tx_buffer.size_used = 0U;
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
    printf("test_case:%u len ref:%u fact:%u\n", test_case_i, test_repl_len[test_case_i], size);
    if (size != test_repl_len[test_case_i])
        exit(EXIT_FAILURE);
    for (uint16_t i = 0U; i < size; i ++)
    {
        uint8_t byte_ref = ((uint8_t*)test_repl_data[test_case_i])[i];
        uint8_t byte_fact = data[i];
        printf("[0x%03x:%u] %02x : %02x\n", i, i, byte_ref, byte_fact);
        if (byte_ref != byte_fact)
            exit(EXIT_FAILURE);
    }
    return size;
}
