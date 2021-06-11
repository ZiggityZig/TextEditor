/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

/*** defines ***/

#define CTRL_KEY(k) ((k)&0x1f) //Sets the 3 msb bits to 0's, mirroring what ctrl does in the terminal
typedef struct termios termios;

/*** data ***/

termios orig_termios;
char debug = 0;

/*** terminal ***/

void die(const char *s)
{
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3); 
  perror(s);
  exit(1);
}

void disableRawMode()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode()
{
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);

  termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

char editorReadKey()
{
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
  {
    if (nread == -1 && errno != EAGAIN)
      die("read");
  }
  return c;
}

/*** input ***/

void editorProcessKeypress()
{
  char c = editorReadKey();
  switch (c)
  {
  case CTRL_KEY('q'):
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    exit(0);
    break;
  }
}

/*** output ***/

void editorDrawRows() {
  int y;
  for (y = 0; y < 24; y++) {
    write(STDOUT_FILENO, "~\r\n", 3);
  }
}

void editorRefreshScreen() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  editorDrawRows();
  write(STDOUT_FILENO, "\x1b[H", 3);
}

/*** init ***/

int main(int argc, char **argv)
{
  enableRawMode();

  for (int i = 1; i < argc; i++)
  {
    if (strncmp(argv[i], "-D", 2) == 0)
    {
      debug = 1;
      fprintf(stderr, "Running in debug mode\r\n");
    }
  }

  while (1)
  {
    editorRefreshScreen();
    editorProcessKeypress();
  }
  return 0;
}

//  if (iscntrl(c))
//   {
//     if(debug)
//       printf("%d\r\n", c);
//   }
//   else
//   {
//     if(debug)
//       printf("%d ('%c')\r\n", c, c);
//     else {
//        printf("%c", c);
//       fflush(stdout);
//     }

//   }