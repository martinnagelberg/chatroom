#include <stdio.h>
#define NUM_OF_LINES 200
#define NUM_OF_CHARACTERS 50
 
int main(int argc,char * argv[]){
  
  int i = 0;
  char arr[NUM_OF_LINES][NUM_OF_CHARACTERS]; 
  FILE * fp;
 
  fp = fopen("input.txt","r"); // tenes que tener el archivo ese en el mismo path
  if(fp == NULL) {
      printf("Unable to open file");
  }
 
  while(fgets(&arr[i][0], NUM_OF_CHARACTERS, fp) != NULL)
  i++;

  // Aca podes imprimirte todas las lineas que quieras
  printf("%s", arr[0]);
  printf("%s", arr[1]);

return 0;
 
}
