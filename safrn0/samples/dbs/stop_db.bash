set -euo pipefail
case $# in
  1)
  ;;
  *)
    echo "Use one of the following options [i,l,g1,g2,g3,g4,all,allbutg1]"
    exit 1
  ;;
esac
role=$1
case $role in
  i)
    echo "Stopping Income DB"
    docker stop i-safrn-db
  ;;
  l)
    echo "Stopping Loan DB"
    docker stop l-safrn-db
  ;;
  g1)
    echo "Stopping Group 1 DB"
    docker stop g1-safrn-db
  ;;
  g2)
    echo "Stopping Group 2 DB"
    docker stop g2-safrn-db
  ;;
  g3)
    echo "Stopping Group 3 DB"
    docker stop g3-safrn-db
  ;;
  g4)
    echo "Stopping Group 4 DB"
    docker stop g4-safrn-db    
  ;;
  all)
    echo "Stopping All-in-one DB"
    docker stop all-safrn-db
  ;;
  allbutg1)
    echo "Stopping All-but-Group1 DB"
    docker stop allbutg1-safrn-db
  ;;
  *)
    echo "Use one of the following options [i,l,g1,g2,g3,g4,all,allbutg1]"
    exit 1
  ;;
esac
