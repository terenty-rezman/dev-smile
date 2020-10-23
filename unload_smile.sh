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

rmmod $module

# remove fs device node 
rm -f /dev/${device}

echo done

# print last entry about module from systemd journal
journalctl -xe | grep $module | tail -1
