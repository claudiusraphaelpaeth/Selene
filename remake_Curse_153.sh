#!/bin/bash

# this version compile against Lua 5.3.4

# USE_MQTT - use Paho's MQTT layer
# USE_CURSES - Build Curses plugin
# DEBUG - Add debuging messages

if which ncursesw6-config; then
        echo "Found ncurses v6"
        NCURSES=ncursesw6-config
elif which ncursesw5-config; then
        echo "Found ncurses v5"
        NCURSES=ncursesw5-config
else
        echo "No ncurses found"
        exit 20
fi

# PLUGIN_DIR=/usr/local/lib/Selene
PLUGIN_DIR=$( pwd )

LUA_DIR=/home/laurent/Projets/lua-5.3.4/install

cd src


echo
echo "Main source"
echo "-----------"
LFMakeMaker -v +f=Makefile --opts="-isystem $LUA_DIR/include -L$LUA_DIR/lib -Wall -DUSE_CURSES `$NCURSES --cflags` `$NCURSES --libs` -DUSE_MQTT -DPLUGIN_DIR='\"$PLUGIN_DIR\"' -L$PLUGIN_DIR -lSelene -DxDEBUG -lpaho-mqtt3c -llua -lm -ldl -Wl,--export-dynamic -lpthread" *.c -t=../Selene > Makefile

cd SeleneLibrary
echo
echo "Selene Library"
echo "--------------"
LFMakeMaker -v +f=Makefile --opts="-isystem $LUA_DIR/include -Wall -fPIC -DUSE_MQTT" *.c -so=../../libSelene.so > Makefile

cd ../SelPlugins/Curses/
echo
echo "Curses plugin"
echo "-------------"
LFMakeMaker -v +f=Makefile -cc='gcc -Wall -DUSE_CURSES `'$NCURSES' --cflags` -fPIC -std=c99 ' *.c -so=../../../SelCurses.so > Makefile

/* Even desactivated, we need to compile an empty library to ensure
 * there is no zombies from a previous compilation
 */
cd ../DirectFB
echo
echo "DirectFB source"
echo "-----------"
LFMakeMaker -v +f=Makefile -cc='gcc -Wall -fPIC' *.c -so=../../../SelDirectFB.so > Makefile

echo
echo
echo "Don't forget"
echo export LD_LIBRARY_PATH=$PLUGIN_DIR:$LD_LIBRARY_PATH

