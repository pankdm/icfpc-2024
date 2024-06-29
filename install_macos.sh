#!/bin/bash
set +ex

if ! type "brew" 1>/dev/null; then
    echo "go get Homebrew first:"
    echo " -> https://brew.sh/"
    exit 1
fi

INSTALL_OPT=$1

function maybe_brew_install() {
    type $@ 1>/dev/null || brew install $@
}

function ask_and_install_deps() {
    if [[ "${INSTALL_OPT}" == "all" ]]; then
        $2
    else
        echo
        read -n1 -p "Install $1 deps? [y,n]" doit
        echo
        case $doit in
        y|Y) $2 ;;
        n|N) ;;
        *) ;;
        esac
    fi
}

function install_python_deps() {
    echo ">>>>>> Installing Python deps"
    maybe_brew_install python3 poetry
    echo installed Python deps.
}

function install_docker() {
    echo ">>>>>> Installing Docker"
    which docker > /dev/null && echo "Docker already installed." && return
    maybe_brew_install --cask docker
    echo installed Docker.
}

ask_and_install_deps Python install_python_deps
ask_and_install_deps Docker install_docker
