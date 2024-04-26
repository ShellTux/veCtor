# Variables {{{

# Variables - Repo Source Codde {{{

ASSETS_DIR      = assets
DOCS_DIR        = docs
INCLUDE_DIRS    = $(shell pwd)/include
OBJ_DIR         = obj
PANDOC_DATA_DIR = pandoc
SRC_DIR         = src
TARGETS_DIR     = build
TARGETS         = vector

SOURCES := $(shell find $(SRC_DIR) \
	   -type f \
	   -name "*.c" -o \
	   -name "*.cpp" 2>/dev/null | tr '\n' ' ')
HEADERS  = $(shell find $(INCLUDE_DIRS) \
	   -name "*.h" -o \
	   -name "*.hpp" \
	   2>/dev/null | tr '\n' ' ')

GITIGNORE := $(OBJ_DIR) $(TARGETS_DIR) $(TARGETS)

define newline


endef

# }}}

# Variables - Documentation {{{

ARCHIVE             = archive.zip
INSTALLATION_MANUAL =
PRESENTATION        =
REPORT              =
USER_MANUAL         =

PANDOC_OPTS      := --resource-path=.:..:$(DOCS_DIR):$(ASSETS_DIR)
PANDOC_THEME_DIR := $(PANDOC_DATA_DIR)/themes
PANDOC_THEME     := 

PANDOC_OPTS += --variable=theme:Warsaw

ifneq ($(wildcard $(PANDOC_THEME:%=$(PANDOC_THEME_DIR)/%.theme)),)
PANDOC_OPTS += $(PANDOC_THEME:%=--highlight-style=$(PANDOC_THEME_DIR)/%.theme)
endif

