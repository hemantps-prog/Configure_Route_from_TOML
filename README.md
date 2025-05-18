# Configure_Route_from_TOML
Designed a tool that loads router configuration parameters from a TOML file and configures interface IP, default route, and DNS settings using Linux commands


# Compilation Steps:

gcc configure_route.c toml.c -o configure_route

sudo ./configure_route


# COMMANDS USED:

### ip addr flush dev enp0s3

Usage:This will remove all IP addresses assigned to the interface enp0s3





### ip addr add 192.168.31.2/24 dev enp0s3

Usage:adds the IP address 192.168.31.2 with a subnet mask of /24 (255.255.255.0) to the network interface enp0s3



### ip link set enp0s3 up
Usage:It activates (brings up) a network interface enp0s3



### ip route flush default
Usage:It removes the current default route(s) from the system's routing table.


### ip route add default via 192.168.31.124 dev enp0s3
Usage: adds route to destination 0.0.0.0 to the gateway addresss 192.168.31.124 from interface enp0s3


Note: dns ip are modified in /etc/resolv.conf
