#!/bin/bash

clear
echo "--- Airport simulation ---"
read -p "Activate debug mode (y or n): " choice

if [[ "$choice" == "y" ]]; then

  gcc main.c -o main -D DEBUG
  ./main

elif [[ "$choice" == "n" ]]; then

  gcc main.c -o main
  ./main

else
  echo "Wrong command $choice"
fi
