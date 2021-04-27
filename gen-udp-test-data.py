# coding=utf-8
import socket
import argparse

"""
Генерирует "запрос" и "ответ" согласно udp-протоколу.
Захват пакетов производить с помощью wireshark
"""

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-port',     default=50090,         help='Порт назначения')
    parser.add_argument('-selfaddr', default='172.16.1.10', help='Свой адрес')
    parser.add_argument('-extaddr',  default='172.16.1.30', help='Внешний адрес')
    parser.add_argument('-data',     default='1234567890',  help='Строка для отправки')
    opts, opts_unk = parser.parse_known_args()
    for opt in opts_unk:
        print(u'внимание: неизвестный аргумент: \"%s\"' % opt)

    self_s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self_s.bind((opts.selfaddr, opts.port))

    ext_s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    ext_s.bind((opts.extaddr, opts.port))

    ext_s.sendto(opts.data, (opts.selfaddr, opts.port))

    data = self_s.recv(1500)
    self_s.sendto(data, (opts.extaddr, opts.port))


if __name__ == '__main__':
    main()
