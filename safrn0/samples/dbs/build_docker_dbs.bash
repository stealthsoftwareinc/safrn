set -euo pipefail
docker build -f all.Dockerfile -t all-safrn-db .
docker build -f allbutg1.Dockerfile -t allbutg1-safrn-db .
docker build -f g1.Dockerfile -t g1-safrn-db .
docker build -f g2.Dockerfile -t g2-safrn-db .
docker build -f g3.Dockerfile -t g3-safrn-db .
docker build -f g4.Dockerfile -t g4-safrn-db .
docker build -f i.Dockerfile -t i-safrn-db .
docker build -f l.Dockerfile -t l-safrn-db .
