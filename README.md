
# Projeto - Gerador de dados estruturados

## Idéias de nomes 
- nexus
- datastorm
- gridstorm
- thrust

## Introdução
Este projeto tem foi desenvolvido com o intuito de praticar habilidades de programação em linguagem C, bem como conceitos avançados como paralelismo e concorrência, programação multi-thread, manipulação de arquivos e conexão com bancos de dados.

## Idéia central 
A idéia deste projeto é desenvolver um sistema estruturado e desacoplado baseado em 3 módulos: 
<br>

- **Core**: Módulo central, responsável pela inicialização do sistema e pelo gerenciamento dos recursos necessários para a integração dos outros 2 módulos a seguir. Responsável também por guardar o buffer de armazenamento dos dados gerados;
<br>

- **Source**: Responsável por interpretar um arquivo de configuração de dados fornecido pelo usuário (schema) e efetuar a geração de dados aleatórios estruturados baseados no modelo de dados. Os dados são gerados e enviados para um **pipeline** no modelo **produtor-consumidor**, para posteriormente serem lidos pelo módulo targe;
<br>

- **Target**: Responsável pela persistência dos dados gerados no ponto alvo requerido (banco de dados, por exemplo). Implementação inicial utiliza o Postgres como alvo. Seu objetivo é ler os dados enviados ao pipeline pelo módulo source, montar lotes de tamanho configurável e, então, enviá-los para persistência no target desejado;

<br>



## Buffer de armazenamento de dados
Todos os dados gerados serão enviados a um buffer intermediário, que funcionará como uma pipeline com controle de concorrência no modelo produtor-consumidor. 

O buffer deve ser thread-safe, bloqueante e backpressure (com limite de tamanho).
- Source: 
    - Gera registros e tenta inserir no buffer
    - Gera e insere um registro por vez.
    - Se buffer estiver cheio: espera.
    - Se o contador do buffer atingir o limite estabelecido, source finaliza.

- Target: 
    - Retira registros do buffer e envia para o target de persistência;
    - Se não há o número de registros necessário para formar o lote: 
        - Se Source ainda não finalizou: **espera**;
        - Se Source já finalizou: **envia lote parcial**.
    - Se buffer estiver vazio: espera.
    - Se o contador do buffer atingiu o limite estabelecido e o buffer está vazio, target finaliza.

Logo, o buffer mantém 3 estados implícitos: 
1. **Normal**: Source pode inserir e target pode retirar.

2. **Source finalizado**: Nenhum novo dado será produzido, mas Target continua consumindo até esvaziar
3. **Encerrado**: Buffer vazio, source e target finalizaram e retornaram.


O tamanho máximo do buffer deve ser definido pelo usuário no arquivo de configuração (Genfile.toml, descrito mais a frente). A implementação deve considerar um tamanho viável para a quantidade de memória disponível caso o usuário não o defina.

## Responsabilidades 
Importante que cada módulo se atenha à sua própria responsabilidade:

- Core: Apenas inicializa e gerencia as threads dos outros módulos e suas dependências comuns (semáforos, buffers, etc);
- Target: Abre e gerencia conexões com bancos de dados / arquivos de persistência, envia dados e fecha conexões antes de finalizar;
- Source: Gerencia arquivos e geradores necessários para a geração de dados.   


## Genfile.toml
Todas as configurações para geração estruturada de dados deve ser descrita em um arquivo de configuração denominado Genfile.toml. 

Este arquivo é responsável por: 
- Descrever o schema lógico dos dados;
- Descrever as regras de geração
- Descrever os relacionamentos
- Descrever o destino dos dados


### Exemplo

``` toml
    [hello]
    chave = valor
```


## Detalhes dos módulos 

Esta seção se dedica a especificar detalhes de implementação de cada um dos módulos do sistema.


### Source 
Seu objetivo é, em suma, gerar dados estruturados de forma coerente e organizada, e então disponibiliza-los para uso posterior em um buffer localizado no módulo Core.

Suas responsabilidades se atém ao controle das dependências necessárias para a geração dos dados.

O módulo source é formado por outros 3 submódulos, sendo eles: 
- source orchestrator:
    - Responsabilidades: 
        - Ler o schema do Genfile.toml
        - Controlar quantidade de registros
        - Gerenciar e chamar os geradores quando necessário 
        - Definir quais as regras de geração de registros
        - Montar registros 
        - Enviar registros para o buffer

- Geradores de campo (Field generators)
    - Responsabilidades: 
        - 
    - Atuação: 
        - Recebe um contexto e parametros necessários (de acordo com cada gerador)
        - Retorna o valor gerado 
    - Tipos: 
        - Geradores **stateless**: Mais simples - não guarda estados, apenas é chamado e retorna um valor.
            - string aleatória
            - inteiro aleatório
            - UUID
            - hash
        - Geradores **statefull**: Mais complexos - guardam valores internos usados para geração. 
            - número sequencial
            - contador global
            - índice incremental
        - Geradores **referenciados**: Geradores responsáveis por popular campos com relacionamento.
            - Qualquer campo que venha de registros criados anteriormente. 
            - Deve detectar anteriormente quais campos são usados como chave estrangeira e guardar os valores necessários - lookup no contexto. 
            - Soluções possíveis: 
                - pool de valores já gerados (pode servir bem quando a chave estrangeira usada pode ser de qualquer registro já gerado anteriormente)
                - cache limitado: guarda algúns dos registros já criados (normalmente os necessários apenas)
                - lookup no contexto
                - geração de todos os necessários para completar o relacionamento em sequência - não funcionaria para todos os casos, mas é uma forma mais simples e com menos buscas externas pesadas.

