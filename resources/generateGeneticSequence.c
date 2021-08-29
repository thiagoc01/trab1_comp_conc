/*
  * Programa: Algoritmo para gerar um arquivo de texto com simulacao de um sequenciamento de DNA
  * Alunos: Gabriele Jandres Cavalcanti e Thiago Figueiredo Lopes de Castro | DREs: 119159948 e 118090044
  * Disciplina: Computacao Concorrente - 2021.1
  * Modulo 1 - Trabalho 1 - Complemento
  * Data: Agosto de 2021
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NBASES 1000

int main(void) {
  FILE *file;
  char bases_array[] = {'A', 'T', 'C', 'G'}; // array de bases nitrogenadas que formam o DNA
  int result, i, index, counter = 0; // counter conta o numero de bases iguais seguidas se ocorrer repetição. Maximo de 4 vezes
  char last_generated = ' '; // guarda a ultima base nitrogenada gerada 

  srand(time(NULL));

  file = fopen("../in/in.txt", "wt");

  if (file == NULL) {
    printf("Erro na criacao do arquivo\n");
    return 2;
  }

  for (i = 0; i < NBASES; i++) {
    index = rand() % 4;

    if (last_generated == bases_array[index] && counter == 3) { // limite de 4 bases nitrogenadas iguais seguidas 
      while (last_generated == bases_array[index]) // continua ate gerar algo diferente
        index = rand() % 4;
      counter = 0; // reinicia o contador
    }
    else if (last_generated == bases_array[index])
      counter++;
    else
      counter = 0; // gerou uma nova base da ultima e essa não havia chegado no limite

    last_generated = bases_array[index];
    result = fputc(last_generated, file);

    if (result == EOF)
      printf("Erro na Gravacao\n");
  }

  fclose(file);

  return 0;
}
