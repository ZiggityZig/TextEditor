#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

typedef struct termios termios;

termios orig_termios;
void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main (int argc, char** argv){
    enableRawMode();
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){
        printf("%c", c);
    }

    return 0;
}