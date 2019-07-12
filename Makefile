SDCC := sdcc
SDAS := sdasz80
SDLD := sdldz80
PYTHON := python

CODELOC86 := 0xD748

all: catlandr.86p

catlandr.86p: bin/86/catlandr.bin
	$(PYTHON) tools/binto86p.py 86 bin/86/catlandr.bin $@

bin/86/catlandr.bin: bin/86/main.ihx
	$(PYTHON) tools/ihxtobin.py $^
	$(PYTHON) tools/trim.py bin/86/main.map bin/86/main.bin $(CODELOC86)
	mv bin/86/main.bin $@

bin/86/main.ihx: bin/main.rel bin/bitmap.rel bin/camera.rel bin/game.rel bin/kibble.rel bin/kitty.rel bin/lander.rel bin/menu.rel bin/moon.rel bin/physics.rel bin/ti86.rel
	@mkdir -p $(dir $@)
	$(SDLD) -m -w -x -i -b _CODE=$(CODELOC86) -b _DATA=0x0000 $@ $^

bin/main.rel: source/main.c
	@mkdir -p $(dir $@)
	$(SDCC) --compile-only -mz80 --nostdlib --no-std-crt0 --reserve-regs-iy --opt-code-speed --max-allocs-per-node 300 -Isource -DCALCULATOR_MODEL=86 $^ -o $@

# Intentionally max allocs 3000
bin/moon.rel: source/moon.c
	@mkdir -p $(dir $@)
	$(SDCC) --compile-only -mz80 --nostdlib --no-std-crt0 --reserve-regs-iy --opt-code-speed --max-allocs-per-node 300 -Isource -DCALCULATOR_MODEL=86 $^ -o $@

bin/ti86.rel: source/calc/ti86/ti86.c
	$(SDCC) --compile-only -mz80 --nostdlib --no-std-crt0 --reserve-regs-iy --opt-code-speed --max-allocs-per-node 300 -Isource -DCALCULATOR_MODEL=86 $^ -o $@

bin/%.rel: source/%.c
	@mkdir -p $(dir $@)
	$(SDCC) --compile-only -mz80 --nostdlib --no-std-crt0 --reserve-regs-iy --opt-code-speed --max-allocs-per-node 300 -Isource -DCALCULATOR_MODEL=86 $^ -o $@

clean:
	rm -rf bin catlandr.86p
