#! /bin/bash

mkdir -p alice
mkdir -p bob
mkdir -p dealer
mkdir -p recipient

cd alice
../target/safrnffnet --orgid 000000000000000000000000000A11CE --port 8000 \
  --peers ../peers.json \
  --lookups ../server/src/test/data/ \
  --study ../server/src/test/data/study2.json \
  --data ../server/src/test/data/alice2.csv \
  --query ../server/src/test/data/${1} &> alice.log &
cd ..

cd bob
../target/safrnffnet --orgid 00000000000000000000000000000B0B --port 8001 \
  --peers ../peers.json \
  --lookups ../server/src/test/data/ \
  --study ../server/src/test/data/study2.json \
  --data ../server/src/test/data/bob2.csv \
  --query ../server/src/test/data/${1} &> bob.log &
cd ..

cd dealer
../target/safrnffnet --orgid 0000000000000000000000000000DEA1 --port 8002 \
  --peers ../peers.json \
  --study ../server/src/test/data/study2.json \
  --query ../server/src/test/data/${1} &> dealer.log &
cd ..

cd recipient
../target/safrnffnet --orgid 00000000000000000000000000FFFFFF --port 8003 \
  --peers ../peers.json \
  --study ../server/src/test/data/study2.json \
  --query ../server/src/test/data/${1} &> recipient.log &
cd ..

wait $( jobs -p )

cd alice
python3 ../lib/fortissimo-dir/src/main/python/analyze.py alice.log
python3 ../lib/fortissimo-dir/src/main/python/csvify.py alice.log
# gprof ../target/safrnffnet gmon.out > alice.prof

cd ../bob
python3 ../lib/fortissimo-dir/src/main/python/analyze.py bob.log
python3 ../lib/fortissimo-dir/src/main/python/csvify.py bob.log
# gprof ../target/safrnffnet gmon.out > bob.prof

cd ../dealer
python3 ../lib/fortissimo-dir/src/main/python/analyze.py dealer.log
python3 ../lib/fortissimo-dir/src/main/python/csvify.py dealer.log
# gprof ../target/safrnffnet gmon.out > dealer.prof

cd ../recipient
python3 ../lib/fortissimo-dir/src/main/python/analyze.py recipient.log
python3 ../lib/fortissimo-dir/src/main/python/csvify.py recipient.log
# gprof ../target/safrnffnet gmon.out > recipient.prof
