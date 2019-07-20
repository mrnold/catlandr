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

OBJ_CALCS := $(addprefix $(OBJ)/, $(CALCS))
BIN_CALCS := $(addprefix $(BIN)/, $(CALCS))

ALLOCS := 3000

SDCCFLAGS := --compile-only -mz80 --nostdlib --no-std-crt0 --reserve-regs-iy --opt-code-speed --max-allocs-per-node $(ALLOCS) -Isource

.PHONY: all $(CALCS)

all: $(CALCS)

86: $(OBJ)/86/catlandr.p | $(BIN)/86
	$(MOVE) $(call PATHSUB, $<) $(call PATHSUB, $(BIN)/86/catlandr.86p)

84pse: $(OBJ)/84pse/catlandr.p | $(BIN)/84pse
	$(MOVE) $(call PATHSUB, $<) $(call PATHSUB, $(BIN)/84pse/catlandr.8xp)

$(BIN_CALCS): | $(BIN)
	$(MKDIR) $(call PATHSUB, $@)

$(OBJ_CALCS): | $(OBJ)
	$(MKDIR) $(call PATHSUB, $@)

$(BIN):
	$(MKDIR) $(call PATHSUB, $@)

$(OBJ):
	$(MKDIR) $(call PATHSUB, $@)

.SECONDEXPANSION:
$(addsuffix /catlandr.p, $(OBJ_CALCS)): $(OBJ)/%/catlandr.p: $(OBJ)/%/catlandr.bin | $(OBJ)/$$*
	$(PYTHON) tools/binto86p.py $* $< $@

$(addsuffix /catlandr.bin, $(OBJ_CALCS)): $(OBJ)/%/catlandr.bin: $(OBJ)/%/main.ihx | $(OBJ)/$$*
	$(PYTHON) tools/ihxtobin.py $<
	$(PYTHON) tools/trim.py $(OBJ)/$*/main.map $(OBJ)/$*/main.bin $(CODELOC$*)
	$(MOVE) $(call PATHSUB, $(OBJ)/$*/main.bin) $(call PATHSUB, $@)

$(addsuffix /main.ihx, $(OBJ_CALCS)): $(OBJ)/%/main.ihx: $(OBJ)/%/crt0.rel $(addprefix $(OBJ)/%/, $(addsuffix .rel, $(SOURCES))) $(OBJ)/%/ti.rel | $(OBJ)/$$*
	$(SDLD) -m -w -x -i -b _CODE=$(CODELOC$*) -b _DATA=0x0000 $@ $^

$(addsuffix /ti.rel, $(OBJ_CALCS)): $(OBJ)/%/ti.rel: source/calc/ti%/ti$$*.c | $(OBJ)/$$*
	$(SDCC) $(SDCCFLAGS) -DCALCULATOR_MODEL=$(MODEL$*) $< -o $@

$(addsuffix /crt0.rel, $(OBJ_CALCS)): $(OBJ)/%/crt0.rel: source/calc/ti%/crt0.s | $(OBJ)/$$*
	$(SDAS) -p -g -o $@ $<

$(foreach calc, $(CALCS), $(addsuffix .rel, $(addprefix $(OBJ)/$(calc)/, $(SOURCES)))): %.rel : source/$$(notdir %).c | $(OBJ_CALCS)
	$(SDCC) $(SDCCFLAGS) -DCALCULATOR_MODEL=$(MODEL$(subst /$(notdir $@),,$(subst $(OBJ)/,,$@))) $^ -o $@

clean:
	$(RMDIR) $(BIN)
	$(RMDIR) $(OBJ)
