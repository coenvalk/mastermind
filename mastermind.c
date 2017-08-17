/*
  MIT License
  
  Copyright (c) 2017 Coen Valk
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  
*/
// Guess Mastermind code

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

unsigned char* get_code(int length, unsigned char colors, int index) {
  unsigned char* code = (unsigned char*) malloc(length * sizeof(unsigned char));
  int i;
  for (i = 0; i < length; i++) {
    code[length - i - 1] = index % colors;
    index /= colors;
  }
  return code;
}

unsigned char** create_all(int length, unsigned char colors, int n) { // creates list of possible combinations
  unsigned char** S = (unsigned char**) malloc(n * sizeof(unsigned char*));
  unsigned char* first = (unsigned char*) malloc(length * sizeof(unsigned char));
  int i;
  for (i = 0; i < length; i++) {
    first[i] = 0;
  }
  S[0] = first;
  for (i = 1; i < n; i++) {
    unsigned char* now = (unsigned char*) malloc(length * sizeof(unsigned char));
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

void print_one(unsigned char* now, int length) {
  int i;
  for (i = 0; i < length; i++) {
    printf("%d", (int) now[i]);
  }
}

void print_all(unsigned char** S, int n, int length) { // Prints all current possibilities
  int i;
  for (i = 0; i < n; i++) {
    print_one(S[i], length);
    printf(", ");
  }
  printf("\n");
}

bool isin(unsigned char x, unsigned char* code, int length) {
  int i;
  for (i = 0; i < length; i++) {
    if (code[i] == x) return true;
  }
  return false;
}

int howmany(unsigned char x, unsigned char* code, int length) {
  int r = 0;
  int i;
  for (i = 0; i < length; i++) {
    if (code[i] == x) r++;
  }
  return r;
}

int* analyze(unsigned char* code, unsigned char* guess, int length, unsigned char colors) {
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
  int* r = (int*) malloc(sizeof(int) * 2);
  r[0] = c-p;
  r[1] = p;
  return r;
}

int reduce(unsigned char** S, unsigned char* now, int c, int p, int length, unsigned char colors, int n) {
  int x = 0;
  int i;
  for (i = 0; i < n; i++) {
    int* r = analyze(S[i], now, length, colors);
    if (r[0] == c && r[1] == p) x++;
    free(r);
  }
  return x;
}

int fullReduce(unsigned char** S, unsigned char* now, int length, unsigned char colors, int n) {
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

unsigned char** SetReduce(unsigned char** S, unsigned char* now, int c, int p, int length, unsigned char colors, int n, int newN) {
  unsigned char** newS = malloc(sizeof(unsigned char*) * newN);
  int j = 0;
  int i;
  for (i = 0; i < n; i++) {
    int* r = analyze(S[i], now, length, colors);
    if (r[0] == c && r[1] == p) {
      newS[j] = malloc(length * sizeof(unsigned char));
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

unsigned char* BestMove(unsigned char** S, int length, unsigned char colors, int n) {
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
  unsigned char colors = 6;
  print_one(get_code(length, colors, 7), length);
  int n = 1;
  int i;
  for (i = 1; i <= length; i++) {
    n *= colors;
  }
  unsigned char** S = create_all(length, colors, n);
  unsigned char* move = S[7]; //BestMove(S, length, colors, n);
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
