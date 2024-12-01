#!/bin/bash

# Fail on error.
set -e

# Install pre-requisites.
sudo apt install -y meson python3 python3-pip pkg-config libnuma-dev
pip3 install pyelftools

# Download sources.
wget https://fast.dpdk.org/rel/dpdk-23.11.1.tar.xz
tar -xvf dpdk-23.11.1.tar.xz
cd dpdk-stable-23.11.1
mkdir -p build

# Build and install.
meson --prefix=$HOME build
ninja -C build
ninja -C build install

# Set hugepages.
echo 8192 | sudo tee /sys/devices/system/node/node*/hugepages/hugepages-2048kB/nr_hugepages
sudo mkdir -p /mnt/huge || true
sudo mount -t hugetlbfs -opagesize=2M nodev /mnt/huge