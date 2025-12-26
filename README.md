# Projeto – Gerador de Dados Estruturados

## Ideias de nomes
- nexus  
- datastorm  
- gridstorm  
- thrust  

---

## Introdução
Este projeto tem como objetivo principal **praticar programação em C em nível avançado**, explorando conceitos como:

- Arquitetura modular e desacoplada  
- Paralelismo e concorrência  
- Programação multi-thread  
- Manipulação de arquivos  
- Persistência de dados em bancos relacionais  

O foco não é apenas gerar dados, mas **construir um sistema extensível, previsível e arquiteturalmente sólido**.

---

## Organização de diretórios
``` text
montar
```

---

## Ideia central
O sistema é organizado em **três módulos principais**, cada um com responsabilidade bem definida:

### Core
- Inicializa o sistema
- Lê o arquivo de configuração (`Genfile.toml`)
- Cria e gerencia recursos compartilhados:
  - Buffer thread-safe
  - Semáforos / mutex
- Inicializa e sincroniza as threads do **Source** e do **Target**
- Controla o ciclo de vida da aplicação

### Source
- Interpreta o schema descrito no `Genfile.toml`
- Gera dados estruturados conforme as regras definidas
- Atua como **produtor** no modelo produtor–consumidor
- Envia registros gerados para o buffer intermediário

### Target
- Atua como **consumidor**
- Lê registros do buffer
- Agrupa registros em lotes configuráveis
- Persiste os dados no destino configurado (PostgreSQL na versão inicial)
- Gerencia suas próprias conexões e recursos

---

## Buffer de armazenamento de dados
O buffer funciona como um **pipeline produtor–consumidor**, com as seguintes características:

- Thread-safe  
- Bloqueante  
- Com backpressure (tamanho máximo configurável)  

### Comportamento do Source
- Gera um registro por vez
- Tenta inserir no buffer
- Se o buffer estiver cheio → espera
- Ao atingir o número total de registros → finaliza

### Comportamento do Target
- Remove registros do buffer
- Forma lotes para persistência
- Se não houver registros suficientes:
  - Source ativo → espera
  - Source finalizado → envia lote parcial
- Finaliza quando:
  - Source terminou **e**
  - Buffer está vazio

### Estados implícitos do buffer
1. **Normal** – produção e consumo ativos  
2. **Source finalizado** – apenas consumo  
3. **Encerrado** – buffer vazio, threads finalizadas  

O tamanho máximo do buffer é definido no `Genfile.toml`. Caso não seja especificado, o sistema escolhe um valor seguro com base em memória disponível.

---

## Responsabilidades dos módulos
Separação rígida de responsabilidades é **regra**, não sugestão:

- **Core**
  - Gerencia threads
  - Gerencia buffer e sincronização
  - Não gera dados
  - Não persiste dados

- **Source**
  - Gerencia geração de dados
  - Gerencia contexto de geração
  - Não conhece destino final dos dados

- **Target**
  - Gerencia persistência
  - Abre e fecha conexões
  - Não conhece regras de geração

---

## Genfile.toml
Arquivo de configuração central do sistema.

Responsável por:
- Definir o schema lógico dos dados
- Definir regras de geração
- Definir relacionamentos
- Definir destino e parâmetros de persistência
- Definir limites (buffer, lotes, seeds, etc.)

### Exemplo simplificado
```toml
[dataset]
records = 100000
buffer_size = 5000

[user.id]
type = "sequence"

[user.name]
type = "name"

[user.email]
type = "email"

```

## Módulos 

### Source 
O módulo source é responsável por toda a geração de dados. Sua estrutura é dividida em 4 módulos:
- Orchestrador (`source.c`)
- Services (`services/**.c`)
- Generators (`generators/**.c`)
- Context (`context.c`)

#### Estruturação do módulo
O módulo se encontra no diretório source do projeto, e está dividido da seguinte forma:
``` text
source/
 ├── source.c              // orquestrador
 ├── context.c             // gerenciamento de estado
 ├── services/             // lógica de geração
 │   ├── random_service.c
 │   ├── sequence_service.c
 │   ├── dictionary_service.c
 │   └── reference_service.c
 ├── generators/           // composição de services
 │   ├── name_gen.c
 │   ├── cpf_gen.c
 │   ├── phone_gen.c
 │   ├── email_gen.c
 │   └── derived_gen.c
```

A seguir, os submódulos do módulo source são listados e descritos de forma detalhada, bem como suas atribuições e subdivisões.

#### Orchestrador
Responsável por: 
- Ler o schema do `Genfile.toml`
- Controlar o número total de registros
- Decidir quais generators iniciar e usar para cada campo de registro gerado.
- Orquestrar a criação de cada registro.
- Enviar registros ao buffer. 

#### Services
São os motores responsáveis pela geração de dados. Construídos de forma genérica e reutilizável.

Características:
- Implementam toda a lógica de geração
- Podem manter estado interno no contexto
- São reutilizáveis
- Não conhecem schema nem campos específicos, apenas são chamados e geram dados de acordo com sua responsabilidade e o estado do seus campos do contexto.

Tipos de services:
- Stateless
    - Inteiro aleatório
    - String aleatória
    - UUID
    - Hash
- Stateful
    - Sequências
    - Contadores
    - Índices incrementais
- Referenciados
    - Lookup de chaves estrangeiras
    - Pools de valores já gerados
    - Cache limitado de referências

#### Generators
Responsáveis por combinar services para gerar um tipo de dado específico.

Características:
- Stateless
- Não armazenam estado
- Não implementam algoritmos complexos
- Apenas coordenam chamadas aos services

