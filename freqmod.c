#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <ncurses.h>

#define S_RATE  (44100)
 
int buffer[S_RATE];
int main() {
    float p1=0,p2=0;
    float fc=440,fm=440;
    float amplitude=3200000;
    float theta=0;
    char ch;
    WINDOW *w = initscr();
    cbreak();
    nodelay(w, TRUE);
    noecho();
    while(1) {
        for(int i=0;i<S_RATE;i++) {
            if((ch=getch())!=-1) {
                if(ch=='q') { endwin(); return 0; }
                else if(ch=='u') { fc+=100; }
                else if(ch=='d') { fc-=100; }
                else if(ch=='x') { fm+=100; }
                else if(ch=='c') { fm+=100; }
                else if(ch=='t') { theta+=1; }
                fprintf(stderr,"%02.2f %02.2f %02.2f\r\n",fc,fm,theta);
            }
            float fr = fc*2*M_PI/S_RATE;
            float fs = fm*2*M_PI/S_RATE+theta;
            p1 += fr; 
            p2 += fs; 
            buffer[i]=(int) (amplitude*(sin(p1+sin(p2))));
        }
        fwrite(buffer, sizeof(int), S_RATE, stdout);
    }
}
