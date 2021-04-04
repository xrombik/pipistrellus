
# Текущее состояние

[![CMake](https://github.com/xrombik/pipistrellus/actions/workflows/cmake.yml/badge.svg)](https://github.com/xrombik/pipistrellus/actions/workflows/cmake.yml)

*в процессе разработки ...*

Что уже работает:
- arp-протокол
- icmp-протокол в части выполнеия проверки связи (пинг)

Что будет добавлено:
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

# Использование

Добавьте в свой код два файла: 
 - pipistrellus.h
 - pipistrellus.с

## Действующие сущности

 - Нетопырь
 - Пользователь
 - Оборудование

## Пример программы

[показать](main.c)
