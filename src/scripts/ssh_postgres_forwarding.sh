#!/bin/bash
set -e

if [ "$#" -ne 2 ]; then
	echo "Please specify parameters: username address"
	exit 1
fi

ssh -L 5432:127.0.0.1:5432 $1@$2
