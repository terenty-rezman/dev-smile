#!/bin/bash

# terminate script on error
set -e

# check if user root
if [[ $EUID != 0 ]]; then
  echo "! the script is meant to be run with 'sudo'"
  exit 
fi

module="smile"
device="$module"
real_user=$(who mom likes | awk '{print $1}')

# load module into kernel
insmod ./${module}.ko

# determine major number of the device
major=$(awk "\$2 == \"$module\" {print \$1}" /proc/devices)

# create filesystem node to represent the device
mknod /dev/${device} c $major 0

# allow everyone to rw the device 
mode="a+rw"

chmod $mode /dev/${device}
