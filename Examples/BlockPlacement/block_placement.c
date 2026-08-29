#include <stdio.h>
#include <stdlib.h>

int check(int n) {
  if (n < 0) {
    printf("Greska: negativna vrednost!\n");
    exit(1);
  }

  return n * 2;
}

int sum(int n) {
  int s = 0;

  for (int i = 0; i < n; i++) {
    s = s + check(i);
  }

  return s;
}

int process(int *a, int n) {
  int s = 0;

  for (int i = 0; i < n; i++) {
    if (a[i] < 0) {
      printf("Greska: neispravan element!\n");
      abort();
    }

    s = s + a[i];
  }

  return s;
}

int main() {
  int a[5] = {1, 2, 3, 4, 5};

  printf("sum(10) = %d\n", sum(10));
  printf("process(a, 5) = %d\n", process(a, 5));

  return 0;
}
