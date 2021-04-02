#!/bin/bash

# Настраивает итерфейс IFACE для генерации тестовых пакетов

IFACE=lo
#ip link set dev ${IFACE} down
#ip link set dev ${IFACE} address 70:4d:7b:65:2a:d0
#ip link set dev ${IFACE} up
ifconfig ${IFACE}:10 172.16.1.10 netmask 255.255.255.0
ifconfig ${IFACE}:30 172.16.1.30 netmask 255.255.255.0
