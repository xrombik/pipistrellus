
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "pipistrellus.h"

/** \file */

/** Заполняет buffer данными "с провода".
Определяется пользователем и используемым оборудованием */
uint16_t hw_receive(uint8_t* data, uint16_t size);

/** Передаёт содержимое buffer оборудованию "на провод".
Определяется пользователем и используемым оборудованием */
uint16_t hw_transmit(uint8_t* data, uint16_t size);