ifneq ($(wildcard $(PANDOC_DATA_DIR)),)
	PANDOC_OPTS += \
		       $(foreach filter,\
		       $(wildcard $(PANDOC_DATA_DIR)/filters/*.lua),\
		       --lua-filter=$(filter))
endif

DOCUMENTS     := $(REPORT) $(USER_MANUAL) $(INSTALLATION_MANUAL)
PRESENTATIONS := $(PRESENTATION)

GITIGNORE += $(DOCUMENTS) $(PRESENTATIONS)

# }}}

# Variables - Python {{{

VENV   := 
PYTHON := ./$(VENV)/bin/python
PIP    := ./$(VENV)/bin/pip

GITIGNORE += $(VENV)

# }}}

# Variables - Compilation {{{

CC     = gcc
CCP    = g++
CFLAGS = -Wall -Wextra -Werror
LINKS  =

CFLAGS += -Wno-error=unused-parameter
CFLAGS += -Wno-error=unused-variable
CFLAGS += -Wno-error=unused-but-set-variable
CFLAGS += -fdiagnostics-color=always
CFLAGS += $(INCLUDE_DIRS:%=-I%)

# }}}

# Variables - Miscellaneous {{{

OPEN = xdg-open

# }}}

# }}}

# Rules {{{

# Rules - Custom {{{

all: warning $(VENV) $(DOCUMENTS) $(PRESENTATIONS) $(TARGETS)

.PHONY: gdb
gdb: MAKEFLAGS += --always-make --no-print-directory
gdb: CFLAGS += -g -Og
gdb:
	$(MAKE) $(MAKEFLAGS) CFLAGS="$(CFLAGS)" $(TARGETS)

.PHONY: debug
debug: MAKEFLAGS += --always-make --no-print-directory
debug: CFLAGS += -DDEBUG=1
debug:
	$(MAKE) $(MAKEFLAGS) CFLAGS="$(CFLAGS)" $(TARGETS)

.PHONY: rebuild
rebuild: MAKEFLAGS += --always-make --no-print-directory
rebuild:
	$(MAKE) $(MAKEFLAGS) $(TARGETS)

.PHONY: run
run: warning $(TARGETS)
	@for target in $(TARGETS) ; do echo ./$$target ; ./$$target ; done

clean-ipcs:
ifeq ($(shell echo "$$(id --user) < 1000" | bc), 0)
	ipcrm --all
else
	@echo This rule is meant to remove non-root ipcs resources
endif

.PHONY: watch
watch:
	$(eval INOTIFYWAIT_OPTS = --quiet --event modify)

	@while true ;\
	do \
		$(MAKE) warning --no-print-directory || exit 1 ;\
		$(MAKE) --no-print-directory $(TARGETS); \
		inotifywait $(INOTIFYWAIT_OPTS) $(SOURCES) $(HEADERS); \
	done

.PHONY: PRINT-MACROS
PRINT-MACROS:
	@make --print-data-base \
		| grep -A1 "^# makefile" \
		| grep -v "^#\|^--" \
		| sort

.PHONY: warning
warning:
ifneq ($(shell pwd | grep --count ' '),0)
	@echo -e '\033[33m[WARNING]\033[0m:' \
		'Current working directory of Makefile contains spaces.' \
		'This is known to cause bugs.'
	@echo -e '\033[36m[INFO]\033[0m: pwd: $(shell pwd)'
	@echo 'Please Try compiling by making sure' \
		'the full path to this Makefile does not contain spaces.'
	-exit 1
endif

clang-tidy: .clang-tidy
	parallel --jobs 4 --group clang-tidy --quiet ::: $(SOURCES)

define CLANG_TIDY_CONTENT
Checks: -*,readability-identifier-naming
CheckOptions:
  - { key: readability-identifier-naming.EnumCase,            value: CamelCase}
  - { key: readability-identifier-naming.EnumConstantCase,    value: UPPER_CASE}
  - { key: readability-identifier-naming.FunctionCase,        value: camelBack}
  - { key: readability-identifier-naming.StructCase,          value: CamelCase}
  - { key: readability-identifier-naming.MacroDefinitionCase, value: UPPER_CASE}
endef

.clang-tidy:
	@echo '$(subst $(newline),\n,$(CLANG_TIDY_CONTENT))' | tee $@

clang-format: .clang-format
	clang-format --verbose -i $(SOURCES) $(HEADERS) 2>&1

define CLANG_FORMAT_CONTENT
---
AlignAfterOpenBracket: Align
AlignArrayOfStructures: Right
AlignConsecutiveAssignments:
  Enabled: true
  AcrossEmptyLines: false
  AcrossComments: true
  AlignCompound: true
  PadOperators: true
AlignConsecutiveBitFields:
  Enabled: true
  AcrossEmptyLines: false
  AcrossComments: true
  AlignCompound: true
  PadOperators: true
AlignConsecutiveDeclarations: false
AlignConsecutiveMacros:
  Enabled: true
  AcrossEmptyLines: false
  AcrossComments: true
  AlignCompound: true
  PadOperators: true
AlignOperands: Align
# AlignTrailingComments:
#   Kind: Always
#   OverEmptyLines: 2
AlignEscapedNewlines: Left
AllowAllArgumentsOnNextLine: false
AllowAllConstructorInitializersOnNextLine: false
AllowAllParametersOfDeclarationOnNextLine: true
AllowShortBlocksOnASingleLine: Empty
AllowShortCaseLabelsOnASingleLine: false
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: AllIfsAndElse
AllowShortLoopsOnASingleLine: true
AlwaysBreakAfterDefinitionReturnType: None
AlwaysBreakAfterReturnType: None
AlwaysBreakBeforeMultilineStrings: false
BinPackArguments: false
BinPackParameters: false
BreakBeforeBraces: Custom
BraceWrapping:
  AfterCaseLabel: false
  AfterClass: false
  BeforeElse: false
  AfterControlStatement: Never
  AfterEnum: false
  AfterFunction: true
  AfterNamespace: false
  AfterStruct: false
  AfterUnion: false
  AfterExternBlock: false
  BeforeCatch: false
  BeforeLambdaBody: false
  BeforeWhile: false
  IndentBraces: false
  SplitEmptyFunction: false
  SplitEmptyRecord: false
  SplitEmptyNamespace: false
BreakBeforeBinaryOperators: All
BreakBeforeTernaryOperators: true
BreakConstructorInitializers: BeforeComma
BreakInheritanceList: BeforeComma
ColumnLimit: 80
IncludeBlocks: Regroup
IndentCaseLabels: false
IndentPPDirectives: BeforeHash
IndentWidth: 8
IndentWrappedFunctionNames: false
JavaScriptQuotes: Single
JavaScriptWrapImports: true
KeepEmptyLinesAtTheStartOfBlocks: false
Language: Cpp
MaxEmptyLinesToKeep: 2
PointerAlignment: Right
ReflowComments: false
SortIncludes: true
SortUsingDeclarations: true
SpaceAfterCStyleCast: true
SpaceAfterLogicalNot: false
SpaceBeforeAssignmentOperators: true
SpaceBeforeParens: ControlStatements
SpaceBeforeRangeBasedForLoopColon: true
SpaceInEmptyParentheses: false
SpacesInCStyleCastParentheses: false
SpacesInContainerLiterals: true
SpacesInParentheses: false
SpacesInSquareBrackets: false
TabWidth: 8
UseTab: ForIndentation
endef

.clang-format:
	@echo '$(subst $(newline),\n,$(CLANG_FORMAT_CONTENT))' | tee $@

setup: .clangd

.PHONY: .clangd
.clangd: GITIGNORE += .clangd
.clangd: .gitignore
	rm --force $@

	@echo Diagnostics: | tee --append $@
	@echo '  UnusedIncludes: Strict' | tee --append $@
	@echo '  MissingIncludes: Strict' | tee --append $@
	@echo CompileFlags: | tee --append $@
	@echo '  Add:' | tee --append $@

	@for flag in $(CFLAGS) ; do echo "    - $$flag" | tee --append $@ ; done

.gitignore:
ifneq ($(shell git rev-parse --show-toplevel 2>/dev/null),)
	$(eval APPEND_GITIGNORE := tr ' ' '\n' | tee --append $@)
	@echo $(GITIGNORE) | tr ' ' '\n' | tee $@
	@echo | $(APPEND_GITIGNORE)

	$(eval IGNORE_API = https://www.toptal.com/developers/gitignore/api)

ifneq ($(SOURCES),)
	curl --silent --location $(IGNORE_API)/c >> $@
endif
ifneq ($(VENV),)
	curl --silent --location $(IGNORE_API)/python >> $@
endif

make.snippets: Makefile
	@echo snippet Template \"Makefile Template\" | tee $@

	sed 's|^|\t|' $< | tee --append $@
	sed -i '0,/assets/s//$${1:assets}/' $@
	sed -i '0,/docs/s//$${2:docs}/' $@
	sed -i '0,/$$(shell pwd)\/include/s//$${3:$$(shell pwd)\/include}/' $@
	sed -i '0,/obj/s//$${4:obj}/' $@
	sed -i '0,/pandoc/s//$${5:pandoc}/' $@
	sed -i '0,/src/s//$${6:src}/' $@
	sed -i '0,/build/s//$${7:build}/' $@
	sed -i '0,/hello-world factorial/s//$${8:target}/' $@
	sed -i '0,/archive.zip/s//$${9:archive.zip}/' $@
	sed -i '0,/installation-manual.pdf/s//$${10:installation-manual.pdf}/' \
		$@
	sed -i '0,/presentation.pdf/s//$${11:presentation.pdf}/' $@
	sed -i '0,/report.pdf/s//$${12:report.pdf}/' $@
	sed -i '0,/user-manual.pdf/s//$${13:user-manual.pdf}/' $@
	sed -i '0,/onehalfdark/s//$${14:onehalfdark}/' $@
	sed -i '0,/venv/s//$${15:venv}/' $@
	sed -i '0,/gcc/s//$${16:gcc}/' $@
	sed -i '0,/g++/s//$${17:g++}/' $@
	sed -i '0,/-Wall -Wextra -Werror/s//$${18:-Wall -Wextra -Werror}/' $@
	# TODO: Add Links
	sed -i '0,/xdg-open/s//$${19:xdg-open}/' $@
	sed -i '0,/-g -Og/s//$${20:-g -Og}/' $@
	sed -i '0,/-DDEBUG=1/s//$${21:-DDEBUG=1}/' $@

	sed -i 's|\$$\$$|\$$\$$|g' $@

endif


.PHONY: clean
clean:
	@printf "\n\033[31m"
	@printf "########################\n"
	@printf "Cleaning ...\n" $@
	@printf "########################\n"
	@printf "\033[0m\n"
	rm --force $(TARGETS) $(DOCUMENTS) $(PRESENTATIONS)
	rm --recursive --force $(OBJ_DIR) $(VENV)
	find . -type f -name '*.pyc' -delete

help:
	man

# }}}

# Rules - Documentation {{{

$(PRESENTATIONS): %.pdf: $(DOCS_DIR)/%.md
	pandoc $(PANDOC_OPTS) --to=beamer --output=$@ $<

$(DOCUMENTS): %.pdf: $(DOCS_DIR)/%.md
	pandoc $(PANDOC_OPTS) --output=$@ $<

archive: $(ARCHIVE)

.PHONY: $(ARCHIVE)
$(ARCHIVE): $(DOCUMENTS) $(PRESENTATIONS)
	git archive --output=$@ $(^:%=--add-file=%) HEAD

# }}}

# Rules - Compilation {{{

$(OBJ_DIR)/%.c.o: %.c $(HEADERS)
	@printf "\n\033[31m"
	@printf "########################\n"
	@printf "Building %s\n" $@
	@printf "########################\n"
	@printf "\033[0m\n"
	mkdir --parents "$$(dirname "$@")"
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.cpp.o: %.cpp $(HEADERS)
	@printf "\n\033[31m"
	@printf "########################\n"
	@printf "Building %s\n" $@
	@printf "########################\n"
	@printf "\033[0m\n"
	mkdir --parents "$$(dirname "$@")"
	$(CCP) $(CFLAGS) -c -o $@ $<

# TODO: Add Parallel Compilation

_SOURCES = vector test
vector: $(_SOURCES:%=$(OBJ_DIR)/$(SRC_DIR)/%.c.o)
	$(CC) $(CFLAGS) -o $@ $^

# }}}

# Rules - Python {{{

$(VENV): $(VENV)/bin/activate .gitignore

$(VENV)/bin/activate: requirements.txt
	python3 -m venv $(VENV)
	$(PIP) install --requirement $<

# }}}

# }}}

# vim: foldmethod=marker foldlevel=1
