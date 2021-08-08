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
#include "timer.h"

/* Variaveis globais */
long int ocorrencia_letra[26];
long int tamanho = 0; 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
char *nome_arquivo;

/* Funcoes */
void inicializa_vetor_ocorrencia() {
  for (int i = 0 ; i < 26 ; i++)
      ocorrencia_letra[i] = 0;
}

void *calcula_ocorrencia_letra(void *args) {
  FILE *arq = fopen(nome_arquivo, "r");

  long int id = (long int) args;
  long int blocos = tamanho/4;
  long int inicio = blocos * id;
  long int fim;
  long int lido = 0; // Total lido pela thread. Não pode superar o tamanho do bloco.

  char buffer[2048];

  long int ocorrencia_letra_local[26];

  for (int i = 0 ; i < 26 ; i++)
      ocorrencia_letra_local[i] = 0;

  if (id == 3)
      fim = tamanho;

  else
      fim = inicio + blocos;

  fseek(arq, inicio, SEEK_SET);    

  while (ftell(arq) < fim) {
    fgets(buffer, sizeof(buffer), arq);
    for (int i = 0 ; buffer[i] != '\0' && lido < blocos; i++, lido++) {
        for (int j = 0 ; j < 26 ; j++) {
            if (buffer[i] == 'a' + j || buffer[i] == 'A' + j) {
                ocorrencia_letra_local[j] += 1;
                break;
            }
        } 
    }
  }

  pthread_mutex_lock(&lock);
  for (int i = 0 ; i < 26 ; i++) {
      ocorrencia_letra[i] += ocorrencia_letra_local[i];
  }
  pthread_mutex_unlock(&lock);
  fclose(arq);

  pthread_exit(NULL);
}

void inicializa_threads() {
  pthread_t *tids = (pthread_t *) malloc(sizeof(pthread_t) * 4);
  
  for (long int i = 0 ; i < 4 ; i++) {
      if (pthread_create((tids + i), NULL, calcula_ocorrencia_letra, (void *) i)) {
          puts("erro");
          exit(255);
      }
  }

  for (long int i = 0 ; i < 4 ; i++) {
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
  nome_arquivo = argv[1];
  FILE *arq = fopen(nome_arquivo, "r");

  if (!arq) {
      puts("erro");
      return 1;
  }
  char buffer[500];

  fseek(arq, 0, SEEK_END);
  tamanho = ftell(arq);
  printf("%ld\n", tamanho);

  long int ocorrencia_letra_seq[26];

  for (int i = 0 ; i < 26 ; i++) {
      ocorrencia_letra_seq[i] = 0;
  }

  rewind(arq);

  GET_TIME(i);

  while (!feof(arq)) {
    fgets(buffer, sizeof(buffer), arq);
    for (int i = 0 ; buffer[i] != '\0' ; i++) {
        for (int j = 0 ; j < 26 ; j++) {
            if (buffer[i] == 'a' + j || buffer[i] == 'A' + j) {
                ocorrencia_letra_seq[j] += 1;
                break;
            }
        } 
    }
  }
  for (int i = 0 ; i < 26 ; i++) {
      printf("Letra %c : %ld vezes\n", 'a' + i, ocorrencia_letra_seq[i]);
  }

  puts("-----------------------------------------------------------------------------------");
  fclose(arq);

  GET_TIME(f);

  printf("%lf\n", f-i);

  inicializa_vetor_ocorrencia();
  GET_TIME(i);
  inicializa_threads();

  for (int i = 0 ; i < 26 ; i++) {
      printf("Letra %c : %ld vezes\n", 'a' + i, ocorrencia_letra[i]);
  }
  GET_TIME(f);

  printf("%lf\n", f-i);
  
  return 0;
}