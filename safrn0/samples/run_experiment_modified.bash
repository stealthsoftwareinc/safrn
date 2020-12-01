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
  local -r logger="${conf#*/}"
  echo "$conf.cfg"
  if [ -f "$conf.cfg" ]; then
    echo "Writing to $logger.log"
    $prog --config "$conf.cfg" >> "$logger.log" < <(sleep infinity) &
  fi
}

run_party safrn_front_server "$expt/A-I"
run_party safrn_front_server "$expt/A-L"
run_party safrn_back_server "$expt/I"
run_party safrn_back_server "$expt/L"
run_party safrn_back_server "$expt/G1-I"
run_party safrn_back_server "$expt/G1-L"
run_party safrn_back_server "$expt/G2-I"
run_party safrn_back_server "$expt/G2-L"
run_party safrn_back_server "$expt/G3-I"
run_party safrn_back_server "$expt/G3-L"
run_party safrn_back_server "$expt/G4-I"
run_party safrn_back_server "$expt/G4-L"

sleep infinity
