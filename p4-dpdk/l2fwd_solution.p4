#include <core.p4>
#include <psa.p4>

const bit<16> ETH_TYPE_IPV4 = 0x0800;
const bit<16> ETH_TYPE_ARP = 0x0806;
const bit<8> PROTO_ICMP = 1;
const bit<8> PROTO_TCP = 6;
const bit<8> PROTO_UDP = 17;

const bit<16> ARP_OPCODE_REPLY = 2;

typedef bit<48> mac_addr_t;
typedef bit<32> ipv4_addr_t;

header ethernet_t {
    mac_addr_t dst_addr;
    mac_addr_t src_addr;
    bit<16>    ether_type;
}

header ipv4_t {
    bit<4>      version;
    bit<4>      ihl;
    bit<6>      dscp;
    bit<2>      ecn;
    bit<16>     total_len;
    bit<16>     identification;
    bit<3>      flags;
    bit<13>     frag_offset;
    bit<8>      ttl;
    bit<8>      protocol;
    bit<16>     checksum;
    ipv4_addr_t src_addr;
    ipv4_addr_t dst_addr;
}

header tcp_t {
    bit<16> sport;
    bit<16> dport;
    bit<32> seq_no;
    bit<32> ack_no;
    bit<4>  data_offset;
    bit<3>  res;
    bit<3>  ecn;
    bit<6>  ctrl;
    bit<16> window;
    bit<16> checksum;
    bit<16> urgent_ptr;
}

header udp_t {
    bit<16> sport;
    bit<16> dport;
    bit<16> len;
    bit<16> checksum;
}

header arp_t {
    bit<16> hw_type;
    bit<16> proto_type;
    bit<8>  hw_addr_len;
    bit<8>  proto_addr_len;
    bit<16> opcode;
}

header arp_ipv4_t {
    mac_addr_t  src_hw_addr;
    ipv4_addr_t src_ipv4_addr;
    mac_addr_t  dst_hw_addr;
    ipv4_addr_t dst_ipv4_addr;
}

struct metadata {
    bit<16> l4_sport;
    bit<16> l4_dport;
}

struct headers {
    ethernet_t ethernet;
    // ipv4_t     ipv4;
    // tcp_t      tcp;
    // udp_t      udp;
    // arp_t      arp;
    // arp_ipv4_t arp_ipv4;
}

struct empty_t {}

parser DemoIngressParser(packet_in packet, out headers hdr, inout metadata meta, in psa_ingress_parser_input_metadata_t istd, in empty_t resubmit_meta, in empty_t recirculate_meta) {
    state start {
        transition parse_ethernet;
    }

    state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition accept;
    }
}


control DemoIngress(inout headers hdr, inout metadata meta, in psa_ingress_input_metadata_t istd, inout psa_ingress_output_metadata_t ostd) { 
  bit<48> tmp;
  apply { 
    tmp = hdr.ethernet.src_addr;
    hdr.ethernet.src_addr = hdr.ethernet.dst_addr;
    hdr.ethernet.dst_addr = tmp;
    ostd.egress_port = istd.ingress_port;
  }
}

control DemoIngressDeparser(packet_out packet, out empty_t clone_i2e_meta, out empty_t resubmit_meta, out empty_t normal_meta, inout headers hdr, in metadata meta, in psa_ingress_output_metadata_t istd) {
  apply { 
    packet.emit(hdr.ethernet);
  }
}

parser DemoEgressParser(packet_in buffer, out headers hdr, inout metadata meta, in psa_egress_parser_input_metadata_t istd, in empty_t normal_meta, in empty_t clone_i2e_meta, in empty_t clone_e2e_meta) {

    state start {
        transition accept;
    }
}

control DemoEgress(inout headers hdr,
                   inout metadata meta,
                   in    psa_egress_input_metadata_t  istd,
                   inout psa_egress_output_metadata_t ostd) {

    apply {
    }
}

control DemoEgressDeparser(packet_out packet,
out empty_t clone_e2e_meta,
out empty_t recirculate_meta,
inout headers hdr,
in metadata meta,
in psa_egress_output_metadata_t istd,
in psa_egress_deparser_input_metadata_t edstd) {

    apply {
    }
}

IngressPipeline(DemoIngressParser(), DemoIngress(), DemoIngressDeparser()) ip;
EgressPipeline(DemoEgressParser(), DemoEgress(), DemoEgressDeparser()) ep;

PSA_Switch(ip, PacketReplicationEngine(), ep, BufferingQueueingEngine()) main;
