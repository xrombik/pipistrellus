#include "pipistrellus.h"


uint8_t MAC_BROADCAST[]   = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const uint8_t MAC_ZERO[]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

extern uint32_t eth_printf(const char* fmt, ...);


uint32_t get_checksum(const void *data, uint32_t count)
{
    /* Compute Internet Checksum for "count" bytes beginning at location "addr"  */
    uint32_t sum = 0;

    while (count > 1)
    {
        /*  This is the inner loop */
        sum += *(uint16_t*) data ++;
        count -= 2;
    }

    /*  Add left-over byte, if any */
    if (count > 0)
        sum += * (uint8_t*) data;

    /*  Fold 32-bit sum to 16 bits */
    while (sum >> 16)
       sum = (sum & 0xffff) + (sum >> 16);

   return ~sum;
}


void udp_init_addr(udp_addr* udp_addr, const uint8_t* addr, const uint16_t port)
{
    memcpy(&udp_addr->addr, addr, sizeof udp_addr->addr);
    udp_addr->port = swap16(port);
}


bool udp_receive(const buffer* rx_buffer, const udp_addr* src, const udp_addr* dst)
{
    // TODO: Заглушка
    return false;
}


bool udp_send(buffer* tx_buffer, const udp_addr* src, const udp_addr* dst)
{
    // TODO: Заглушка
    return true;
}


bool udp_get_data(const buffer* x_buffer, buffer* udp_buffer)
{
    udp_frame* udpf = (udp_frame*) x_buffer->data;
    if (udpf->proto != UDP_PROTO)
        return false;
    uint16_t len = udpf->lendg - 8;
    if (len > udp_buffer->size_alloc)
        return false;
    udp_buffer->data = x_buffer->data + sizeof(udp_frame);
    udp_buffer->size_used = len;
    return true;
}


bool arp_send(buffer* tx_buffer, const buffer* rx_buffer, const mac_addrs* mac_addr, uint32_t ip_addr)
{
    if (tx_buffer->size_alloc < sizeof(arp_frame))
        return false;
    
    if (rx_buffer->size_used < sizeof(arp_frame))
        return false;
    
    arp_frame* tx_arpf = (arp_frame*) tx_buffer->data;
    arp_frame* rx_arpf = (arp_frame*) rx_buffer->data;
    
    memcpy(tx_arpf->maddrs.src, mac_addr->src,       sizeof tx_arpf->maddrs.src);
    memcpy(tx_arpf->sndr_mac,   mac_addr->src,       sizeof tx_arpf->sndr_mac);
    memcpy(tx_arpf->maddrs.dst, rx_arpf->maddrs.src, sizeof tx_arpf->maddrs.dst);
    memcpy(tx_arpf->trgt_mac,   rx_arpf->sndr_mac,   sizeof tx_arpf->trgt_mac);
    
    tx_arpf->maddrs.type = rx_arpf->maddrs.type;
    tx_arpf->sndr_ip     = ip_addr;
    tx_arpf->trgt_ip     = rx_arpf->sndr_ip;
    tx_arpf->opcode      = ARP_REP;
    tx_arpf->hw_type     = rx_arpf->hw_type;
    tx_arpf->hw_size     = rx_arpf->hw_size;
    tx_arpf->prot_size   = rx_arpf->prot_size;
    tx_arpf->prot_type   = rx_arpf->prot_type;
    
    tx_buffer->size_used = sizeof *tx_arpf;
    
    return true;
}


bool arp_receive(const buffer* rx_buffer, const mac_addrs* maddrs, uint32_t ip_addr)
{
    if (rx_buffer->size_used < sizeof(arp_frame))
        return false;
    arp_frame* arpf = (arp_frame*) rx_buffer->data;
    if (arpf->maddrs.type != ETH_TYPE_ARP)
        return false;
    if (arpf->opcode != ARP_ASQ)
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


void mac_init_addr(mac_addrs* maddr, const uint8_t* src, const uint8_t* dst)
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
    if (memcmp(rx_madrs->dst, MAC_ZERO, sizeof rx_madrs->dst) == 0)
        if (memcmp(rx_madrs->src, MAC_ZERO, sizeof rx_madrs->src) == 0)
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


bool icmp_receive(const buffer* rx_buffer, uint32_t ip_addr)
{
    if (rx_buffer->size_used < sizeof (icmp_frame))
        return false;
    icmp_frame* icmpf = (icmp_frame*) rx_buffer->data;
    if (icmpf->maddrs.type != ETH_TYPE_IPV4)
        return false;
    if (icmpf->proto != ICMP_PROTO)
        return false;
    if (icmpf->opcode != ICMP_TYPE_ASQ)
        return false;
    return icmpf->trgt_ip == ip_addr;
}


bool icmp_send(buffer* tx_buffer, const buffer* rx_buffer)
{
    if (rx_buffer->size_used < sizeof (icmp_frame))
        return false;
    if (tx_buffer->size_alloc < rx_buffer->size_used)
        return false;

    icmp_frame* tx_icmpf = (icmp_frame*) tx_buffer->data;
    icmp_frame* rx_icmpf = (icmp_frame*) rx_buffer->data;

    memcpy(tx_icmpf->maddrs.dst, rx_icmpf->maddrs.src, sizeof tx_icmpf->maddrs.dst);
    memcpy(tx_icmpf->maddrs.src, rx_icmpf->maddrs.dst, sizeof tx_icmpf->maddrs.src);
    tx_icmpf->maddrs.type = rx_icmpf->maddrs.type;
    tx_icmpf->dsc_ecn     = rx_icmpf->dsc_ecn; 
    tx_icmpf->verlen      = rx_icmpf->verlen;
    tx_icmpf->totlen      = rx_icmpf->totlen;
    tx_icmpf->id          = swap16(swap16(rx_icmpf->id) + 1U);
    tx_icmpf->flags       = 0U;
    tx_icmpf->hdr_csum    = 0U;
    tx_icmpf->csum        = 0U;
    tx_icmpf->ttl         = rx_icmpf->ttl;
    tx_icmpf->proto       = rx_icmpf->proto;
    tx_icmpf->sndr_ip     = rx_icmpf->trgt_ip;
    tx_icmpf->trgt_ip     = rx_icmpf->sndr_ip;
    tx_icmpf->opcode      = ICMP_TYPE_REP;
    tx_icmpf->code        = rx_icmpf->code;
    tx_icmpf->be          = rx_icmpf->be;
    tx_icmpf->le          = rx_icmpf->le;
    
    memcpy(tx_icmpf->time, rx_icmpf->time, sizeof tx_icmpf->time);
    memcpy(tx_buffer->data + sizeof(*tx_icmpf), rx_buffer->data + sizeof(*rx_icmpf), rx_buffer->size_used - sizeof(*rx_icmpf));
    
    tx_buffer->size_used = rx_buffer->size_used;
    // TODO: Разобраться почему нужно "- 4"
    tx_icmpf->csum       = get_checksum(&tx_icmpf->opcode, tx_buffer->size_used - (uint32_t)(&tx_icmpf->opcode - (uint8_t*)tx_icmpf) - 4);
    tx_icmpf->hdr_csum   = get_checksum(&tx_icmpf->verlen, (uint32_t) ((uint8_t*) &tx_icmpf->opcode - (uint8_t*) &tx_icmpf->verlen));
    
    return true;
}
