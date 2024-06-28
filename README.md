# ICFPC-2024

## Team: Snakes, Monkeys and Two Smoking Lambdas

![splash](repo/splash.png)

## Overview

Core solvers are implemented in Python.

Assistive tech include:
 - UI and visualizer via React, Vite and Mantine
 - Helper server via Flask

## Setup

To get up and running you need Python ^3.10, Poetry and Docker.

On a Mac, simply run:

```zsh
git clone https://github.com/pankdm/icfpc-2024
cd ./icfpc-2024
./install_macos.sh

Install Python deps? [y,n]y
>>>>>> Installing Python deps
installed Python deps.

Install Docker deps? [y,n]y
>>>>>> Installing Docker
Docker already installed.
````

## Tools

To start the server and UI, use

```
make start
```

To execute one-off scripts, use preloaded Python repl via

```
make repl

Welcome to ICFPC-2023!
Team: Snakes, Monkeys and Two Smoking Lambdas

Modules:
  - ICFPC
  - server
  - scripts
  - slack
>>>
```


### Problems


To download problems:

```
python3 language/downloader.py lambdaman 21
python3 language/downloader.py spaceship 25
```
