#!/usr/bin/env bash
# П. 3 і 4.4: енерговитрати через Intel RAPL (powercap sysfs) або perf (power/energy-pkg/).
# Використання: ./power.sh ./matmul_O0 naive 1024
# Ідея "творчості": міряємо (а) систему у спокої (idle) і (б) під навантаженням,
# тоді енергія програми = E_load - P_idle * t.
set -euo pipefail
BIN=$1; MODE=$2; N=$3
IDLE_SECS=${IDLE_SECS:-10}

RAPL=/sys/class/powercap/intel-rapl:0/energy_uj   # package-0, мікроджоулі
if [ ! -r "$RAPL" ]; then
  echo "RAPL недоступний напряму; спробуйте: sudo chmod -R a+r /sys/class/powercap/intel-rapl*"
  echo "або використайте: sudo perf stat -a -e power/energy-pkg/ -- $BIN $MODE $N"
  echo "На AMD/ноутбуці без RAPL: powertop, або USB-ватметр / вбудований лічильник батареї (upower)."
  exit 1
fi

read_e() { cat "$RAPL"; }

# 1) idle: система без нашої програми
e0=$(read_e); sleep "$IDLE_SECS"; e1=$(read_e)
p_idle=$(echo "scale=3; ($e1-$e0)/1000000/$IDLE_SECS" | bc)
echo "Idle: ${p_idle} Вт (пакет CPU)"

# 2) під навантаженням
e0=$(read_e); t0=$(date +%s.%N)
"$BIN" "$MODE" "$N"
t1=$(date +%s.%N); e1=$(read_e)
dur=$(echo "$t1-$t0" | bc)
e_sys=$(echo "scale=3; ($e1-$e0)/1000000" | bc)
echo "3.1 Система за час роботи: ${e_sys} Дж за ${dur} с (середня ${p_idle:+$(echo "scale=2; $e_sys/$dur" | bc)} Вт)"

# 3) вклад лише програми
e_prog=$(echo "scale=3; $e_sys - $p_idle*$dur" | bc)
echo "3.2 Лише програма (за вирахуванням idle): ${e_prog} Дж"
