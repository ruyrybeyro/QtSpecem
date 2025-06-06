#!/bin/bash
sudo dnf install epel-release git
sudo yum groupinstall "C Development Tools and Libraries"
sudo yum install SDL2 SDL2-devel
sudo yum install mesa-libGL-devel
sudo dnf install qt6-qtbase qt6-qtbase-devel qt6-qtdeclarative qt6-qtdeclarative-devel qt6-qtmultimedia qt6-qtmultimedia-devel
echo "Now you can run qmake"
echo "If qmake does not run, run scripts/do_qmake.sh"
