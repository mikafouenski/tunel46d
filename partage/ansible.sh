#!/bin/bash

cd /vagrant

export sudo='echo "m1reseaux" | sudo -S'

sudo ansible-playbook -c local config.vm*.yml
