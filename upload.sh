#!/bin/bash
pros mu --slot 1 --name "LEFT"

sed -i "s/left/left_alternate/g" ./include/parameters.h # we do a little replacing
pros mu --slot 2 --name "LEFT ALT"

sed -i "s/left_alternate/right/g" ./include/parameters.h # we do a little replacing
pros mu --slot 3 --name "RIGHT"

sed -i "s/right/right_alternate/g" ./include/parameters.h # we do a little replacing
pros mu --slot 4 --name "RIGHT ALT"

sed -i "s/right_alternate/left/g" ./include/parameters.h # we do a little replacing
