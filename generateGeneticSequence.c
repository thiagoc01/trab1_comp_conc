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

#define NBASES 100 // numero de caracteres (bases nucleotidicas)

int main(void) {
  FILE *file;
  char basesArray[] = {'A', 'T', 'C', 'G'};
  int result, i, index;

  file = fopen("in.txt", "wt");

  if (file == NULL) {
      printf("Erro na criacao do arquivo\n");
      return 2;
  }

  for (i = 0; i < NBASES; i++) {
    index = i % 4; // criterio do indice pode ser mudado
    result = fputc(basesArray[index], file);  					  
    if (result == EOF)		    
       printf("Erro na Gravacao\n");
  }

  fclose(file);

  return 0;
}