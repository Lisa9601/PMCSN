#!/bin/bash

clear
read -p "Activate debug mode (y or n): " choice

if [[ "$choice" == "y" ]]; then

  gcc ./lib/demo.c -o demo -D DEBUG
  ./demo
  rm demo

elif [[ "$choice" == "n" ]]; then

  gcc ./lib/demo.c -o demo
  ./demo
  rm demo

else
  echo "Wrong command $choice"
fi