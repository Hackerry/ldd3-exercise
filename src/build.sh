#!/bin/bash

function build() {
    echo "Building $1..."
    cd $1
    make
    cd ..
}

function install() {
    echo "Installing $1 .ko modules to $2..."
    cd $1
    cp *.ko $2
}

# Check if an argument is provided
if [ $# -lt 1 ]; then
    echo "Usage: $0 build_dirname"
    exit 1
fi

# Call the function based on the first argument
build_dirname=$1
install_dirname=$LDD_ROOT/initramfs/tmp/
build $build_dirname
install $build_dirname $install_dirname