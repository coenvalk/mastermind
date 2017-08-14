// Guess Mastermind code

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

char** create_all(int length, int colors, int n) { // creates list of possible combinations
  char** S = malloc(n * sizeof(char*));
  char* first = malloc(length * sizeof(char));
  int i;
  for (i = 0; i < length; i++) {
    first[i] = 0;
  }
  S[0] = first;
  for (i = 1; i < n; i++) {
    char* now = malloc(length * sizeof(char));
    int j;
    for (j = 0; j < length; j++) {
      now[j] = S[i-1][j];
    }
    now[length-1]++;
    for (j = length-1; j > 0; j--) {
      if (now[j] >= colors) {
	now[j] = 0;
	now[j-1]++;
      } else {
	break;
      }
    }
    S[i] = now;
  }
  return S;
}

void print_one(char* now, int length) {
  int i;
  for (i = 0; i < length; i++) {
    printf("%d", (int) now[i]);
  }
}

void print_all(char** S, int n, int length) { // Prints all current possibilities
  int i;
  for (i = 0; i < n; i++) {
    print_one(S[i], length);
    printf(", ");
  }
  printf("\n");
}

bool isin(char x, char* code, int length) {
  int i;
  for (i = 0; i < length; i++) {
    if (code[i] == x) return true;
  }
  return false;
}

int howmany(char x, char* code, int length) {
  int r = 0;
  int i;
  for (i = 0; i < length; i++) {
    if (code[i] == x) r++;
  }
  return r;
}

int* analyze(char* code, char* guess, int length, int colors) {
  int i;
  int c = 0;
  int p = 0;
  for (i = 0; i < colors; i++) {
    int gue = howmany(i, guess, length);
    int cod = howmany(i, code, length);
    if (cod > gue)
      c += gue;
    else
      c += cod;
  }
  for (i = 0; i < length; i++) {
    if (isin(guess[i], code, length)) {
      if (guess[i] == code[i]) {
	p++;
      }
    }
  }
  int* r = malloc(sizeof(int) * 2);
  r[0] = c-p;
  r[1] = p;
  return r;
}

int reduce(char** S, char* now, int c, int p, int length, int colors, int n) {
  int x = 0;
  int i;
  for (i = 0; i < n; i++) {
    int* r = analyze(S[i], now, length, colors);
    if (r[0] == c && r[1] == p) x++;
    free(r);
  }
  return x;
}

int fullReduce(char** S, char* now, int length, int colors, int n) {
  int responses[13][2] = {{0, 0}, {1, 0}, {0, 1}, {2, 0}, {1, 1}, {0, 2}, {3, 0}, {2, 1}, {1, 2}, {0, 3}, {4, 0}, {3, 1}, {2, 2}};
  int x = 0;
  int index = 0;
  int i;
  for (i = 0; i < 13; i++) {
    int y = reduce(S, now, responses[i][0], responses[i][1], length, colors, n);
    if (y > x) {
      x = y;
      index = i;
    }
  }
  return x;
}

char** SetReduce(char** S, char* now, int c, int p, int length, int colors, int n, int newN) {
  char** newS = malloc(sizeof(char*) * newN);
  int j = 0;
  int i;
  for (i = 0; i < n; i++) {
    int* r = analyze(S[i], now, length, colors);
    if (r[0] == c && r[1] == p) {
      newS[j] = malloc(length * sizeof(char));
      int k;
      for (k = 0; k < length; k++) {
	newS[j][k] = S[i][k];
      }
      j++;
    }
    free(r);
  }
  for (i = 0; i < n; i++) {
    free(S[i]);
  }
  free(S);
  S = newS;
  return newS;
}

char* BestMove(char** S, int length, int colors, int n) {
  int best = 0;
  float bestR = fullReduce(S, S[0], length, colors, n);
  int i;
  for (i = 1; i < n; i++) {
    int x = fullReduce(S, S[i], length, colors, n);
    //printf("Code, Value: ");
    // print_one(S[i], length);
    // printf(" %d\n", x);
    if (x < bestR) {
      best = i;
      bestR = x;
    }
  }
  return S[best];
}



int main() {
  int length = 4;
  int colors = 6;
  int n = 1;
  int i;
  for (i = 1; i <= length; i++) {
    n *= colors;
  }
  char** S = create_all(length, colors, n);
  char* move = S[7]; //BestMove(S, length, colors, n);
  while (n > 0) {
    printf("%d remaining candidates\n", n);
    print_one(move, length);
    printf("\n");
    int c;
    int p;
    printf("Colors: ");
    scanf("%d", &c);
    printf("Place: ");
    scanf("%d", &p);
    if (c == 0 && p == 4) {
      printf("Yay! I win!\n");
      break;
    }
    int newN = reduce(S, move, c, p, length, colors, n);
    if (newN == 0) {
      printf("I'm Stumped...\n");
      break;
    }
    S = SetReduce(S, move, c, p, length, colors, n, newN);
    n = newN;
    move = BestMove(S, length, colors, n);
  }
  for (i = 0; i < n; i++) {
    free(S[i]);
  }
  free(S);
  return 0;
}
