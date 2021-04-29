
# Текущее состояние

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/346ae29f295646cdba20827563c55595)](https://app.codacy.com/gh/xrombik/pipistrellus?utm_source=github.com&utm_medium=referral&utm_content=xrombik/pipistrellus&utm_campaign=Badge_Grade_Settings)
[![CMake](https://github.com/xrombik/pipistrellus/actions/workflows/cmake.yml/badge.svg)](https://github.com/xrombik/pipistrellus/actions/workflows/cmake.yml)
[![Build Status](https://travis-ci.com/xrombik/pipistrellus.svg?branch=main)](https://travis-ci.com/xrombik/pipistrellus)


Что уже работает:
 - arp-протокол
 - icmp-протокол в части выполнеия проверки связи (пинг)
 - udp-протокол с нефрагментированными датаграммами


![Open for everyone](pipistrellus.jpeg)

# Нетопырь *(лат. - pipistrellus)*

 - Тонкая библиотека для управления ARP, ICMP и UDP датаграммами
 - Предназначена для выполнения на микрокотроллерах

# Особенности

 - Операции копирования производит только для служебных полей
 - Динамическое выделение памяти не использует
 - Зависит только от стандартной библиотеки Си

# Ограничения

 - Работает только с IPV4
 - Не собирает фрагментированные UDP датаграммы

# Действующие сущности

 - Нетопырь
 - Пользователь
 - Оборудование

# Использование

Пользователь добавляет в свой проект два файла: 

 - [pipistrellus.с](pipistrellus.c)
 - [pipistrellus.h](pipistrellus.h)
 

# Пример программы

[main.c](main.c)
