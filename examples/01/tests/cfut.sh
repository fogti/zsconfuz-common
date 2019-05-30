# USAGE: . "$(dirname "$0")/cfut.sh" || { echo "cfut.sh not found"; exit 1; }

echo_n() {
  printf '%s' "$@"
}

# print_result RESULT_VAR
print_result() {
  local ret=$?
  [ $ret -eq 0 ] && echo yes || echo no
  zsconfuz-push result "$1=$([ $ret -ne 0 ] ; echo $?)"
}
