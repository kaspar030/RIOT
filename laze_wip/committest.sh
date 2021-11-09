TEST=$1
set -e
git add laze.yml tests-${TEST}/laze.yml
git ci laze.yml tests-${TEST}/laze.yml -m "tests/unittests/tests-${TEST}: add laze buildfile"
