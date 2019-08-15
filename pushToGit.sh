#!/usr/local/env bash

set -e;

git add ./DigitalSensor
git add ./LEDstrip
sudo git add ./MoistureSensors        
git add ./Multiplexer
git add ./Scheduler
git add ./StaticFunctions
git add ./UnixTimeHandler
git add ./pushToGit.sh

git commit -m "bashed update"

git remote rm origin

git remote add origin "https://github.com/MrSoir/d1mini_libraries.git"

git push -u origin master
