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
    echo "Starting Income DB..."
    port=13300
  ;;
  l)
    echo "Starting Loan DB..."
    port=13301
  ;;
  g1)
    echo "Starting Group 1 DB..."
    port=13302
  ;;
  g2)
    echo "Starting Group 2 DB..."
    port=13303
  ;;
  g3)
    echo "Starting Group 3 DB..."
    port=13304
  ;;
  g4)
    echo "Starting Group 4 DB..."
    port=13305
  ;;
  all)
    echo "Starting All-in-one DB..."
    port=13306
  ;;
  allbutg1)
    echo "Starting All-but-Group1 DB..."
    port=13307
  ;;
  *)
    echo "Use one of the following options [i,l,g1,g2,g3,g4,all,allbutg1]"
    exit 1
  ;;
esac

docker run --name=$role-safrn-db -d --publish=127.0.0.1:$port:3306/tcp --rm $role-safrn-db

#wait for mysql server to be running
ctrlc=0
trap ctrlc=1 SIGINT
while ! docker run -it --rm --network=host mysql mysql --host=127.0.0.1 --port=$port -uroot -proot -e exit >/dev/null; do
  sleep 1
  case $ctrlc in 1) exit 1 ;; esac
done
trap - SIGINT

echo "MySQL Server for $role-safrn-db is now running on port $port"

