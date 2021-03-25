#include "pipistrellus.h"


const uint8_t MAC_BROADCAST[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


bool arp_send(buffer* tx_buffer, const buffer* rx_buffer, const mac_addrs* mac_addr)
{
    if (tx_buffer->size_alloc < sizeof (arp_frame))
        return false;
    if (rx_buffer->size_used < sizeof (arp_frame))
        return false;
    arp_frame* tx_arpf = (arp_frame*) tx_buffer->data;
    memcpy(tx_arpf->maddrs.src, mac_addr->src, sizeof tx_arpf->maddrs.src);
    memcpy(tx_arpf->sndr_ip, mac_addr->src, sizeof tx_arpf->sndr_ip);
    arp_frame* rx_arpf = (arp_frame*) rx_buffer;
    memcpy(tx_arpf->maddrs.dst, rx_arpf->maddrs.src, sizeof tx_arpf->maddrs.dst);
    memcpy(tx_arpf->trgt_ip, rx_arpf->sndr_ip, sizeof tx_arpf->trgt_ip);
    tx_arpf->opcode = ARP_REP;
    tx_arpf->maddrs.type = rx_arpf->maddrs.type;
    tx_arpf->hw_type = rx_arpf->hw_type;
    tx_arpf->prot_type = rx_arpf->prot_type;
    return true;
}


bool arp_receive(const buffer* rx_buffer, const mac_addrs* maddr, uint32_t ip_addr)
{
    if (!mac_receive(rx_buffer, maddr))
        return false;
    if (rx_buffer->size_used < sizeof(arp_frame))
        return false;
    arp_frame* arpf = (arp_frame*) rx_buffer->data;
    if (arpf->opcode != ARP_ASQ)
        return false;
    if (arpf->maddrs.type != 0x0806)
        return false;
    return arpf->trgt_ip == ip_addr;
}


bool mac_set_addr(buffer* x_buffer, const mac_addrs* addr)
{
    if (x_buffer->size_alloc < sizeof *addr)
        return false;
    memcpy(x_buffer->data, addr, sizeof *addr);
    return true;
}


bool mac_init(mac_addrs* maddr, const uint8_t* src, const uint8_t* dst)
{
    memcpy(maddr->src, src, sizeof maddr->src);
    memcpy(maddr->dst, dst, sizeof maddr->dst);
    maddr->type = 0U;
}


bool mac_receive(const buffer* rx_buffer, const mac_addrs* maddr)
{
    if (rx_buffer->size_used < sizeof *maddr)
        return false;
    mac_addrs* rx_madrs = (mac_addrs*) rx_buffer->data;
    if (memcmp(rx_madrs->dst, MAC_BROADCAST, sizeof rx_madrs->dst) == 0)
        return true;
    return memcmp(rx_madrs->dst, maddr->src, sizeof rx_madrs->dst) == 0;
}


bool mac_send(buffer* x_buffer, const mac_addrs* addr)
{
    if (x_buffer->size_alloc < sizeof *addr)
        return false;
    memcpy(x_buffer->data, addr, sizeof *addr);
    return true;
}


bool icmp_receive(const buffer* rx_buffer, const mac_addrs* maddr, uint32_t ip_addr)
{
    if (mac_receive(rx_buffer, maddr))
        return ip_addr == ((ip_header*)(rx_buffer->data + sizeof(mac_addrs)))->dstadr;
    return false;
}


bool icmp_send(buffer* tx_buffer, const buffer* rx_buffer)
{
    return true;
}
