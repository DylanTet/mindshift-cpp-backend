#!/bin/bash

if [[ ! -d "build" ]]; then
  echo "You need to build the project before you run."
  exit 0
fi

./build/bin/mindshift-backend-project
