SDCC := sdcc
SDAS := sdasz80
SDLD := sdldz80
PYTHON := python

ifdef SystemRoot
	RMDIR := rmdir /S /Q
	MKDIR := mkdir
	MOVE := cmd /c move
	PATHSUB = $(subst /,\,$(1))
else
	RMDIR := rm -rf
	MKDIR := mkdir -p
	MOVE := mv
	PATHSUB = $(1)
endif

OBJ := obj
BIN := bin
SOURCES := $(basename $(notdir $(wildcard source/*.c)))

CALCS := 86 84pse
MODEL86 := 86
MODEL84pse := 8402
CODELOC86 := 0xD748
CODELOC84pse := 0x9D95

.PHONY: all $(CALCS)

all: 86 84pse

86: $(OBJ)/86/catlandr.p | $(BIN)/86
	$(MOVE) $(call PATHSUB, $<) $(call PATHSUB, $(BIN)/86/catlandr.86p)

84pse: $(OBJ)/84pse/catlandr.p | $(BIN)/84pse
	$(MOVE) $(call PATHSUB, $<) $(call PATHSUB, $(BIN)/84pse/catlandr.8xp)

$(addprefix $(BIN)/, $(CALCS)): | $(BIN)
	$(MKDIR) $(call PATHSUB, $@)

$(addprefix $(OBJ)/, $(CALCS)): | $(OBJ)
	$(MKDIR) $(call PATHSUB, $@)

$(BIN):
	$(MKDIR) $(call PATHSUB, $@)

$(OBJ):
	$(MKDIR) $(call PATHSUB, $@)

.SECONDEXPANSION:
$(addsuffix /catlandr.p, $(addprefix $(OBJ)/, $(CALCS))): $(OBJ)/%/catlandr.p: $(OBJ)/%/catlandr.bin | $(OBJ)/$$*
	$(PYTHON) tools/binto86p.py $* $< $@

$(addsuffix /catlandr.bin, $(addprefix $(OBJ)/, $(CALCS))): $(OBJ)/%/catlandr.bin: $(OBJ)/%/main.ihx | $(OBJ)/$$*
	$(PYTHON) tools/ihxtobin.py $<
	$(PYTHON) tools/trim.py $(OBJ)/$*/main.map $(OBJ)/$*/main.bin $(CODELOC$*)
	$(MOVE) $(call PATHSUB, $(OBJ)/$*/main.bin) $(call PATHSUB, $@)

$(addsuffix /main.ihx, $(addprefix $(OBJ)/, $(CALCS))): $(OBJ)/%/main.ihx: $(addprefix $(OBJ)/%/, $(addsuffix .rel, $(SOURCES))) $(OBJ)/%/ti.rel | $(OBJ)/$$*
	$(SDLD) -m -w -x -i -b _CODE=$(CODELOC$*) -b _DATA=0x0000 $@ $^

$(addsuffix /ti.rel, $(addprefix $(OBJ)/, $(CALCS))): $(OBJ)/%/ti.rel: source/calc/ti%/ti$$*.c | $(OBJ)/$$*
	$(SDCC) --compile-only -mz80 --nostdlib --no-std-crt0 --reserve-regs-iy --opt-code-speed --max-allocs-per-node 300 -Isource -DCALCULATOR_MODEL=$(MODEL$*) $< -o $@

$(foreach calc, $(CALCS), $(addsuffix .rel, $(addprefix $(OBJ)/$(calc)/, $(SOURCES)))): %.rel : source/$$(notdir %).c | $(addprefix $(OBJ)/, $(CALCS))
	$(SDCC) --compile-only -mz80 --nostdlib --no-std-crt0 --reserve-regs-iy --opt-code-speed --max-allocs-per-node 300 -Isource -DCALCULATOR_MODEL=$(MODEL$(subst /$(notdir $@),,$(subst $(OBJ)/,,$@))) $^ -o $@

clean:
	$(RMDIR) $(BIN) $(OBJ)
