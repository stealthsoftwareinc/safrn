#! /bin/bash

# Copyright 2020 Stealth Software Technologies Inc

# gitlab script setup stuff.
set -e
. src/bash/preludes/gitlab-ci.bash

# actual packaging stuff
cd core/
make dependencies-download-only

# SST packageing
cd lib/fortissimo-dir/lib/sst-dir/
./autogen
./configure ASCIIDOCTOR='build-aux/adock asciidoctor'
make dist
cd ../
mv sst-dir/sst-*.tar.gz ./
rm -rf sst-dir/
tar -xzf sst-*.tar.gz
rm sst-*.tar.gz
mv sst-*/ sst-dir/
cd ../../../

cd ../
curr_path="$( realpath . )"
cd ../

mkdir archives/
tar -czf archives/safrn.tar.gz --exclude='*/tmp.*' --exclude='*/.git' safrn/


cd "${curr_path}"
mv ../archives/ ./archives/
