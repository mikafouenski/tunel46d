#!/bin/bash

for wm in $(ls -d VM*); do
    cd $wm
    vagrant up
    cd ..
done
