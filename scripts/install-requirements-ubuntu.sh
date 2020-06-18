#!/bin/bash
# Install the system requirements on Ubuntu.
#
# author: andreasl

sudo apt update
sudo apt upgrade
sudo apt install \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    libx11-dev \
    libxft-dev \
    libxdo-dev \
    libxml2 \
    xclip
