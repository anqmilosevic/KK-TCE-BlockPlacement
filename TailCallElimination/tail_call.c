#include <stdio.h>

int factorial(int n, int acc) {
  if (n <= 1) {
    return acc;
  }

  return factorial(n - 1, n * acc);
}

int gcd(int a, int b) {
  if (b == 0) {
    return a;
  }

  if (a > b) {
    return gcd(a - b, b);
  }

  return gcd(a, b - a);
}

void countdown(int n) {
  if (n == 0) {
    return;
  }

  printf("%d ", n);
  countdown(n - 1);
}

int fibonacci(int n) {
  if (n < 2) {
    return n;
  }

  return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
  printf("factorial(10, 1) = %d\n", factorial(10, 1));
  printf("gcd(48, 18) = %d\n", gcd(48, 18));
  countdown(5);
  printf("\n");
  printf("fibonacci(10) = %d\n", fibonacci(10));

  return 0;
}
