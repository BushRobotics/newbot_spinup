#!/bin/bash
pros mu --slot 1 --name "LEFT"

sed -i "s/left/right/g" ./include/parameters.h # we do a little replacing
pros mu --slot 2 --name "RIGHT"

sed -i "s/right/left/g" ./include/parameters.h # we do a little replacing