Cada um dos registros que será gerado deve seguir uma ordem lógica simples: 

- Criar um registro vazio
- Percorre os campos definidos no schema
- Para cada campo:
    - identifica o gerador
    - chama o gerador com:
        - contexto
        - parâmetros
        - registro parcial
    - adiciona o valor ao registro

- envia o registro para o buffer


O esquema a seguir mostra a organização do diretório do módulo source. 
``` text
source/
 ├── source.c                   // orquestrador
 ├── context.c                  // estado compartilhado
 ├── generators/                // geradores de dados 
 │   ├── name_gen.c
 │   ├── cpf_gen.c
 │   ├── phone_gen.c
 │   ├── uuid_gen.c
 │   ├── seq_gen.c
 │   ├── random_int_gen.c
 │   ├── derived_gen.c
 │   └── ref_gen.c
```

## Context 

Este é um serviço de estado que atua como persistidor de informações para os geradores de dados entre gerações diferentes. O objetivo principal é centralizar todas as informações necessárias para atuação dos geradores sem que estes guardem informações relevantes entre gerações, como endereços de arquivos de dados ou informações sobre gerações anteriores.

Geradores devem ser, ao máximo possível, funções de execução efêmera, sem persistência de estado por conta própria, tendo como única responsabilidade o conhecimento sobre a geração do seu próprio tipo de dado, gerando previsibilidade e testabilidade. 

A estrutura do contexto segue o seguinte padrão: 

``` text
SourceContext
 ├── RandomContext          
 ├── SequenceContext
 ├── DictionaryContext
 ├── ReferenceContext
 └── StatsContext
```

- RandomContext: 
Responsável pelas variáveis relacionadas à geração aleatória (seeds) - garantia de reprodutibilidade 

- SequenceContext:
Guarda contadores por campo, steppers e valores iniciais declarados no Genfile. Tudo o que é necessário para geração de qualquer tipo de sequências.  

- DictionaryContext
Guarda arquivos de dicionários (usados para geração de dados que fazem sentido), tamanhos e indices.

- ReferenceContext
Guarda informações necessárias para criação de relacionamentos, como pools de valores já gerados, índices válidos, limites de memória (cache), etc. 
**Formato ainda a definir**

- StatsContext
Guarda estatísticas da geração, como número de registros já gerados e enviados, numero de erros, etc.

Comunicação gerador-contexto é simples. O gerador recebe o endereço do contexto por parâmetro, lê o que precisar, salva o que precisar e finaliza. A thread de geração é única, então não há necessidade de controle de concorrência no acesso.

### Montagem do Context
Inicialmente, o core lê o schema declarado no arquivo de configuração, e então monta o context no formato necessário. A montagem inclui alocação de contadores, ponteiros para dicionários (arquivos de dados) e ponteiros para pools necessários. **O schema define o que será guardado**

### !ATENÇÃO!
Evitar os seguintes casos: 
❌ Contexto como void* genérico
❌ Contexto com campos públicos acessados direto
❌ Geradores mexendo em estrutura interna
❌ “context->whatever++” espalhado pelo código


## Idéias extras
- Seeds reprodutiveis - permitem gerar o mesmo dataset mais de uma vez
- Distribuições estatísticas personalizadas - 
    - Normalmente pensamos em manter a distribuição dos dados aleatória (uniforme), mas existem outras que podem ser interessantes, como: 
        - Normal
        - Exponencial 
        - Zipf

- Dicionários costumizados 
    - Pensar em uma forma de escrever e adicionar regras condicionais
    Ex: if idade < 18 ? sem CNH : com CNH
        if pais == "Brasil" ? CPF : code

<br>
<br>
<br>
<br>
<br>

1. Modulo core inicia os modulos source e target, além de um buffer que servirá para guardar os dados gerados. Minha idéia é que os dois módulos rodem em threads dedicadas, com o source gerando os dados e jogando para uma lista encadeada, e o target pegando os dados da lista encadeada e enviando em lotes para o banco de dados (preciso de uma especificação de como isso funcionaria precisamente). 

2. Além de iniciar os módulos, o core também lê um arquivo de configuração que quero chamar de Genfile.json (ou outro formato mais conveniente, aceito sugestões), que contêm todas as especificações de configuração necessárias, como formato dos dados (schema), de onde vem (arquivo pré-gerado, hash aleatório), relacionamentos, como serão gerados os dados, etc.

``` C

```


3. Após iniciados os módulos, cada um começa a fazer sua função, rodando em paralelo para uma maior eficiência (sendo sincero, o meu objetivo usando paralelismo é para aprender mesmo, não acho tão necessário) 

4. Após a quantidade de dados ser gerada, a thread source retorna, e após todos estarem salvos, a thread target retorna, e então o módulo core finaliza.

Creio que cada módulo possa ser independente e gerenciar seus próprios recursos, por exemplo, target abre e fecha sua própria conexão, source gerencia seus arquivos abertos, e por aí vai 