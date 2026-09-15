#!/usr/bin/env bash
set -euo pipefail

if ! [[ $* == *--skip-rebuild* ]]
then
    make
else
    echo "--skip-rebuild used, skipping compilation"
fi

if ! [[ $* == *--skip-iso-rebuild* ]]
then
    bash ./genisoimg.sh
else
    echo "--skip-iso-rebuild used, running latest ISO image"
fi

bash ./run.sh
