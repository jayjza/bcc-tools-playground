#include <uapi/linux/bpf.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <linux/udp.h>


#define DEFAULT_ACTION XDP_PASS

BPF_TABLE(MAPTYPE, uint32_t, long, dropcnt, 256);

static inline int parse_ipv4(void *data, u64 nh_off, void *data_end) {
    struct iphdr *iph = data + nh_off;

    if ((void*)&iph[1] > data_end)
        return 0;
    return iph->protocol;
}

static inline int parse_ipv6(void *data, u64 nh_off, void *data_end) {
    struct ipv6hdr *ip6h = data + nh_off;

    if ((void*)&ip6h[1] > data_end)
        return 0;
    return ip6h->nexthdr;
}

// SEC("xdp")
int xdp_prog1(struct xdp_md *ctx) {
    int rc = DEFAULT_ACTION;

    void* data_end = (void*)(unsigned long) ctx->data_end;
    void* data = (void*)(unsigned long) ctx->data;

    // bpf_trace_printk("prog running");

    // Boundary check: check if packet is larger than a full ethernet + ip header
    if (data + sizeof(struct ethhdr) + sizeof(struct iphdr) > data_end)
    {
        return DEFAULT_ACTION;
    }

    struct ethhdr *eth = data;

    // Ignore packet if ethernet protocol is not IP-based
    if (eth->h_proto != bpf_htons(ETH_P_IP))
    {
        return XDP_PASS;
    }

    struct iphdr *ip = data + sizeof(*eth);

    // We currently only work on UDP traffic
    if (ip->protocol != IPPROTO_UDP) {
        return rc;
    }

    struct udphdr *udp;
    if (data + sizeof(*eth) + sizeof(*ip) + sizeof(*udp) > data_end)
    {
        return XDP_DROP;
    }
    udp = data + sizeof(*eth) + sizeof(*ip);

    u16 *dport = &udp->dest;


    bpf_trace_printk("Dropping UDP packet to %d", *dport);

    return XDP_DROP;


    return rc;
}