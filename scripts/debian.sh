#!/bin/bash
# Tested with Debian 13
sudo apt install git build-essential libsdl2-dev
sudo apt install qt6-base-dev qt6-base-dev-tools qt6-declarative-dev qt6-multimedia-dev
sudo apt install alsa-utils libasound2 libasound2-plugins pulseaudio
#git clone https://github.com/ruyrybeyro/QtSpecem
echo "Now you can run qmake"
echo "If qmake does not run, run scripts/do_qmake.sh"
echo to run sound, do "pulseaudio --start" first as the user running ./QtSpecem
cat << EOF
pcm.!default {
    type pulse
}

ctl.!default {
    type pulse
}
EOF

