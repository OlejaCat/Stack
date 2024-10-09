CFLAGS = -Wall -Wextra -Og
ifeq ($(CC), clang)
CFLAGS += -Wconversion -Wdangling -Wdeprecated -Wdocumentation -Wformat -Wfortify-source -Wgcc-compat -Wgnu -Wignored-attributes -Wignored-pragmas -Wimplicit -Wmost -Wshadow-all -Wthread-safety -Wuninitialized -Wunused -Wformat
CFLAGS += -Wargument-outside-range -Wassign-enum -Wbitwise-instead-of-logical -Wc23-extensions -Wc11-extensions -Wcast-align -Wcast-function-type -Wcast-qual -Wcomma -Wcomment -Wcompound-token-split -Wconditional-uninitialized -Wduplicate-decl-specifier -Wduplicate-enum -Wduplicate-method-arg -Wduplicate-method-match -Wempty-body -Wempty-init-stmt -Wenum-compare -Wenum-constexpr-conversion -Wextra-tokens -Wfixed-enum-extension -Wfloat-equal -Wloop-analysis -Wframe-address -Wheader-guard -Winfinite-recursion -Wno-gnu-binary-literal -Wint-conversion -Wint-in-bool-context -Wmain -Wmisleading-indentation -Wmissing-braces -Wmissing-prototypes -Wover-aligned -Wundef -Wvla
endif
ifeq ($(CC), cc)
CFLAGS += -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-usage=8192 -Wstack-protector
endif
ifeq ($(CC), gcc)
CFLAGS += -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-usage=8192 -Wstack-protector
endif

LINKER_FLAGS = -lm
# ASAN_FLAGS = -fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

INCLUDE = -I MyMiniLib/include -I include -I build
CFLAGS += $(INCLUDE) $(ASAN_FLAGS) -D_CANARY_PROTECT -D_HASH_PROTECT -DDEBUG -D_FORTIFY_SOURCES=3 -ggdb -g3

all: base

base: main.o stack.o dump.o helpful_functions.o logger.o my_asserts.o work_with_doubles.o canary_protection.o hash_protection.o
	@$(CC) $(LINKER_FLAGS) $(CFLAGS) main.o stack.o dump.o helpful_functions.o logger.o my_asserts.o work_with_doubles.o hash_protection.o canary_protection.o -o stack

main.o: source/main.cpp
	@$(CC) $(CFLAGS) -c source/main.cpp

stack.o: source/stack.cpp
	@$(CC) $(CFLAGS) -c source/stack.cpp

dump.o: source/dump.cpp
	@$(CC) $(CFLAGS) -c source/dump.cpp

canary_protection.o: source/canary_protection.cpp
	@$(CC) $(CFLAGS) -c source/canary_protection.cpp

hash_protection.o: source/hash_protection.cpp
	@$(CC) $(CFLAGS) -c source/hash_protection.cpp

helpful_functions.o: MyMiniLib/source/helpful_functions.cpp
	@$(CC) $(CFLAGS) -c MyMiniLib/source/helpful_functions.cpp

logger.o: MyMiniLib/source/logger.cpp
	@$(CC) $(CFLAGS) -c MyMiniLib/source/logger.cpp

my_asserts.o: MyMiniLib/source/my_asserts.cpp
	@$(CC) $(CFLAGS) -c MyMiniLib/source/my_asserts.cpp

work_with_doubles.o: MyMiniLib/source/work_with_doubles.cpp
	@$(CC) $(CFLAGS) -c MyMiniLib/source/work_with_doubles.cpp

clean:
	@rm -rf *.o stack
