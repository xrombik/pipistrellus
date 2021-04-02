#!/bin/bash

# Возвращает итерфейс IFACE в исходое состояние

IFACE=lo
#DEF_MAC=$(ethtool -P ${IFACE} | grep -iEom1 '([0-9a-f]{2}:){5}[0-9a-f]{2}')

ip addr flush dev ${IFACE}:30
ip addr flush dev ${IFACE}:30
#ip link set dev ${IFACE} down
#ip link set dev ${IFACE} address ${DEF_MAC}
#ip link set dev ${IFACE} up
#service networking restart
