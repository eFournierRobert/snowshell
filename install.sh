#!/bin/bash

cd build && cmake --install . --config Release # Install everything
echo "/usr/local/bin/snowshell" >> /etc/shells # Append in shell file