#!/bin/bash
set +ex

if ! type "brew" 1>/dev/null; then
    echo "go get Homebrew first:"
    echo " -> https://brew.sh/"
    exit 1
fi

INSTALL_OPT=$1

function maybe_brew_install() {
    type $1 1>/dev/null || brew install $1
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
    maybe_brew_install --cask docker
    echo installed Docker.
}

function install_ui_deps() {
    echo ">>>>>> Installing UI deps"
    maybe_brew_install node
    maybe_brew_install pnpm
    cd ./ui
    pnpm i
    cd ..
    echo installed UI deps.
}

ask_and_install_deps Python install_python_deps
ask_and_install_deps Docker install_docker_deps
ask_and_install_deps UI install_ui_deps
