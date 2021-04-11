#!/bin/bash

# TODO: Заглушка
# Генерация тестовых пакетов.

# apt-get install iputils-arping -y
arping -s 172.16.1.30 172.16.1.10 -c 1 -I lo:30
ping -I 172.16.2.30 172.16.2.10 -c 1

