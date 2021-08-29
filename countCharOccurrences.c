/*
  * Programa: Algoritmo concorrente para encontrar a ocorrência de caracteres em um arquivo texto
  * Alunos: Gabriele Jandres Cavalcanti e Thiago Figueiredo Lopes de Castro | DREs: 119159948 e 118090044
  * Disciplina: Computacao Concorrente - 2021.1
  * Modulo 1 - Trabalho 1
  * Data: Agosto de 2021
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include "./libs/timer.h"

#define SIZE_BUFFER 512

/* Variaveis globais */
long int char_occurrences[26]; // quantidade de cada caractere no arquivo
long int size = 0; // tamanho do arquivo de entrada
int nthreads; // numero de threads
char *file_name; // nome do arquivo de entrada
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // variavel para exclusao mutua

/* Funcoes */

// Inicializa o array de ocorrencias de caracteres
void initialize_char_occurrences(long int *array) {
  for (int i = 0; i < 26; i++) {
    *(array + i)= 0;
  }
}

// Conta a quantidade de caracteres de modo sequencial
void count_char_occurrences_seq(long int *char_occurrences_seq, char file_name[]) {
  char buffer[SIZE_BUFFER]; // buffer de leitura do arquivo

  // Abertura do arquivo de entrada para leitura
  FILE *file = fopen(file_name, "r");
  if (!file) {
      printf("Erro ao abrir o arquivo de entrada. \n");
      exit(3);
  }
  fseek(file, 0, SEEK_END);
  size = ftell(file);
  rewind(file);

  // Contagem dos caracteres enquanto nao chega no fim do arquivo
  while (!feof(file)) {
    fgets(buffer, sizeof(buffer), file);
    for (long int i = 0 ; buffer[i] != '\0' ; i++) {
      for (int j = 0 ; j < 26 ; j++) {
        if (buffer[i] == 'a' + j || buffer[i] == 'A' + j) {
          *(char_occurrences_seq +j) += 1;
          break;
        }
      } 
    }
  }

  fclose(file);
}

// Imprime a ocorrencia de cada caractere, se a ocorrencia do caractere correspondente nao for nula
void print_char_occurrences(long int *array, char version[], char char_wanted) {
  if (char_wanted == ' ') {
    printf("--- VERSAO %s --- \n-- Ocorrencias de caracteres no arquivo %s -- \n", version, file_name);
    for (int i = 0; i < 26; i++) {
      if(*(array + i) != 0) printf("%c: %ld vezes \n", 'A' + i, *(array + i));
    }
  } else {
    int index = (char_wanted - 65); // indice do caractere no array de ocorrencias
    printf("--- VERSAO %s --- \nO caractere %c foi encontrado %ld vezes no arquivo %s fornecido. \n", version, char_wanted, *(array + index), file_name);
  }
}

// Mostra o tempo total de execucao de uma tarefa
void show_task_time(double start) {
  double end;
  GET_TIME(end);
  printf("Tempo total: %lf s\n\n", end - start);
}

// Funcao que as threads irao executar para contar a quantidade de caracteres de modo concorrente
void *count_char_occurrences(void *arg) {
  long int id = (long int) arg; // identificador da thread
  char buffer[SIZE_BUFFER]; // buffer de leitura do arquivo
  long int char_occurrences_local[26]; // array local para controle de caracteres em cada bloco das threads

  // Inicializa o array de ocorrencias local de cada thread
  for (int i = 0; i < 26; i++) {
    char_occurrences_local[i] = 0;
  }

  // Leitura do arquivo de entrada
  FILE *file = fopen(file_name, "r");

  while (!feof(file)) {
    fgets(buffer, sizeof(buffer), file);
    int size_buffer = strlen(buffer);

    for (long int i = id ; i < size_buffer; i += nthreads) {
      for (int j = 0 ; j < 26 ; j++) {
        if (buffer[i] == 'a' + j || buffer[i] == 'A' + j) {
            char_occurrences_local[j] += 1;
            break;
        }
      }
    }
  }
  fclose(file);

  // Garantia de exclusao mutua para contagem correta
  pthread_mutex_lock(&lock);
  for (int i = 0; i < 26; i++) {
    char_occurrences[i] += char_occurrences_local[i];
  }
  pthread_mutex_unlock(&lock);

  pthread_exit(NULL);
}

// Funcao para criar as threads
void create_threads(pthread_t **tids, int num_threads) {
  // Aloca memoria para as estruturas de dados
  *tids = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);
  if (*tids == NULL) {
    printf("Erro na alocacao de memoria para tids \n");
    exit(2);
  }

  // Cria as threads
  for (long int i = 0 ; i < num_threads ; i++) {
    if (pthread_create((*tids) + i, NULL, count_char_occurrences, (void *) i)) {
      printf("Erro na criacao das threads \n");
      exit(3);
    }
  }
}

// Funcao para aguardar o termino das threads
void join_threads(pthread_t *tids, int num_threads) {
  for (long int i = 0 ; i < num_threads ; i++) {
    if (pthread_join(*(tids + i), NULL)) {
      printf("Erro no join das threads \n");
      exit(4);
    }
  }
  free(tids);
}

// Funcao que checa a corretude da solucao concorrente
void analyze_output_correctness(long int *char_occurrences, long int *char_occurrences_seq) {
  for (int i = 0; i < 26; i++) {
    if (*(char_occurrences_seq + i) != *(char_occurrences + i)) {
      printf("A contagem esta incorreta. O esperado para o caractere %c eh %ld e o encontrado foi %ld \n", 'A' + i, *(char_occurrences_seq + i), *(char_occurrences + i));
      break;
    }
  }
  printf("A contagem esta correta. \n");
}

// Funcao principal
int main(int argc, char **argv) {
  double start; // variaveis para controle do tempo
  char wanted_char; // caractere de interesse a ser buscado no arquivo de entrada
  long int char_occurrences_seq[26]; // armazena a quantidade de cada caractere no arquivo para a versao sequencial
  pthread_t *tids; // identificadores das threads no sistema

  // Verificacao dos parametros da linha de comando
  if (argc < 3) {
    printf("Digite %s <arquivo de entrada> <numero de threads> <caractere a ser procurado (opcional)>.\n", argv[0]);
    return 1;
  }

  // Atribuicao dos parametros
  file_name = argv[1];
  nthreads = atoi(argv[2]);
  wanted_char = argv[3] ? toupper(argv[3][0]) : ' '; // a variavel recebera algum valor se o usuario passar

  // Validacao do numero de threads a serem utilizadas na solucao concorrente
  if (nthreads == 0) {
    printf("Digite um numero de threads valido (maior que 0). \n");
    return 2;
  }

  // Sequencial 
  initialize_char_occurrences(char_occurrences_seq);
  GET_TIME(start);
  count_char_occurrences_seq(char_occurrences_seq, file_name);
  print_char_occurrences(char_occurrences_seq, "SEQUENCIAL", wanted_char);
  show_task_time(start);

  // Concorrente
  initialize_char_occurrences(char_occurrences);
  GET_TIME(start);
  create_threads(&tids, nthreads);
  join_threads(tids, nthreads);
  print_char_occurrences(char_occurrences, "CONCORRENTE", wanted_char);
  show_task_time(start);

  // Checa a corretude da solução
  analyze_output_correctness(char_occurrences, char_occurrences_seq);

  return 0;
}