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


### PYTHON SHUTDOWN SERVICE

- [ ] CREATE SHUTDOWN PYTHON PROGRAM
  - ```nano ~/pyshutdown.py```
  - add:
  ```
  ```

- [ ] CREATE FILE PYSHUTDOWN.SERVICE
  - ```sudo nano /lib/systemd/system/pyshutdown.service```
  - add:
  ```
  [Unit]
  Description=Python Shutdown Service

  [Service]
  ExecStart=/home/pi/pyshutdown.py
  StandardOutput=null

  [Install]
  WantedBy=multi-user.target
  Alias=pyshutdown.service
  ```


### APACHE2

#### APACHE2 is a web server. You can serve web pages to those who connect to your access point.

- [ ] STOP DNSMASQ FOR NOW
  - ```sudo systemctl stop dnsmasq.service```

- [ ] INSTALL APACHE2
  - ```sudo apt-get install apache2```
  - ```Do you want to continue? [Y/n] ```
- [ ] MAKE CHANGES TO THE DEFAULT WEB PAGE
- [ ] UPDATE rc.d
  - ```sudo update-rc.d apache2 defaults```


### HOSTAPD

#### HOSTAPD will turn your raspberry pi into a wifi access point

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
  ssid=↟↟↟↟↟↟↟↟
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

#### DNSMASQ will enable your access point to hand out IP addresses so others can connect

- [ ] INSTALL DNSMASQ
  - ```sudo apt-get install dnsmasq```
  - ```Do you want to continue? [Y/n] Y```
 - [ ] CONFIGURE DNSMASQ
  - ```sudo cp /etc/dnsmasq.conf /etc/dnsmasq.old```
  - ```sudo nano /etc/dnsmasq.conf```
  - add at end:
  ```
  log-facility=/var/log/dnsmasq.log
  address=/#/10.0.0.1
  interface=wlan0
  dhcp-range=10.0.0.10,10.0.0.250,12h
  no-resolv
  log-queries
  ```
- [ ] UPDATE rc.d
  - sudo ```sudo update-rc.d dnsmasq defaults```
- [ ] REBOOT
  - ```sudo reboot```
- [ ] CHECK TO SEE IF DNSMASQ WORKS BY CONNECTING TO YOUR ACCESS POINT
  - ```ifconfig```
  - *if your IP address starts with 10.0.0.#, you're doing great!*
