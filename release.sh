#!/usr/bin/env bash
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MIT

CUR_DIR=$(dirname $(readlink -f "$0"))
VERSION=""

usage() {
printf "release version tool

usage:
    bash release.sh [-h] [-v VERSION]

optional arguments:
    -v VERSION     version identifier, such as: 0.5.3
    -h             show the help message and exit

examples:
    bash release.sh -v 0.5.4
\n"
}

log_error() {
    echo "ERROR: $1"
}

parse_cmdline() {
    # parse cmd line arguments
    while getopts ":v:h" opt; do
        case "$opt" in
            v)
                VERSION="$OPTARG"
            ;;
            h)
                usage; exit 0
            ;;
            ?)
                log_error "please check the params."; usage; return 1
            ;;
        esac
    done
    if [ -z "$VERSION" ]; then
        log_error "invalid VERSION argument"; return 2
    fi
    return 0
}

# release_version VERSION_NUMBER
release_version() {
    # check version fromat
    local ver=${1// /}
    if [ -z "$ver" ]; then
        log_error "version \"$1\" is empty"; return 1
    fi
    local major=$(echo $ver | cut -d . -f 1)
    local minor=$(echo $ver | cut -d . -f 2)
    local patch=$(echo $ver | cut -d . -f 3)
    if [[ -z "$major" || -z "$minor" || -z "$patch" ]]; then
        log_error "\"$ver\" is invalid format, valid such as x.y.z"; return 1
    fi
    ver="$major.$minor.$patch"
    # check version size
    local curver=$(grep -E "^VERSION\s+=\s+[0-9.]{5,}" $CUR_DIR/src/QxRibbon.pri | awk '{print $NF}')
    if [ -z "$curver" ]; then
        log_error "get current version failed"; return 1
    fi
    if [[ "$ver" < "$curver" || "$ver" == "$curver" ]]; then
        log_error "\"$ver\" is not larger than current version \"$curver\""; return 1
    fi
    # update version
    sed -i -r "s|(^VERSION\s+=).*|\1 $ver|g" $CUR_DIR/src/QxRibbon.pri
    sed -i -r "s|(^set\(QX_RIBBON_VERSION_MAJOR ).*\)|\1$major\)|g" $CUR_DIR/CMakeLists.txt
    sed -i -r "s|(^set\(QX_RIBBON_VERSION_MINOR ).*\)|\1$minor\)|g" $CUR_DIR/CMakeLists.txt
    sed -i -r "s|(^set\(QX_RIBBON_VERSION_PATCH ).*\)|\1$patch\)|g" $CUR_DIR/CMakeLists.txt
    sed -i -r "s|(^#define QX_RIBBON_VERSION_MAJOR ).*|\1$major|g" $CUR_DIR/src/QxRibbonGlobal.h
    sed -i -r "s|(^#define QX_RIBBON_VERSION_MINOR ).*|\1$minor|g" $CUR_DIR/src/QxRibbonGlobal.h
    sed -i -r "s|(^#define QX_RIBBON_VERSION_PATCH ).*|\1$patch|g" $CUR_DIR/src/QxRibbonGlobal.h

    return 0
}

main() {
    local ret=0

    parse_cmdline "$@"; ret=$?
    if [ $ret -ne 0 ]; then
        echo "please try -h for more information"; return $ret
    fi
    release_version "$VERSION"; ret=$?
    if [ $ret -ne 0 ]; then
        echo "release version failed"; return $ret
    fi

    return $ret
}

main "$@"
