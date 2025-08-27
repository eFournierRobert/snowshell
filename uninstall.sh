#!/bin/bash

cd build && cmake --build . --target uninstall
sed -i.bak '\#^/usr/local/bin/snowshell$#d' /etc/shells # Creates a backup of the file just in case