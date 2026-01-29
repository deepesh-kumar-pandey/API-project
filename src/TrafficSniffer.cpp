#include "../include/TrafficSniffer.h"
#include <iostream>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

TrafficSniffer::TrafficSniffer() : active(false), handle(nullptr) {}

TrafficSniffer::~TrafficSniffer() {
    stop_sniffing();
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
    char *dev = pcap_lookupdev(errbuf);
    if (dev == nullptr) {
        std::cerr << "[Sniffer] Error finding device: " << errbuf << std::endl;
        active = false;
        return;
    }

    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        std::cerr << "[Sniffer] Error opening device " << dev << ": " << errbuf << std::endl;
        active = false;
        return;
    }

    struct bpf_program fp;
    std::string filter = "tcp port " + std::to_string(port);
    if (pcap_compile(handle, &fp, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
        std::cerr << "[Sniffer] Error parsing filter: " << pcap_geterr(handle) << std::endl;
        active = false;
        return;
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        std::cerr << "[Sniffer] Error setting filter: " << pcap_geterr(handle) << std::endl;
        active = false;
        return;
    }

    std::cout << "[Sniffer] Monitoring traffic on " << dev << " port " << port << "..." << std::endl;
    pcap_loop(handle, 0, packet_handler, reinterpret_cast<u_char*>(this));
}

void TrafficSniffer::packet_handler(u_char* user, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    TrafficSniffer* sniffer = reinterpret_cast<TrafficSniffer*>(user);
    
    // Ethernet header is 14 bytes
    struct ip* ip_header = (struct ip*)(packet + 14);
    char src_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);

    if (sniffer->traffic_callback) {
        sniffer->traffic_callback(std::string(src_ip));
    }
}
