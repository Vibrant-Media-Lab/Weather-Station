# Weather Station Onboard
## Design Overview

##### Author: Josh Arabia

### Setup

### Main Loop

### Sensors and Peripherals

### Local Storage

### Ethernet Connectivity

#### Configuration

The sketch contains the following data fields near the top of the file, in the configuration section. These must be set manually for the device:

```c
char mac_add[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //MAC Address
char ip_add[] = {0, 0, 0, 0}; //Local IP Address
```

It also contains a number of flags and configurations which determine the behavior of the system at runtime, which should also be set manually:

```c
//Whether to connect to the internet. Disables all internet capabilities if false - Default: true
bool using_ethernet = true;	
// Whether to connect via DHCP and request an IP address. Don't use if you have a static IP - Default: false
bool using_dhcp = false;
//How frequently to renew the DHCP lease, in milliseconds - Default: 1 day
unsigned long dhcp_refesh_rate = 86400000;
//How may connection attempts to make via the Ethernet port. If this number is exceeded, the module will cease internet communications
char reconnect_attempts = 8;
```