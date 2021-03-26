#include "pipistrellus.h"


const uint8_t MAC_BROADCAST[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


bool udp_init_addr(udp_addr* udp_addr, const uint8_t* addr, const uint16_t port)
{
    memcpy(&udp_addr->addr.dword, addr, sizeof udp_addr->addr.dword);
    uint8_t* p = (uint8_t*) &port;
    udp_addr->port.bytes[0] = p[1];
    udp_addr->port.bytes[1] = p[0];
}


bool udp_receive(const buffer* rx_buffer,  const udp_addr* src, const udp_addr* dst)
{
    return false;
}


bool udp_send(buffer* tx_buffer, const udp_addr* src, const udp_addr* dst)
{
    return true;
}


bool udp_get_buffer(const buffer* x_buffer, buffer* udp_buffer)
{
    // TODO: Заглушка
    udp_buffer->data = x_buffer->data;
    udp_buffer->size_alloc = x_buffer->size_alloc;
    udp_buffer->size_used = x_buffer->size_used;
    return true;
}


bool arp_send(buffer* tx_buffer, const buffer* rx_buffer, const mac_addrs* mac_addr, uint32_t ip_addr)
{
    if (tx_buffer->size_alloc < sizeof (arp_frame))
        return false;
    if (rx_buffer->size_used < sizeof (arp_frame))
        return false;
    
    arp_frame* tx_arpf = (arp_frame*) tx_buffer->data;
    arp_frame* rx_arpf = (arp_frame*) rx_buffer->data;
    
    memcpy(&tx_arpf->maddrs.src, &mac_addr->src,        sizeof tx_arpf->maddrs.src);
    memcpy(&tx_arpf->sndr_ip,    &ip_addr,              sizeof tx_arpf->sndr_ip);
    memcpy(&tx_arpf->sndr_mac,   &mac_addr->src,        sizeof tx_arpf->sndr_mac);
    memcpy(&tx_arpf->maddrs.dst, &rx_arpf->maddrs.src,  sizeof tx_arpf->maddrs.dst);
    memcpy(&tx_arpf->trgt_ip,    &rx_arpf->sndr_ip,     sizeof tx_arpf->trgt_ip);
    memcpy(&tx_arpf->trgt_mac,   &rx_arpf->sndr_mac,    sizeof tx_arpf->trgt_mac);

    tx_arpf->opcode = ARP_REP;
    tx_arpf->maddrs.type = rx_arpf->maddrs.type;
    tx_arpf->hw_type = rx_arpf->hw_type;
    tx_arpf->hw_size = rx_arpf->hw_size;
    tx_arpf->prot_size = rx_arpf->prot_size;
    tx_arpf->prot_type = rx_arpf->prot_type;
    tx_buffer->size_used = sizeof *tx_arpf;
    
    return true;
}


bool arp_receive(const buffer* rx_buffer, const mac_addrs* maddrs, uint32_t ip_addr)
{
    if (!mac_receive(rx_buffer, maddrs))
        return false;
    if (rx_buffer->size_used < sizeof(arp_frame))
        return false;
    arp_frame* arpf = (arp_frame*) rx_buffer->data;
    if (arpf->opcode != ARP_ASQ)
        return false;
    if (arpf->maddrs.type != ETH_TYPE_ARP)
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


bool mac_init_addr(mac_addrs* maddr, const uint8_t* src, const uint8_t* dst)
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
    if (!mac_receive(rx_buffer, maddr))
        return false;
    return ip_addr == ((ip_header*)(rx_buffer->data + sizeof(mac_addrs)))->dstadr;
}


bool icmp_send(buffer* tx_buffer, const buffer* rx_buffer)
{
    return true;
}
