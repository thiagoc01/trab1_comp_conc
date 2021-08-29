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
#include <sys/stat.h>
#include <stdbool.h>
#include "./libs/timer.h"

/* Variaveis globais */
long int size = 0;  // tamanho do arquivo de entrada
int num_threads; // numero de threads
bool same_char; // indica se a sequencia tem todos os caracteres iguais
char *file_name; // nome do arquivo de entrada
long int total_occurrences = 0; // total de ocorrencias da sequencia
int sequence_size; // tamanho da sequencia alvo
char *wanted_sequence; // sequencia alvo
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/* Funcoes */
// Funcao para verificar se a sequencia tem todos os caracteres iguais
int check_same_char(char *s) {
  char first_char = wanted_sequence[0];

  while (*s != '\0') {
    if (*s != first_char)
      return 0;
    s++;
  }

  return 1;
}

// Funcao para tratar a quebra de uma sequencia devido à quebra pelo buffer
void treat_buffer_break(bool can_find_sequence, int last_position_sequence, FILE *file, long int *total_occurrences_local) {
  int buffer_position = 0;
  char aux[last_position_sequence * sequence_size + 1]; // buffer auxiliar para guardar o começo do outro bloco
  aux[0] = '\0';

  if (!same_char) {
    if (!can_find_sequence) { 
      fgets(aux, sequence_size - last_position_sequence + 1, file);
      buffer_position = 0;

      while (aux[buffer_position] != '\0' && wanted_sequence[last_position_sequence] != '\0') {
        if (aux[buffer_position] != wanted_sequence[last_position_sequence])
          return;

        last_position_sequence++;
        buffer_position++;

        if (last_position_sequence == sequence_size)
          (*total_occurrences_local)++;
      }
    }
    return ;
  }

  else {
    if (!can_find_sequence) {
      int i = last_position_sequence;
      fpos_t pos;
      char c;
      fgetpos(file, &pos);

      while ((c = fgetc(file)) == wanted_sequence[0])
        i++;

      fsetpos(file, &pos);

      if (i % sequence_size != 0)
        return;

      fgets(aux, sequence_size - last_position_sequence + 1, file);
      buffer_position = 0;

      while (aux[buffer_position] != '\0' && wanted_sequence[last_position_sequence] != '\0') {
        if (aux[buffer_position] != wanted_sequence[last_position_sequence])
          return;

        last_position_sequence++;
        buffer_position++;

        if (last_position_sequence == sequence_size) {
          (*total_occurrences_local)++;
        }
      }
    }
    return ;
  }
}

