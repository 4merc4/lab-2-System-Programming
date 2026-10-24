# Лаба: профілювання, perf, FlameGraph, енерговитрати, оптимізація

Програма: множення матриць `matmul.c`. Дві версії алгоритму (`naive` — цикл i-j-k, `opt` — i-k-j)
і дві збірки (`-O0` та `-O3 -march=native`). Це дає змогу показати і оптимізацію ключами, і зміну коду.

## Підготовка (Linux, не WSL1)
```bash
sudo apt install build-essential linux-tools-common linux-tools-generic linux-tools-$(uname -r) git bc
sudo sysctl kernel.perf_event_paranoid=-1     # щоб perf працював без sudo
make
```

## Порядок виконання

| Пункт | Команда | Що зберегти / пояснити |
|---|---|---|
| 1 FlameGraph (1.1 показати процес, 1.2 інтерпретація) | `./flamegraph.sh ./matmul_O0 naive 1024 flame_O0` | `flame_O0.svg`: ширина = частка CPU-часу, висота = глибина стеку; плато `matmul_naive` |
| 2.1 | `./stats.sh ./matmul_O0 naive 1024 O0_naive` | у `time_*.txt`: wall time, max RSS, page faults, context switches |
| 2.2 | (там же) `perf stat -d` | IPC, cache-misses, L1-dcache-load-misses, branch-misses |
| 2.3 | (там же) `perf record` + `perf report` | розподіл за функціями, `perf annotate` на гарячу функцію |
| 3.1 / 3.2 | `./power.sh ./matmul_O0 naive 1024` | Дж системи (RAPL) та Дж лише програми = E − P_idle·t |
| 4.1 | `make asm`; або вставити `matmul.c` у godbolt.org (gcc, `-O0` vs `-O3 -march=native`) | `-O0`: все через стек (`mov [rbp-..]`); `-O3`: цикли розгорнуті, регістри |
| 4.1.1 | дивитися `vmulpd/vaddpd/vfmadd231pd` з `ymm` (AVX2) чи `zmm` (AVX-512) | одна інструкція обробляє 4 (ymm) або 8 (zmm) double |
| 4.2 | `./stats.sh ./matmul_O3 opt 1024 O3_opt` | порівняти таблицю до/після |
| 4.3 | `./flamegraph.sh ./matmul_O3 opt 1024 flame_O3` | змінилась ширина/форма стовпчика, зникли зайві кадри |
| 4.4 | `./power.sh ./matmul_O3 opt 1024` | менший час → менше Дж, навіть якщо потужність вища (race-to-idle) |

Рекомендація: проганяйте кожен вимір 3–5 разів і беріть середнє; закрийте зайві програми.
Можна додати проміжні кроки: `naive` з `-O3` і `opt` з `-O0`, щоб розділити вплив ключів і зміни коду.

## Таблиця результатів (заповнити)

| Показник | naive -O0 | opt -O3 | Різниця |
|---|---|---|---|
| Час, с | | | |
| IPC | | | |
| L1 miss, % | | | |
| cache-misses | | | |
| Енергія системи, Дж | | | |
| Енергія програми, Дж | | | |

## Що має бути у відео (екран + камера + голос)
1. Збірка, запуск, побудова FlameGraph «наживо» + пояснення графіка.
2. `time -v`, `perf stat -d`, `perf record/report`: що означає кожне ключове поле.
3. Вимір енергії двома способами (система / лише програма), пояснити метод.
4. Godbolt/`make asm`: різниця асемблера, AVX-інструкції.
5. Порівняльна таблиця, новий FlameGraph, висновки щодо енергії.

## Здача
GitHub-репозиторій (цей каталог + `results/`, SVG, скріншоти) і посилання на YouTube-відео.
Здача за тиждень до дедлайну дає +2 б.
