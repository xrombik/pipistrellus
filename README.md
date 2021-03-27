![Open for everyone](pipistrellus.jpeg)
[![CMake](https://github.com/xrombik/pipistrellus/actions/workflows/cmake.yml/badge.svg)](https://github.com/xrombik/pipistrellus/actions/workflows/cmake.yml)

# Нетопырь *(лат. - pipistrellus)*

 - Тонкая библиотека для управления ARP, ICMP и UDP датаграммами
 - Предназачеа для выполнения на микрокотроллерах

# Особенности

1. Операция копирования используется только для служебных полей
2. Динамическое выделение памяти не используется

# Ограничения

1. Работает только с IPV4
2. Не собирает фрагментированные UDP датаграммы

# Использование

Добавьте в свой код два файла: 
 - pipistrellus.h
 - pipistrellus.с

## Действующие сущности

1. Нетопырь
2. Пользователь
3. Оборудование

## Пример программы

[показать](main.c)
