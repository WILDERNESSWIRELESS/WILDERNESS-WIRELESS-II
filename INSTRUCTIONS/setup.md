# SETUP INSTRUCTIONS

### PREP

- [ ] FORMAT SD CARD W SD CARD FORMATTER
- [ ] WRITE RASPBIAN TO SD CARD W ETCHER

### FIRST START
- [ ] INSERT SD CARD
- [ ] CONNECT KBD, MOUSE, SCREEN
- [ ] CONNECT POWER (wait for boot to desktop)
- [ ] SETUP WIFI ON WLAN0
- [ ] START TERMINAL
- [ ] CHECK NETWORK CONFIG
  - ```ifconfig```
- [ ] WRITE DOWN IP ADDRESS for WLAN0
  - ```inet 192.168.###.###```
- [ ] RASPBERRY PI CONFIGURATION
  - ```sudo raspi config```
- [ ] CHANGE USER PASSWORD
- [ ] CHANGE HOSTNAME
  - ```TAB``` \<OK\> or \<Cancel\>
- [ ] CHANGE INTERFACING OPTIONS
  - P2 SSH ```tab``` \<YES\> or \<NO\>
- [ ] CHANGE BOOT OPTIONS
  - B1 Desktop / CLI
  - B2 Console Autologin \<ENTER\>
- [ ] CHANGE BOOT OPTIONS
  - B3 Splash Screen - ```TAB``` \<NO\>, \<ENTER\>
- [ ] QUIT CONFIG
  - ```TAB``` \<Finish\>
- [ ] SHUT DOWN
  - ```sudo shutdown now```
- [ ] REMOVE POWER
- [ ] REMOVE KBD, MOUSE, SCREEN. MAKE SURE ORIGINAL WIRELESS INTERFACE IS PLUGGED IN

### SECOND START

- [ ] APPLY POWER
- [ ] UPDATE PACKAGE LISTS
  - ```sudo apt-get update```
- [ ] COPY DHCPCD.CONF to DHCPCD.OLD
  - ```sudo cp /etc/dhcpcd.conf /etc/dhcpcd.old```
- [ ] EDIT DHCPCD.CONF (2 ways)
    - ```sudo nano /etc/dhcpcd.conf```
    - add line at end ```denyinterfaces wlan0```
-- or --
    - ```sudo echo "denyinterfaces wlan0" >> /etc/dhcpcd.conf```
    - *if you reboot at this point, ssh may not work because the wlan0 interface is now down!*
- [ ] COPY INTERFACES TO INTERFACES.OLD
  - ```sudo cp /etc/network/interfaces /etc/network/interfaces.old```
- [ ] EDIT INTERFACES
  - ```sudo nano /etc/network/interfaces```
  - add:
  ```
  auto lo
  iface lo inet loopback

  auto wlan0
  iface wlan0 inet static
  address 10.0.0.1
  netmask 255.255.255.0
  broadcast 255.0.0.0

  auto wlan1
  iface wlan1 inet dhcp

  wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf
  ```
- [ ] REBOOT
  - ```sudo reboot```
  - *after reboot wlan1 should come back with dhcp, wlan0 with a static IP*






### HOSTAPD

- [ ] INSTALL HOSTAPD
  - ```sudo apt-get install hostapd```
  - ```Do you want to continue? [Y/n] Y```
- [ ] MAKE CONFIG FILE FOR HOSTAPD
  - ```touch /etc/hostapd/hostapd.conf```
- [ ] EDIT HOSTAPD.CONF
  - ```sudo nano /etc/hostapd/hostapd.conf```
  - add:
  ```
  interface=wlan0
  driver=nl80211
  ssid=WILDERNESS-WIRELESS-00
  channel=1
  ```
- [ ] EDIT HOSTAPD IN INIT.D
  - FIND LINE ```DAEMON_CONF=```
  - CHANGE TO ```DAEMON_CONF=/etc/hostapd/hostapd.conf```
- [ ] UPDATE rc.d
  - ```sudo update-rc.d hostapd defaults```
- [ ] REBOOT
  - ``sudo reboot```
  - *after reboot, your wireless network name should be visible but not joinable*

### DNSMASQ

### APACHE2
