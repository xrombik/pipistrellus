# Нетопырь *(лат. - pipistrellus)*

Тонкая библиотека для управления UDP и ICMP датаграммами

# Особенности

1. Операция копирования используется только для инициализации
2. Динамическое выделение памяти не используется

# Ограничения

1. Работает только с IPV4
2. Не собирает фрагментированные UDP датаграммы

# Использование

## Действующие сущности

1. Нетопырь
2. Пользователь
3. Непонятное оборудование Ethernet

## Пример программы

```C

#include <stdint.h>

#include "pipistrellus.h"
#include "main.h"

const char* MAC_SRC = {0x01, 0x02, 0x03, 0x04, 0x05};
const char* MAC_DST = {0xff, 0xff, 0xff, 0xff, 0xff};


int main(int argc, char** argv)
{
  /* Нетопырю всё равно что здесь происходит
  ... */
  
  mac_addr maddr;
  mac_init(&maddr, MAC_SRC, MAC_DST);
  
  buffer rx_buffer;
  uint8_t rx_data[1500];
  rx_buffer.data = rx_data;
  rx_buffer.size = sizeof rx_data;

  buffer tx_buffer;
  uint8_t tx_data[1500];
  tx_buffer.data = tx_data;
  tx_buffer.size = sizeof tx_data;

  /* Здесь оборудование должно быть готово для приёма и передачи данных */
  while(1)
  {
    /* Получить пакет "с провода" */
    rx_buffer.size = hw_receive(rx_buffer.data);
    
    /* Здесь нетопырь ожидает, что rx_buffer.data будет содержать mac-адреса,
    тип, IP-заголовок и данные, а rx_buffer.size длину eth-фрейма */
    
    /* Здесь нетопырь обработает запросы ICMP */
    if (icmp_receive(rx_buffer, &maddr))
    {
      icmp_send(&rx_buffer, &maddr, &tx_buffer);
      hw_transmit(tx_buffer.data, rx_buffer.len, &maddr);
    }
    
    /* Здесь нетопырь обработает запросы UDP */
    if (udp_receive(&rx_buffer, &udp_src, &udp_dst))
    {
      buffer udp_rx_buffer;
      if (udp_get_data(&rx_buffer, &udp_rx_buffer))
      {
        /* Здесь пользователь может обработать udp-датаграмму, например так: */
        if (udp_rx_buffer.size > 20)
          printf("data[8]:%u\n", udp_rx_buffer.data[8]);
      }
    }
    
    /* Здесь пользователь может отправить данные в сеть, например так: */
    if (udp_buffer.size > 10)
    {
      buffer tx_buffer;
      if (udp_get_data(&tx_buffer, &tx_buffer))
      {
        tx_buffer.size = strcpy(tx_buffer.data, "pipistrellus");
        udp_send(&tx_buffer, &udp_src, &udp_dst);
        mac_send(&tx_buffer, &mac_addr);
        hw_transmit(tx_buffer.data, tx_buffer.size);
      }
    }
  }
}

/** Заполняет buffer данными "с провода".
Определяется пользователем и особенностями используемого оборудования */
uint32_t hw_receive(uint8_t* data, uint32_t size)
{
  /* Здесь выполняют работу с оборудованием, для получения данных "с провода".
  Данные из оборудования заносят в buffer, например так:
  uint32_t i;
  for (i = 0; i < size; i ++)
  {
    if (...) break;
    data[i] = ...;
  }
  ...
  */
  return i;
}

/* Передаёт содержимое buffer оборудованию "на провод".
Определяется пользователем и особенностями используемого оборудования */
uint32_t hw_transmit(uint8_t* data, uint32_t size, mac_addr* maddr)
{
  /* Здесь выполняют работу с оборудованием, для выставлеия данных "на провод"
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

```
