# Computação Concorrente | Trabalho 01 | 2021.1
*Solução concorrente para o problema de contar as ocorrências de caracteres e sequências em um arquivo de texto.*

## Tabela de Conteúdo

1. [Tecnologias utilizadas](#tecnologias-utilizadas)
2. [Estrutura do repositório](#estrutura-do-repositório)
3. [Como gerar uma sequência de entrada?](#como-gerar-uma-sequência-de-entrada)
4. [Como usar o programa?](#como-usar-o-programa)
5. [Autores](#autores)

## 🖥️ Tecnologias utilizadas
O projeto foi desenvolvido utilizando a linguagem C e para a compilação recomenda-se o uso do GCC (GNU Compiler Collection).

![Badge](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)

## 📂 Estrutura do repositório
O repositório é composto pelos subdiretórios:
* **in**: conjunto de arquivos de entrada
* **libs**: bibliotecas e macros externas necessárias para cálculo do desempenho
* **resources**: códigos auxiliares para o programa principal

Além disso, o repositório conta com os dois arquivos principais:
* **countCharOccurrences**: código fonte responsável por contar as ocorrências de um caractere específico ou de todos os caracteres presentes em um arquivo de entrada
* **countSequences**: código fonte responsável por contar as ocorrências de determinada sequência em um arquivo de entrada

## 📜 Como gerar uma sequência de entrada?
Caso queira utilizar como entrada um sequenciamento de DNA diferente do que se encontra nesse repositório, você pode utilizar o programa *generateGeneticSequence*, que se encontra no subdiretório resources. Para editar o tamanho do arquivo a ser gerado, basta alterar a macro NBASES com o valor desejado e seguir o passo a passo abaixo:

1. Compile o programa
```
  gcc generateGeneticSequence.c -o genSeq
```

2. Execute o programa
```
  ./genSeq
```
O arquivo de entrada gerado estará em *in/in.txt*.

## 🤔 Como usar o programa?

1.  Clone esse repositório
```
  git clone https://github.com/thiagoc01/trab1_comp_conc.git
```

2. Compile o programa
```
  gcc countCharOccurrences.c -o countChar -lpthread
  gcc countSequences.c -o countSeq -lpthread
```

3. Execute o programa

Para o problema de contagem de caracteres no arquivo de entrada, utilize:
```
  ./countChar <arquivo de entrada> <numero de threads>
```

Caso queira descobrir a quantidade de um caractere específico, um último paramêtro opcional pode ser passado. Utilize:
```
  ./countChar <arquivo de entrada> <numero de threads> <caractere a ser procurado>
```

Para o problema de contagem de sequências no arquivo de entrada, utilize:
```
   ./countSeq <arquivo de entrada> <numero de threads> <sequencia a ser procurada>
```

## 👩‍💻 Autores
* Gabriele Jandres Cavalcanti
* Thiago Figueiredo Lopes de Castro
