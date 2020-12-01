set -euo pipefail
case $# in
  1)
  ;;
  *)
    echo "Usage: bash run_experiment.bash [folder]"
    exit 1
  ;;
esac
expt=$1

cleanup() {
  set +e
  trap - SIGTERM
  kill -- -$$
}
trap cleanup EXIT SIGINT SIGTERM

run_party() {
  local -r prog=$1
  local -r conf=$2
  if [ -f "$conf" ]; then
    $prog --config "$conf" < <(sleep infinity) &
  fi
}

run_party safrn_front_server "$expt/A-I.cfg"
run_party safrn_front_server "$expt/A-L.cfg"
run_party safrn_back_server "$expt/I.cfg"
run_party safrn_back_server "$expt/L.cfg"
run_party safrn_back_server "$expt/G1-I.cfg"
run_party safrn_back_server "$expt/G1-L.cfg"
run_party safrn_back_server "$expt/G2-I.cfg"
run_party safrn_back_server "$expt/G2-L.cfg"
run_party safrn_back_server "$expt/G3-I.cfg"
run_party safrn_back_server "$expt/G3-L.cfg"
run_party safrn_back_server "$expt/G4-I.cfg"
run_party safrn_back_server "$expt/G4-L.cfg"

sleep infinity
