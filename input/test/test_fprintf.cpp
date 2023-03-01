#include <stdio.h>
#include <stdlib.h>

int main()
{
   FILE * fp = fopen ("file.txt", "a");
   fprintf(fp, "%s %s %s %d\n", "We", "are", "in", 2014);
   fprintf(fp, "%s %s %s %d", "We", "are", "in", 2023);
   
   fclose(fp);

    fp = fopen ("file.txt", "a");
   fprintf(fp, "%s %s %s %d\n", "We", "are", "in", 201448);
   fprintf(fp, "%s %s %s %d", "We", "are", "in", 202339);
   
   return(0);
}