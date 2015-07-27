#!/bin/bash

killall Server
killall Client
./Server > ./Server_output.log &
./Client 1 &
./Client 2 &
./Client 3 &
./Client 4 &

