#!/bin/bash

#sudo /vagrant/tunalloc tun0 &
sudo ip a add 172.16.2.1/28 dev tun0
sudo ip route add 172.16.2.176/28 via 172.16.2.1
sudo ip route add 172.16.2.144/28 via 172.16.2.1
