#include <uapi/linux/bpf.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>

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

int xdp_prog1(struct CTXTYPE *ctx) {
    int rc = RETURNCODE; // let pass XDP_PASS or redirect to tx via XDP_TX
    void* data_end = (void*)(long)ctx->data_end;
    void* data = (void*)(long)ctx->data;
    bpf_trace_printk("prog running");
    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end) {
        return rc;
    }

    if (eth->h_proto != htons(ETH_P_IP)) {
        return rc;
    }

    struct iphdr *ip = (void *)(eth + 1);
    if ((void *)(ip + 1) > data_end) {
        return rc;
    }

    if (ip->protocol != IPPROTO_TCP) {
        return rc;
    }

    struct tcphdr *tcp = (void *)ip + (ip->ihl << 2);
    if ((void *)(tcp + 1) > data_end) {
        return rc;
    }
    if (tcp->syn) {
        bpf_trace_printk("syn packet found");
    }
    // drop packets

    long *value;
    uint16_t h_proto;
    uint64_t nh_off = 0;
    uint32_t index;

    nh_off = sizeof(*eth);

    if (data + nh_off  > data_end)
        return rc;

    // h_proto = eth->h_proto;

    // if (h_proto == htons(ETH_P_IP))
    //     index = parse_ipv4(data, nh_off, data_end);
    // else if (h_proto == htons(ETH_P_IPV6))
    //    index = parse_ipv6(data, nh_off, data_end);
    // else
    //     index = 0;

    // value = dropcnt.lookup(&index);
    // if (value)
    //     __sync_fetch_and_add(value, 1);

    return rc;
}