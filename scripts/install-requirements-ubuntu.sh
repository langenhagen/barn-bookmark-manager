#!/bin/bash
# Install the system requirements on Ubuntu.
#
# author: andreasl

sudo apt update; sudo apt upgrade
sudo apt install build-essential cmake libx11-dev libxft-dev libxdo-dev xclip
