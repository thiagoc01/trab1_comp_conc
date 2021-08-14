/*
  * Programa: Algoritmo concorrente para encontrar as ocorrências de um codon em uma sequencia de RNA
  * Alunos: Gabriele Jandres Cavalcanti e Thiago Figueiredo Lopes de Castro | DREs: 119159948 e 118090044
  * Disciplina: Computacao Concorrente - 2021.1
  * Modulo 1 - Trabalho 1
  * Data: Agosto de 2021
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "./libs/timer.h"

/* Variaveis globais */
long int ocorrencia_bases[4];
long int tamanho = 0;
int num_threads;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
char *nome_arquivo;

/* Funcoes */

void inicializa_ocorrencia_bases()
{
    for (int i = 0 ; i < 4 ; i++)
        ocorrencia_bases[i] = 0;
}

void *calcula_ocorrencia_bases(void *args) {
  FILE *arq = fopen(nome_arquivo, "r");

  long int id = *(long int *) args;
  long int blocos = tamanho / num_threads;
  long int inicio = id * blocos;
  long int fim;
  long int lido = 0;

  if (id == num_threads - 1)
    fim = tamanho;
  else
    fim = inicio + blocos;

  char buffer[512];

  long int ocorrencia_bases_local[4];

  for (int i = 0 ; i < 4 ; i++)
    ocorrencia_bases_local[i] = 0;

  int posicao;
  char *aux;

    fseek(arq, inicio, SEEK_CUR);

  while (ftell(arq) < fim)
  {
      fgets(buffer, sizeof(buffer), arq);

      for (long int i = 0 ; buffer[i] != '\0' && lido < (fim - inicio) ; i++, lido++)
      {
          if (buffer[i] == 'A')
            ocorrencia_bases_local[0]++;
          else if (buffer[i] == 'C')
            ocorrencia_bases_local[1]++;
          else if (buffer[i] == 'G')
            ocorrencia_bases_local[2]++;
          else if (buffer[i] == 'T')
            ocorrencia_bases_local[3]++;
      }
  }


  pthread_mutex_lock(&lock);

  for (int i = 0 ; i < 4 ; i++)
      ocorrencia_bases[i] += ocorrencia_bases_local[i];

  pthread_mutex_unlock(&lock);
  fclose(arq);
  pthread_exit(NULL);
}

void inicializa_threads() {
  pthread_t *tids = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);
  long int args[num_threads];

  for (long int i = 0 ; i < num_threads ; i++) {
        args[i] = i;
      if (pthread_create((tids + i), NULL, calcula_ocorrencia_bases, (void *) &args[i])) {
          puts("erro");
          exit(255);
      }
  }

  for (long int i = 0 ; i < num_threads ; i++) {
      if (pthread_join(*(tids + i), NULL)) {
          puts("erro");
          exit(255);
      }
  }
  free(tids);
}

// Funcao principal
int main(int argc, char **argv)
 {
  double i,f;

  if (argc < 3)
  {
    printf("Digite %s <caminho do arquivo> <número de threads>.\n", argv[0]);
    return 1;
  }

  nome_arquivo = argv[1];
  num_threads = atoi(argv[2]);

  if (num_threads == 0)
  {
      puts("Digite um número válido (maior que 0).");
      return 2;
  }

  FILE *arq = fopen(nome_arquivo, "r");

  if (!arq) {
      puts("erro");
      return 3;
  }

  fseek(arq, 0, SEEK_END);
  tamanho = ftell(arq);
  printf("%ld\n", tamanho);

  long int ocorrencia_bases_seq[4];

  for (int i = 0 ; i < 4 ; i++)
    ocorrencia_bases_seq[i] = 0;

  char buffer[512];

  rewind(arq);

  // sequencial 
  GET_TIME(i);

  while (!feof(arq)) {
    fgets(buffer, sizeof(buffer), arq);

     for (long int i = 0 ; buffer[i] != '\0' ; i++)
      {
          switch (buffer[i])
          {
            case 'A':
                ocorrencia_bases_seq[0]++;
                break;
            case 'C':
                ocorrencia_bases_seq[1]++;
                break;
            case 'G':
                ocorrencia_bases_seq[2]++;
                break;
            case 'T':
                ocorrencia_bases_seq[3]++;
                break;
          }
      }

  }

  printf("Ocorrências:\n A: %ld vezes\tC: %ld vezes\tG: %ld vezes\tT: %d vezes", ocorrencia_bases_seq[0], ocorrencia_bases_seq[1], ocorrencia_bases_seq[2], ocorrencia_bases_seq[3]);

  puts("\n-----------------------------------------------------------------------------------");
  fclose(arq);

  GET_TIME(f);

  printf("Tempo sequencial: %lf\n", f-i);

  puts("-----------------------------------------------------------------------------------");

  inicializa_ocorrencia_bases();

  GET_TIME(i);

  inicializa_threads();

  printf("Ocorrências:\n A: %ld vezes\tC: %ld vezes\tG: %ld vezes\tT: %d vezes\n", ocorrencia_bases[0], ocorrencia_bases[1], ocorrencia_bases[2], ocorrencia_bases[3]);

  GET_TIME(f);

  printf("Tempo concorrente: %lf\n", f-i);

  return 0;
}
