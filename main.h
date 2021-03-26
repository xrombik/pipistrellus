
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "pipistrellus.h"


/** Заполняет buffer данными "с провода".
Определяется пользователем и используемым оборудованием */
uint32_t hw_receive(uint8_t* data, uint32_t size);

/** Передаёт содержимое buffer оборудованию "на провод".
Определяется пользователем и используемым оборудованием */
uint32_t hw_transmit(uint8_t* data, uint32_t size);
