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


## Usage


### To download problems:

```
python language/downloader.py lambdaman 1 21
python language/downloader.py spaceship 1 25
```

### To upload solutions:

```
python language/uploader.py solutions/spaceship/best/ 1 25
```


### REPL to send commands to their server:

```
python language/repl.py repl
```

### Send a single command:

```
python language/repl.py send 'get spaceship'
```

### How to submit 3d:

1. For problem `N`, copy the cells from spreadsheet into `N-something.input` file (for example `1.input`)
2. Run

```
PYTHONPATH="./language/" python solvers/3d/clean2.py solutions/3d/organized/1.input
```

### How to java
cd solvers/3d
brew install maven openjdk
... make sure java is on your path and resolves (use jenv or whatever).
mvn compile exec:java -Dexec.mainClass="icfpc3d.Main" -Dexec.args="../../solutions/3d/organized/1.output 5 0"

### How to Kotlin
`cd ide`
From CLI run `./gradlew :cli:run --args="<input> <a> <b>"`

Relative path is resolved to `cli`, so it should be something like `../../solutions/3d/organized/2.output`.


### Monitor leaderboard

Single run:

```
PYTHONPATH="./language/" python scripts/check_leaderboard.py
```

Monitoring script:

```
./scripts/monitor_leaderboard.sh
```

