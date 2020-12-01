#!/bin/bash

if [ "$1" == "" ]; then
	echo "Please specify the key path"
else
	eval `ssh-agent -s`
	ssh-add $1
fi
