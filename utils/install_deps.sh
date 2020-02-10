# Requires latest g++ version
# https://tuxamito.com/wiki/index.php/Installing_newer_GCC_versions_in_Ubuntu
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc g++ gcc-5 g++-5 gcc-6 g++-6 gcc-7 g++-7
sudo update-alternatives --remove-all gcc
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 90 --slave /usr/bin/g++ g++ /usr/bin/g++-5
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 80 --slave /usr/bin/g++ g++ /usr/bin/g++-6
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 70 --slave /usr/bin/g++ g++ /usr/bin/g++-7
sudo update-alternatives --config gcc