// Funcao que as threads vao executar para calcular o total de ocorrencias da sequencia no arquivo
void *calculate_total_occurrences(void *args) {
  FILE *file = fopen(file_name, "r");
  long int id = (long int) args; // identificador da thread
  long int block_size = size / num_threads; // tamanho de cada bloco da thread
  long int start = id * block_size; // inicio do bloco de cada thread
  long int total_occurrences_local = 0; // total de ocorrencias na sequencia no bloco da thread
  char *buffer; // buffer de leitura do arquivo
  long int total_read = 0; // total lido pela thread. Não pode superar block_size.
  long int buffer_position = 0, last_position_sequence = 1;
  char *actual_char, *next_char;

  // Se for a ultima thread o tamanho do bloco eh diferente (pode ser maior ou menor)
  if (id == num_threads - 1) {
    block_size = size - start; 
  } 
  
  buffer = (char *) malloc(sizeof(char) * block_size + 1); // aloca de acordo com o tamanho e soma mais um para o caractere nulo.
  if (!buffer) {
    printf("Erro de alocacao para o buffer\n");
    exit(1);
  }

  fseek(file, start, SEEK_SET); // aponta para o inicio do bloco da thread

  // Nao posso estar procurando por somente 1 char nesse caso
  if (sequence_size > 1) {
    fgets(buffer, block_size + 1, file);
    actual_char = strchr(buffer, wanted_sequence[0]);
    bool can_find_sequence = false; // Se can_find_sequence = true, então, ao chegar no final do buffer, tudo foi lido corretamente e a sequência não está partida entre dois blocos.

    while (actual_char) { // Enquanto encontrar a primeira letra da sequência no buffer, há a possibilidade de haver uma sequência
      total_read = (actual_char - buffer) + 1; // O caractere lido não é incluído no cálculo, por estar no endereço atual.
      buffer_position = 1;

      if (total_read <= block_size) {
        next_char = actual_char + 1; // Se não houver sequência a partir dessa letra encontrada, então a próxima letra será a adjacente à ela.
        last_position_sequence = 1;
        can_find_sequence = false; // Reinicia can_find_sequence

        while (total_read <= block_size && actual_char[buffer_position] != '\0' && wanted_sequence[last_position_sequence] != '\0') {
          if (actual_char[buffer_position] != wanted_sequence[last_position_sequence]) {
            can_find_sequence = true; // Indica que completou a leitura da sequência.
            break;
          }
          buffer_position++;
          last_position_sequence++;
          total_read++;

          if (last_position_sequence == sequence_size) {
            total_occurrences_local++;
            next_char = actual_char + sequence_size; // Pular todas as letras depois da primeira letra encontrada
            can_find_sequence = true;
          }
        }

        if (actual_char[buffer_position] == '\0')
          treat_buffer_break(can_find_sequence, last_position_sequence, file, &total_occurrences_local);
      }
      actual_char = strchr(next_char, wanted_sequence[0]);
    }
  }

  else {
    fgets(buffer, block_size + 1, file);
    int pos = 0;
    while ((actual_char = strchr(buffer + pos, wanted_sequence[0])) && total_read < block_size) {
      total_occurrences_local++;
      pos = (actual_char - buffer) + 1;
      total_read++;
    }
  }

  // Sincronizacao por exclusao mutua porque vamos alterar a variavel global
  pthread_mutex_lock(&lock);
  total_occurrences += total_occurrences_local;
  pthread_mutex_unlock(&lock);

  fclose(file);
  free(buffer);
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
    if (pthread_create((*tids) + i, NULL, calculate_total_occurrences, (void *) i)) {
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

// Funcao sequencial para calcular o total de ocorrencias da sequencia no arquivo
void calculate_total_occurrences_seq(long int *total_occurrences_seq) {
  int a = 0, b;
  char *aux, *next;

  // Abertura do arquivo para leitura
  FILE *file = fopen(file_name, "r");
  if (!file) {
    printf("Erro ao abrir o arquivo para leitura");
    exit(3);
  }

  // Calculo do tamanho do arquivo
  struct stat st;
  int fd = fileno(file);
  fstat(fd, &st);
  size = st.st_size;

  if (size < sequence_size) {
    printf("Sequencia eh maior que o arquivo.\n");
    exit(4);
  }

  if (size / num_threads <= sequence_size) {
    num_threads = 1;
    while (size / num_threads > sequence_size)
      num_threads++;
  }

  char *buffer = (char *) malloc(sizeof(char) * size + 1);
  if (!buffer) {
    printf("Erro de alocacao para o buffer\n");
    exit(1);
  }
  
  fgets(buffer, size + 1, file);

  if (sequence_size > 1) {
    aux = strchr(buffer, wanted_sequence[0]);
    while (aux) {
      next = aux + 1;
      b = 1;
      a = 1;

      while (aux[a] != '\0' && wanted_sequence[b] != '\0') {
        if (aux[a] != wanted_sequence[b])
          break;
        a++;
        b++;

        if (b == sequence_size) {
          (*total_occurrences_seq)++;
          next = aux + sequence_size;
          break;
        }
      }
      aux = strchr(next, wanted_sequence[0]);
    }
  }
  else {
    int pos = 0;
    while ((aux = strchr(buffer + pos, wanted_sequence[0]))) {
      (*total_occurrences_seq)++;
      pos = (aux - buffer) + 1;
    }
  }

  fclose(file);
  free(buffer);
}

// Mostra o tempo total de execucao de uma tarefa
void show_task_time(double start) {
  double end;
  GET_TIME(end);
  printf("Tempo total: %lf s\n\n", end - start);
}

// Imprime a ocorrencia da sequencia
void print_sequence_occurrences(char version[], char wanted_sequence[], long int total_occurrences, char file_name[]) {
  printf("--- VERSAO %s --- \n", version);
  printf("Sequencia %s foi encontrada %ld vezes no arquivo %s\n", wanted_sequence, total_occurrences, file_name);
}

// Funcao que checa a corretude da solucao concorrente
void analyze_output_correctness(long int sequence_occurrences, long int sequence_occurrences_seq) {
  if (sequence_occurrences == sequence_occurrences_seq) {
    printf("A contagem esta correta. \n");
  } else {
    printf("A contagem esta incorreta. Sequencial achou %ld e concorrente achou %ld \n", sequence_occurrences_seq, sequence_occurrences);
  }
}

// Funcao principal
int main(int argc, char **argv) {
  double start;
  long int total_occurrences_seq = 0;
  pthread_t *tids; // identificadores das threads no sistema

  // Verificacao dos parametros da linha de comando
  if (argc < 3) {
    printf("Digite %s <arquivo de entrada> <numero de threads> <sequencia a ser procurada>.\n", argv[0]);
    return 1;
  }

  // Atribuicao dos parametros
  file_name = argv[1];
  num_threads = atoi(argv[2]);
  wanted_sequence = argv[3]; // a variavel recebera algum valor se o usuario passar

  // Validacao do numero de threads a serem utilizadas na solucao concorrente
  if (num_threads == 0) {
    puts("Digite um numero valido (maior que 0).");
    return 2;
  }

  sequence_size = strlen(wanted_sequence);
  same_char = check_same_char(wanted_sequence);

  // Sequencial
  GET_TIME(start);
  calculate_total_occurrences_seq(&total_occurrences_seq);
  print_sequence_occurrences("SEQUENCIAL", wanted_sequence, total_occurrences_seq, file_name);
  show_task_time(start);

  // Concorrente
  GET_TIME(start);
  create_threads(&tids, num_threads);
  join_threads(tids, num_threads);
  print_sequence_occurrences("CONCORRENTE", wanted_sequence, total_occurrences, file_name);
  show_task_time(start);

  // Corretude
  analyze_output_correctness(total_occurrences, total_occurrences_seq);

  return 0;
}