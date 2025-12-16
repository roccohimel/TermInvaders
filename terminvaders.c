#include <stdio.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

struct termios original;

void enableRawMode() {
        struct termios raw;
        tcgetattr(STDIN_FILENO, &original);
        raw = original;
        raw.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
}

void plot(int x, int y, char *z) {
        printf("\033[%d;%dH%s", y, x, z);
}

int main() {
        system("tput smcup; clear");
        printf("\033[?25l");

        enableRawMode();

        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int maxScreenX = w.ws_col - 1;
        int maxScreenY = w.ws_row - 1;

        int tankPositionX = (maxScreenX / 2) - 1;
        int tankPositionY = maxScreenY - 3;
        char *tankChar = "<|>";

        int bulletPositionX = 0;
        int bulletPositionY = 0;
        char *bulletChar = ".";
        int bulletActive = 0;

        while (1) {
                printf("\033[H\033[J");

                plot(tankPositionX, tankPositionY, tankChar);

                if (bulletActive) {
                        plot(bulletPositionX, bulletPositionY, bulletChar);
                        bulletPositionY--;

                        if (bulletPositionY <= 1) {
                                bulletActive = 0;
                        }
                }

                char input = getchar();

                if (input == 'a') tankPositionX--;
                if (input == 'd') tankPositionX++;

                if (input == ' ' && !bulletActive) {
                        bulletActive = 1;
                        bulletPositionX = tankPositionX + 1;
                        bulletPositionY = tankPositionY - 1;
                }

                if (input == 'q') break;
        }

        disableRawMode();
        printf("\033[?25h");
        system("tput rmcup");
        return 0;
}
