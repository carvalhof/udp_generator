#!/bin/bash

# Fail on error.
set -e

# Load the 'vfio' module with no-IOMMU.
sudo modprobe vfio enable_unsafe_noiommu_mode=1

# Bind interfaces.
sudo ifconfig ens1f0 down
sudo ifconfig ens1f1 down
sudo $HOME/bin/dpdk-devbind.py -b vfio-pci 5e:00.0
sudo $HOME/bin/dpdk-devbind.py -b vfio-pci 5e:00.1

# Set hugepages.
echo 8192 | sudo tee /sys/devices/system/node/node*/hugepages/hugepages-2048kB/nr_hugepages
sudo mkdir -p /mnt/huge || true
sudo mount -t hugetlbfs -opagesize=2M nodev /mnt/huge