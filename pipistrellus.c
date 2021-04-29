#include "pipistrellus.h"

const uint8_t  MAC_BROADCAST[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const uint8_t  MAC_ZERO[]      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t  IP_ANY_ADDR[]   = {  0U,   0U,   0U,   0U};
const uint16_t IP_ANY_PORT     = 0U;
const uint8_t  IP_BROAD_MASK[] = {0xff, 0xff, 0xff, 0xff};


extern int eth_printf(const char* fmt, ...);


void udp_addr_clear(udp_addr* udpa)
{
    memset(udpa, 0U, sizeof *udpa);
}


void udp_addr_init(udp_addr* udpa, const uint8_t* addr, uint16_t port)
{
    udpa->addr = *(uint32_t*) addr;
    udpa->port = swap16(port);
}


bool udp_send(buffer* tx_buffer, const net_node* sndr, const net_node* trgt, size_t len)
{
    if (tx_buffer->size_alloc < sizeof (udp_frame))
        return false;
    
    udp_frame* udpf = (udp_frame*) tx_buffer->data;
    
    memcpy(udpf->maddrs.sndr, sndr->mac, sizeof udpf->maddrs.sndr);
    memcpy(udpf->maddrs.trgt, trgt->mac, sizeof udpf->maddrs.trgt);
    udpf->maddrs.type = ETH_TYPE_IPV4;
    
    udpf->sndr_addr = sndr->udp->addr;
    udpf->sndr_port = sndr->udp->port;
    udpf->trgt_addr = trgt->udp->addr;
    udpf->trgt_port = trgt->udp->port;
    udpf->id        = 0U;
    udpf->flags     = UDP_FLAGS;
    udpf->verlen    = ICMP_VERLEN;
    udpf->ttl       = UDP_TTL;
    udpf->proto     = UDP_PROTO;
    udpf->xsum      = 0U;
    udpf->xsumd     = 0U;
    udpf->tos       = UDP_TOS;
    udpf->length    = swap16(sizeof *udpf - sizeof udpf->trgt_port - sizeof udpf->sndr_port);
    udpf->lendg     = swap16(len + sizeof udpf->sndr_addr + sizeof udpf->trgt_addr);

    udpf->xsum = get_checksum(&udpf->verlen, (uint32_t)((uint8_t*)&udpf->sndr_port - (uint8_t*)&udpf->verlen), 0U);

    tx_buffer->size_used = sizeof(udp_frame) + len;
    return false;       
}


bool udp_get_data(buffer* x_buffer, udp_buffer* udpb)
{
    udp_frame* udpf = (udp_frame*) x_buffer->data;
    uint16_t size_used = swap16(udpf->lendg);
    if (size_used < (sizeof udpf->sndr_addr + sizeof udpf->trgt_addr))
        return false;
    udpb->size_used = size_used - sizeof udpf->sndr_addr - sizeof udpf->trgt_addr;
    udpb->data = x_buffer->data + sizeof(udp_frame);
    return true;
}


bool udp_set_data(buffer* x_buffer, udp_buffer* udpb)
{
    if (udpb->size_used > x_buffer->size_alloc - sizeof (udp_frame))
        return false;
    udpb->data = x_buffer->data + sizeof(udp_frame);
    return true;
}


bool udp_receive(const buffer* rx_buffer, const udp_addr* trgt, net_node* sndr, const uint8_t* netmask)
{
    if (rx_buffer->size_used < sizeof(udp_frame))
        return false;
    
    udp_frame* udpf = (udp_frame*) rx_buffer->data;
    if (udpf->proto != UDP_PROTO)
        return false;

    if (udpf->trgt_addr == ((*(uint32_t*)IP_BROAD_MASK) & (~ (*(uint32_t*)netmask))))
        if ((udpf->trgt_addr & (*(uint32_t*)netmask)) == (trgt->addr & (*(uint32_t*)netmask)))
            return udpf->trgt_port == trgt->port;

    if (sndr->udp->port != IP_ANY_PORT)
        if (udpf->trgt_port != trgt->port)
            return false;

    if (sndr->udp->addr != *(uint32_t*)IP_ANY_ADDR)
        if (udpf->trgt_addr != trgt->addr)
            return false;
    
    sndr->udp->addr = udpf->sndr_addr;
    sndr->udp->port = udpf->sndr_port;
    memcpy(sndr->mac, udpf->maddrs.sndr, sizeof udpf->maddrs.sndr);
    return true;
}


void udp_init_addr(udp_addr* udp_addr, const uint8_t* addr, const uint16_t port)
{
    udp_addr->addr = *(uint32_t*)addr;
    udp_addr->port = port;
}


bool process_whire(whire* pwhire)
{
    if (!mac_receive(pwhire->rx, pwhire->mac))
        return false;
    if (arp_receive(pwhire->rx, pwhire->ip))
    {
        arp_send(pwhire->tx, pwhire->rx, pwhire->mac, pwhire->ip);
        return true;
    }
    else if (icmp_receive(pwhire->rx, pwhire->ip))
    {
        icmp_send(pwhire->tx, pwhire->rx);
        return true;
    }
    return false;
}


void buffer_init(buffer* buf)
{
    buf->size_alloc = sizeof buf->data;
    buf->size_used = 0U;
}


uint16_t get_checksum(const void *in_data, size_t count, uint32_t sum)
{
    uint16_t* data = (uint16_t*) in_data;
    
    while (count > 1U)
    {
        sum += *data ++;
        count -= sizeof data[0];
    }

    if (count > 0U)
        sum += * (uint8_t*) data;

    while (sum >> 16U)
       sum = (sum & 0xffff) + (sum >> 16U);

   return ~((uint16_t)sum);
}


bool arp_send(buffer* tx_buffer, const buffer* rx_buffer, const uint8_t* sndr_mac, const uint8_t* sndr_ip)
{
    tx_buffer->size_used = 0U;
    if (tx_buffer->size_alloc < sizeof(arp_frame))
        return false;
    if (rx_buffer->size_used < sizeof(arp_frame))
        return false;
    
    arp_frame* tx_arpf = (arp_frame*) tx_buffer->data;
    arp_frame* rx_arpf = (arp_frame*) rx_buffer->data;
    
    memcpy(tx_arpf->maddrs.sndr, sndr_mac,             sizeof tx_arpf->maddrs.sndr);
    memcpy(tx_arpf->sndr_mac,    sndr_mac,             sizeof tx_arpf->sndr_mac);
    memcpy(tx_arpf->maddrs.trgt, rx_arpf->maddrs.sndr, sizeof tx_arpf->maddrs.trgt);
    memcpy(tx_arpf->trgt_mac,    rx_arpf->sndr_mac,    sizeof tx_arpf->trgt_mac);
    
    tx_arpf->maddrs.type = rx_arpf->maddrs.type;
    tx_arpf->sndr_ip     = *(uint32_t*)sndr_ip;
    tx_arpf->trgt_ip     = rx_arpf->sndr_ip;
    tx_arpf->opcode      = ARP_OPCODE_REP;
    tx_arpf->hw_type     = rx_arpf->hw_type;
    tx_arpf->hw_size     = rx_arpf->hw_size;
    tx_arpf->prot_size   = rx_arpf->prot_size;
    tx_arpf->prot_type   = rx_arpf->prot_type;
    
    tx_buffer->size_used = sizeof *tx_arpf;
    
    return true;
}


bool arp_receive(const buffer* rx_buffer, const uint8_t* ip_addr)
{
    if (rx_buffer->size_used < sizeof(arp_frame))
        return false;
    arp_frame* arpf = (arp_frame*) rx_buffer->data;
    if (arpf->maddrs.type != ETH_TYPE_ARP)
        return false;
    if (arpf->opcode != ARP_OPCODE_ASQ)
        return false;
    return arpf->trgt_ip == *(uint32_t*)ip_addr;
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
    memcpy(maddr->sndr, src, sizeof maddr->sndr);
    memcpy(maddr->trgt, dst, sizeof maddr->trgt);
    maddr->type = 0U;
}


bool mac_receive(const buffer* rx_buffer, const uint8_t* sndr_mac)
{
    if (rx_buffer->size_used < sizeof(mac_addrs))
        return false;
    mac_addrs* rx_madrs = (mac_addrs*) rx_buffer->data;
    if (memcmp(rx_madrs->trgt, MAC_BROADCAST, sizeof rx_madrs->trgt) == 0)
        return true;
    if (memcmp(rx_madrs->trgt, MAC_ZERO, sizeof rx_madrs->trgt) == 0)
        if (memcmp(rx_madrs->sndr, MAC_ZERO, sizeof rx_madrs->sndr) == 0)
            return true;
    return memcmp(rx_madrs->trgt, sndr_mac, sizeof rx_madrs->trgt) == 0;
}


bool mac_send(buffer* tx_buffer, const mac_addrs* maddrs)
{
    if (tx_buffer->size_alloc < sizeof *maddrs)
        return false;
    memcpy(tx_buffer->data, maddrs, sizeof *maddrs);
    return true;
}


bool icmp_receive(const buffer* rx_buffer, const uint8_t* trgt_ip)
{
    if (rx_buffer->size_used < sizeof (icmp_frame))
        return false;
    icmp_frame* icmpf = (icmp_frame*) rx_buffer->data;
    if (icmpf->maddrs.type != ETH_TYPE_IPV4)
        return false;
    if (icmpf->proto != ICMP_PROTO)
        return false;
    if (icmpf->opcode != ICMP_OPCODE_ASQ)
        return false;
    return icmpf->trgt_ip == *(uint32_t*)trgt_ip;
}


bool icmp_send(buffer* tx_buffer, const buffer* rx_buffer)
{
    tx_buffer->size_used = 0U;
    if (rx_buffer->size_used < sizeof(icmp_frame))
        return false;
    if (tx_buffer->size_alloc < rx_buffer->size_used)
        return false;
    
    tx_buffer->size_used  = rx_buffer->size_used;

    icmp_frame* tx_icmpf = (icmp_frame*) tx_buffer->data;
    icmp_frame* rx_icmpf = (icmp_frame*) rx_buffer->data;

    memcpy(tx_icmpf->maddrs.trgt, rx_icmpf->maddrs.sndr, sizeof tx_icmpf->maddrs.trgt);
    memcpy(tx_icmpf->maddrs.sndr, rx_icmpf->maddrs.trgt, sizeof tx_icmpf->maddrs.sndr);
    tx_icmpf->maddrs.type = rx_icmpf->maddrs.type;
    tx_icmpf->dsc_ecn     = rx_icmpf->dsc_ecn; 
    tx_icmpf->verlen      = rx_icmpf->verlen;
    tx_icmpf->totlen      = rx_icmpf->totlen;
    tx_icmpf->id          = swap16(swap16(rx_icmpf->id) + 1U);
    tx_icmpf->flags       = rx_icmpf->flags;
    tx_icmpf->hdr_csum    = 0U;
    tx_icmpf->csum        = 0U;
    tx_icmpf->ttl         = rx_icmpf->ttl;
    tx_icmpf->proto       = rx_icmpf->proto;
    tx_icmpf->sndr_ip     = rx_icmpf->trgt_ip;
    tx_icmpf->trgt_ip     = rx_icmpf->sndr_ip;
    tx_icmpf->opcode      = ICMP_OPCODE_REP;
    tx_icmpf->code        = rx_icmpf->code;
    tx_icmpf->be          = rx_icmpf->be;
    tx_icmpf->le          = rx_icmpf->le;
    
    memcpy(tx_icmpf->time, rx_icmpf->time, sizeof tx_icmpf->time);
    memcpy(tx_buffer->data + sizeof(*tx_icmpf), rx_buffer->data + sizeof(*rx_icmpf), tx_buffer->size_used - sizeof(*tx_icmpf));
    
    tx_icmpf->csum        = get_checksum(&tx_icmpf->opcode, tx_buffer->size_used - (uint32_t)((uint8_t*) &tx_icmpf->opcode - (uint8_t*)tx_icmpf), 0U);
    tx_icmpf->hdr_csum    = get_checksum(&tx_icmpf->verlen, (uint32_t) ((uint8_t*) &tx_icmpf->opcode - (uint8_t*) &tx_icmpf->verlen), 0U);
    
    return true;
}
