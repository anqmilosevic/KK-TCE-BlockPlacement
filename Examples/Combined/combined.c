#include <stdio.h>
#include <stdlib.h>

long long factorial(int n, long long acc) {
  if (n < 0) {
    printf("Greska: negativan argument!\n");
    exit(1);
  }

  if (n <= 1) {
    return acc;
  }

  return factorial(n - 1, n * acc);
}

int main() {
  printf("factorial(20, 1) = %lld\n", factorial(20, 1));

  return 0;
}
