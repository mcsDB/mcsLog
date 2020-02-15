#!/bin/bash -x
#script to create and mount a pmemdir
#requires size as input
USER=`whoami`
echo $USER
sudo umount /mnt/pmemdir
sudo mkdir /mnt/pmemdir
sudo mkfs.ext4 /dev/pmem0
sudo mount -o dax /dev/pmem0 /mnt/pmemdir
sudo chown -R $USER /mnt/pmemdir
