/*
  * Programa: Algoritmo para gerar um arquivo de texto com simulacao de um sequenciamento genetico
  * Alunos: Gabriele Jandres Cavalcanti e Thiago Figueiredo Lopes de Castro | DREs: 119159948 e 118090044
  * Disciplina: Computacao Concorrente - 2021.1
  * Modulo 1 - Trabalho 1 - Complemento
  * Data: Agosto de 2021
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NBASES 10000000

int main(void) {
    srand(time(NULL));
  FILE *file;
  char basesArray[] = {'A', 'T', 'C', 'G'};
  int result, i, index;

  file = fopen("in.txt", "wt");

  if (file == NULL) {
      printf("Erro na criacao do arquivo\n");
      return 2;
  }

  char last_generated = ' '; // Guarda a última base nitrogenada gerada
  int counter_repeated_occurences = 0; // Conta o número de bases nitrogenadas iguais seguidas se ocorrer repetição. Máximo de 4 vezes

  for (i = 0; i < NBASES; i++) {
    index = rand() % 4;

    if (last_generated == basesArray[index] && counter_repeated_occurences == 3) // Limite de 4 bases nitrogenadas iguais seguidas
    {
        while (last_generated == basesArray[index]) // Continua até gerar algo diferente
            index = rand() % 4;
        counter_repeated_occurences = 0; // Reinicia o contador
    }

    else if (last_generated == basesArray[index])
        counter_repeated_occurences++;
    else
      counter_repeated_occurences = 0; // Gerou uma nova base nitrogenada da última e essa não havia chegado no limite

    last_generated = basesArray[index];
    result = fputc(last_generated, file);

    if (result == EOF)
       printf("Erro na Gravacao\n");
  }

  fclose(file);

  return 0;
}
