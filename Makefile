CC=gcc
# CFLAGS: Flags para compilação (-I para os headers .h)
CFLAGS=-c -Wall -I/usr/include/postgresql

# LDFLAGS: Flags para o linker (-L para o caminho da lib e -lpq para a libpq)
LDFLAGS=-lpq

SOURCE=main.c source/source.c source/context.c source/generators/random_int_gen.c source/services/random_int_service.c

OBJ=$(SOURCE:.c=.o)
EXE=main

all: $(EXE)

# Fase de Linkagem: Une os arquivos .o para criar o executável
$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Fase de Compilação: Cria os arquivos .o a partir dos .c
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ) $(EXE)
