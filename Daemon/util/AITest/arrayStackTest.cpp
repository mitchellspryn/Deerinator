#include <stdlib.h>
#include <stdio.h>
#include "AI/ArrayStack.hpp"

int main(void)
{
  ArrayStack<double> A = ArrayStack<double>(10);

  fprintf(stdout, "Pushing back first set of data...\n");
  for (int i = 0; i < 10; i++)
  {
    A.push((double)i / 3.);
  }

  fprintf(stdout, "First set: ");
  double *data = A.data();
  for (int i = 0; i < 10; i++)
  {
    fprintf(stdout, "%lf ", *(data + i));
  }
  fprintf(stdout, "\n");

  A.reset();

  fprintf(stdout, "Pushing back second set of data...\n");
  for (int i = 10; i < 20; i++)
  {
    A.push((double)i / 3.);
  }

  fprintf(stdout, "Second set: ");

  data = A.data();
  for (int i = 0; i < 10; i++)
  {
    fprintf(stdout, "%lf ", *(data + i));
  }
  fprintf(stdout, "\n");

  fprintf(stdout, "Done!\n");
  return 0;
}
