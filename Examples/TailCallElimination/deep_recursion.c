#include <stdio.h>

long long sum(int n, long long acc) {
  if (n == 0) {
    return acc;
  }

  return sum(n - 1, acc + n);
}

int main() {
  printf("sum(1000000, 0) = %lld\n", sum(1000000, 0));

  return 0;
}
