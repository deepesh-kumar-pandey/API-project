#include "../include/TrafficSniffer.h"
#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <netinet/ip.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
#endif

// Portable IP header for cross-platform packet inspection
struct portable_ip {
    unsigned char  ip_vhl;                 /* version << 4 | header length >> 2 */
    unsigned char  ip_tos;                 /* type of service */
    unsigned short ip_len;                 /* total length */
    unsigned short ip_id;                  /* identification */
    unsigned short ip_off;                 /* fragment offset field */
    #define IP_RF 0x8000            /* reserved fragment flag */
    #define IP_DF 0x4000            /* dont fragment flag */
    #define IP_MF 0x2000            /* more fragments flag */
    #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
    unsigned char  ip_ttl;                 /* time to live */
    unsigned char  ip_p;                   /* protocol */
    unsigned short ip_sum;                 /* checksum */
    struct in_addr ip_src, ip_dst;         /* source and dest address */
};

TrafficSniffer::TrafficSniffer() : active(false), handle(nullptr) {
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

TrafficSniffer::~TrafficSniffer() {
    stop_sniffing();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool TrafficSniffer::start_sniffing(int port, TrafficCallback callback) {
    if (active) return false;
    
    traffic_callback = callback;
    active = true;
    capture_thread = std::thread(&TrafficSniffer::run_capture, this, port);
    return true;
}

void TrafficSniffer::stop_sniffing() {
    active = false;
    if (handle) {
        pcap_breakloop(handle);
    }
    if (capture_thread.joinable()) {
        capture_thread.join();
    }
    if (handle) {
        pcap_close(handle);
        handle = nullptr;
    }
}

void TrafficSniffer::run_capture(int port) {
    pcap_if_t *alldevs;
    pcap_if_t *d;
    char *dev = nullptr;

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "[Sniffer] Error in pcap_findalldevs: " << errbuf << std::endl;
        active = false;
        return;
    }

    // Pick the first device that isn't loopback and is up
    for (d = alldevs; d; d = d->next) {
        if (!(d->flags & PCAP_IF_LOOPBACK)) {
            dev = d->name;
            break;
        }
    }

    if (dev == nullptr) {
        std::cerr << "[Sniffer] No suitable interface found." << std::endl;
        pcap_freealldevs(alldevs);
        active = false;
        return;
    }

    handle = pcap_open_live(dev, BUFSIZ, 1, 100, errbuf); 
    if (handle == nullptr) {
        std::cerr << "[Sniffer] Error opening device " << dev << ": " << errbuf << std::endl;
        pcap_freealldevs(alldevs);
        active = false;
        return;
    }

    struct bpf_program fp;
    std::string filter = "tcp port " + std::to_string(port);
    if (pcap_compile(handle, &fp, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
        std::cerr << "[Sniffer] Error parsing filter: " << pcap_geterr(handle) << std::endl;
        pcap_freealldevs(alldevs);
        active = false;
        return;
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        std::cerr << "[Sniffer] Error setting filter: " << pcap_geterr(handle) << std::endl;
        pcap_freealldevs(alldevs);
        active = false;
        return;
    }

    std::cout << "[Sniffer] Monitoring traffic on " << dev << " port " << port << "..." << std::endl;
    pcap_freealldevs(alldevs); 
    pcap_loop(handle, 0, packet_handler, reinterpret_cast<u_char*>(this));
}

void TrafficSniffer::packet_handler(u_char* user, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    TrafficSniffer* sniffer = reinterpret_cast<TrafficSniffer*>(user);
    
    // Ethernet header is 14 bytes. Using portable IP header for source extraction.
    struct portable_ip* ip_header = (struct portable_ip*)(packet + 14);
    char src_ip[INET_ADDRSTRLEN];
    
    // Convert source IP to string (cross-platform)
    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, sizeof(src_ip));

    if (sniffer->traffic_callback) {
        sniffer->traffic_callback(std::string(src_ip));
    }
}
