#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toml.h"

#define MAX_CMD_LEN 256

void run_cmd(const char* cmd) {
    printf("Running: %s\n", cmd);
    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "Command failed: %s\n", cmd);
        exit(1);
    }
}

int main() {
    FILE* fp = fopen("config.toml", "r");
    if (!fp) {
        perror("Error opening config.toml");
        return 1;
    }

    char errbuf[200];
    toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);
    if (!conf) {
        fprintf(stderr, "TOML parse error: %s\n", errbuf);
        return 1;
    }

    // Interface
    toml_table_t* iface = toml_table_in(conf, "interface");
    const char* name = toml_raw_in(iface, "name");
    const char* ip = toml_raw_in(iface, "ip");

    // Route
    toml_table_t* route = toml_table_in(conf, "route");
    const char* destination = toml_raw_in(route, "destination");
    const char* gateway = toml_raw_in(route, "gateway");

    // DNS
    toml_table_t* dns = toml_table_in(conf, "dns");
    toml_array_t* servers = toml_array_in(dns, "servers");

    // Strip quotes
    char iface_name[64], iface_ip[64],route_to_destination[64], gw[64];

    // removes quotations from the string "enp0s3" 
    sscanf(name, "\"%[^\"]\"", iface_name);

    // removes quotations from the string "192.168.31.2/24"
    sscanf(ip, "\"%[^\"]\"", iface_ip);
    
    //removes quotations from the string "0.0.0.0/0"
    sscanf(destination, "\"%[^\"]\"", route_to_destination);
    
    // removes quotations from the string "192.168.31.124"
    sscanf(gateway, "\"%[^\"]\"", gw);

    // Configure interface
    char cmd[MAX_CMD_LEN];
    snprintf(cmd, MAX_CMD_LEN, "ip addr flush dev %s", iface_name);
    run_cmd(cmd);

    snprintf(cmd, MAX_CMD_LEN, "ip addr add %s dev %s", iface_ip, iface_name);
    run_cmd(cmd);

    snprintf(cmd, MAX_CMD_LEN, "ip link set %s up", iface_name);
    run_cmd(cmd);

    // Default route which is gateway in our case 
    run_cmd("ip route flush default");
    snprintf(cmd, MAX_CMD_LEN, "ip route add %s via %s dev %s",route_to_destination, gw, iface_name);
    run_cmd(cmd);

    // DNS
    FILE* dnsf = fopen("/etc/resolv.conf", "w");
    if (!dnsf) {
        perror("Failed to open /etc/resolv.conf");
        return 1;
    }

    for (int i = 0; i < toml_array_nelem(servers); ++i) {
        const char* dns_raw = toml_raw_at(servers, i);
        char dns_ip[64];
        sscanf(dns_raw, "\"%[^\"]\"", dns_ip);
        fprintf(dnsf, "nameserver %s\n", dns_ip);
    }
    fclose(dnsf);

    toml_free(conf);
    printf("Configuration applied.\n");
    return 0;
}

