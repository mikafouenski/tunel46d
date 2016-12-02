#!/bin/bash

for wm in $(ls -d VM*); do
    cd $wm
    vagrant halt &
    cd ..
done
