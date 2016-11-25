#!/bin/bash

sudo /vagrant/tunalloc tun0 &
sudo ip a add 172.16.2.1/28 dev tun0
