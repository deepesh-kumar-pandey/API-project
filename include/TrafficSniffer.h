#ifndef TRAFFIC_SNIFFER_H
#define TRAFFIC_SNIFFER_H

#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <pcap.h>

class TrafficSniffer {
public:
    using TrafficCallback = std::function<void(const std::string& source_ip)>;

    TrafficSniffer();
    ~TrafficSniffer();

    bool start_sniffing(int port, TrafficCallback callback);
    void stop_sniffing();
    bool is_active() const { return active; }

private:
    static void packet_handler(u_char* user, const struct pcap_pkthdr* pkthdr, const u_char* packet);
    void run_capture(int port);

    std::atomic<bool> active;
    std::thread capture_thread;
    TrafficCallback traffic_callback;
    pcap_t* handle;
    char errbuf[PCAP_ERRBUF_SIZE];
};

#endif
