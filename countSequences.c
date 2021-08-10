/*
  * Programa: Algoritmo concorrente para encontrar as ocorrências de uma string num arquivo de texto
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
long int ocorrencia_sequencia = 0;
long int tamanho = 0;
int num_threads;
int tamanho_sequencia;
char *sequencia_alvo;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
char *nome_arquivo;

/* Funcoes */

void *calcula_ocorrencia_sequencia(void *args) {
  FILE *arq = fopen(nome_arquivo, "r");

  long int id = (long int) args;

  char *buffer = (char *) malloc(sizeof(char) * tamanho);

  long int ocorrencia_sequencia_local = 0;

  while (!feof(arq))
  {
      fgets(buffer, tamanho, arq);

      long int i, j, aux, tam_buffer = strlen(buffer);
      int incompativel;

      for (i = id ; i < tam_buffer ; i += num_threads)
      {
            incompativel = 0;
            aux = i;

            for (j = 0; j < tamanho_sequencia ; j++)
            {
                if (sequencia_alvo[j] == buffer[aux])
                    aux++;
                else {
                    incompativel = 1;
                    break;
                }
            }
            if (incompativel == 0)
                ocorrencia_sequencia_local += 1;

      }

  }


  pthread_mutex_lock(&lock);

  ocorrencia_sequencia += ocorrencia_sequencia_local;

  pthread_mutex_unlock(&lock);
  fclose(arq);
  free(buffer);
  pthread_exit(NULL);
}

void inicializa_threads() {
  pthread_t *tids = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);

  for (long int i = 0 ; i < num_threads ; i++) {
      if (pthread_create((tids + i), NULL, calcula_ocorrencia_sequencia, (void *) i)) {
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
int main(int argc, char **argv) {
  double i,f;
  if (argc < 4)
  {
      printf("Digite %s <nome do arquivo> <sequência desejada> <número de threads>.", argv[0]);
      return 1;
  }
  nome_arquivo = argv[1];
  sequencia_alvo = argv[2];
  num_threads = atoi(argv[3]);

  if (num_threads == 0)
  {
      puts("Digite um número válido (maior que 0).");
      return 2;
  }
  tamanho_sequencia = strlen(sequencia_alvo);

  FILE *arq = fopen(nome_arquivo, "r");

  if (!arq) {
      puts("erro");
      return 1;
  }

  fseek(arq, 0, SEEK_END);
  tamanho = ftell(arq);
  printf("%ld\n", tamanho);

  long int ocorrencia_sequencia_seq = 0;

  char *buffer = (char *) malloc(sizeof(char) * tamanho);

  if (!buffer)
  {
      puts("erro buffer");
      return 2;
  }

  rewind(arq);

  GET_TIME(i);

  int posicao = 0;
  char *aux;

  while (!feof(arq)) {
    fgets(buffer, tamanho, arq);

    posicao = 0;

    while ((aux = strstr(buffer + posicao, sequencia_alvo)) != NULL)
    {
        posicao = (aux - buffer) + 1;
        ocorrencia_sequencia_seq++;
    }

  }

    printf("Sequencia %s : %ld vezes\n", sequencia_alvo, ocorrencia_sequencia_seq);

  puts("-----------------------------------------------------------------------------------");
  fclose(arq);

  GET_TIME(f);

  printf("%lf\n", f-i);

  GET_TIME(i);
  inicializa_threads();


  printf("Sequencia %s : %ld vezes\n", sequencia_alvo, ocorrencia_sequencia);

  GET_TIME(f);

  printf("%lf\n", f-i);

  free(buffer);

  return 0;
}