Exemplos:
- name_generator
- cpf_generator
- email_generator
- phone_generator
- derived_generator


##### Interface padrão

Para fins de padronização no uso dos generators, todas as implementações devem a seguinte implementação: 

```c 
// descrição dos parametros vindos do Genfile
typedef struct { 
    const char* name;
    const char* generator;
    void*       params;     // parseado
} FieldSpec;

// descrição da assinatura dos generators
typedef FieldValue (*GeneratorFn)( 
    Context* context,
    const FieldSpec* field_spec,
    const Record* partial_record
);


```

`void* params`: struct específica para cada generator que descreve seus possíveis parâmetros.

##### Exemplo de implementação

Para a configuração de campo a seguir:
```toml 
[user.age]
generator = "random_int"
min = 18
max = 65
```

temos o parametro `nome` do campo com valor `age`, o gerador escolhido é `random_int`, que possui os parâmetros `min` e `max`.

Estes parâmetros descrevem como o valor será gerado. 

Cada generator define explicitamente quais parâmetros aceita por meio de uma struct própria. No caso do `random_int`, os parâmetros possíveis são descritos pela struct:
``` c 
// Descrição dos parâmetros para o random_int generator
typedef struct {
    int min;
    int max;
} RandomIntParams;
```

Esta estrutura descreve o **contrato de configuração** do generator. Sendo assim, qualquer campo que utilize `random_int` deve oferecer parâmetros compatíveis com essa estrutura.

Durante a fase de inicialização do módulo Source, o arquivo Genfile.toml é lido e interpretado pelo Orchestrator. Neste momento, os valores `min` e `max` são lidos do arquivo, convertidos para o tipo `int`, validados e armazenados em uma instância `RandomIntParams`. 

Este processo ocorre uma única vez antes da geração dos registros.

Após o parsing, o campo é representado internamente por uma estrutura `FieldSpec`, que conexta as informações comuns a qualquer campo (nome e generator) com os parâmetros específicos.  


```c 
FieldSpec {
    name      = "age",
    generator = "random_int",
    params    = (RandomIntParams*) {...}
}
```

O ponteiro `params` aponta para uma instância válida de `RandomIntParams`, já pronta para uso.


⚠️ Importante: 
- `params` não contém dados em formato textual.
- Nenhum parsing ocorre durante a geração.
- O generator não conhece TOML.

Durante a geração de cada registro, o ochestrator chama o generator associado ao campo, passando o contexto global, especificação do campo (`FieldSpec`) e o registro parcial (`Record`).

```c 
FieldValue random_int_gen(
    Context* context,
    const FieldSpec* spec,
    const Record* record
) {
    // Converte os parâmetros genéricos para o tipo esperado
    RandomIntParams* p = (RandomIntParams*) spec->params;

    // Usa os parâmetros diretamente, sem parsing adicional
    int value = random_between(p->min, p->max);

    // Monta e retorna o valor gerado
    FieldValue fv;
    fv.type = FIELD_INT;
    fv.value.i = value;
    return fv;
}
```

Neste ponto o generator apenas executa a lógica de geração. Não há validações, leituras adicionais de configuração ou dependências de formato externo.

⚠️ Importante:
- Generator gera valores.
- Orchestrator interpreta configuração.
- Context guarda estado.
- Generators não devem ler Genfile ou depender de informações diretas dele 
- Generators não devem receber parametros em string com conversão posterior. Formato deve ser padronizado. 


#### Context

Objeto central de estado da geração.

Função:
- Armazenar estados compartilhados
- Garantir previsibilidade
- Permitir reprodutibilidade
- Evitar estado espalhado pelo código
- Estrutura conceitual:

``` text
SourceContext
 ├── RandomContext
 ├── SequenceContext
 ├── DictionaryContext
 ├── ReferenceContext
 └── StatsContext
```

- RandomContext
    - Seeds
    - Reprodutibilidade
- SequenceContext
    - Contadores por campo
    - Valores iniciais
    - Steppers
- DictionaryContext
    - Arquivos de dicionários
    - Índices
    - Tamanhos
- ReferenceContext
    - Pools de chaves estrangeiras
    - Cache limitado
    - Estratégias de lookup
- StatsContext
    - Registros gerados
    - Registros enviados
    - Erros
    - Estatísticas gerais 

⚠️ Importante:

- Geradores nunca acessam estruturas internas diretamente

- Comunicação sempre via funções bem definidas

#### Fluxo de geração de um registro

Cada registro é gerado de forma independente seguindo os seguintes passos.  

- Criar registro vazio (alocação de memória).
- Iterar campos do registro. 
Para cada campo:
    - Identificar generator necessário
    - Chamar generator com:
        - Context
        - Parâmetros
        - Registro parcial
- Adicionar valor ao registro
- Enviar registro ao buffer

#### Geração de registros 

A geração de registros segue uma lógica linear simples descrita a seguir: 

``` text
1. Aloca Record vazio
2. Itera schema
3. Para cada campo:
   - resolve generator
   - chama generator
   - recebe FieldValue
   - adiciona ao Record
4. Envia Record para o buffer
```

Os tipos usados são descritos a seguir e no arquivo `main.h`.

``` c 
   
typedef enum {
    FIELD_INT,
    FIELD_FLOAT,
    FIELD_STRING,
    FIELD_BOOL,
    FIELD_NULL
} FieldType;

typedef struct {
    FieldType type;
    union {
        int         i;
        double      f;
        char*       c;
        int         b;    
    } value;

} FieldValue;

typedef struct {
    const char*     name;
    FieldValue      value;
} Field;

typedef struct {
    Field*          fields;
    size_t          field_count;
} Record;
```



















