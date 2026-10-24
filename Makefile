# -fno-omit-frame-pointer потрібен для коректних стеків у perf/FlameGraph
CFLAGS_COMMON = -g -fno-omit-frame-pointer -Wall

all: matmul_O0 matmul_O3

matmul_O0: matmul.c
	gcc $(CFLAGS_COMMON) -O0 -o $@ $<

matmul_O3: matmul.c
	gcc $(CFLAGS_COMMON) -O3 -march=native -o $@ $<

# Асемблер для порівняння (п. 4.1); також можна вставити matmul.c в godbolt.org
asm: matmul.c
	gcc -O0 -S -masm=intel -o asm_O0.s $<
	gcc -O3 -march=native -S -masm=intel -o asm_O3.s $<
	@echo "AVX/FMA інструкції в O3:"; grep -E 'vmulpd|vaddpd|vfmadd|ymm|zmm' asm_O3.s | head -20 || true

clean:
	rm -f matmul_O0 matmul_O3 asm_*.s perf.data* out.perf *.svg
