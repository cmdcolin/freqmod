#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <ncurses.h>

#define S_RATE  (44100)
#define B_SIZE  (1024)

//fmod equation
//x(t)=A(t)*[cos(2pi*fc*t + I(t)cos(2pi*fm*t+theta_m) + theta_c]
//A(t) time varying amplitude
//I(t) modulation index
//fc the carrier frequency
//fm the modulating frequency
//theta_m,c phase constants


int buffer[B_SIZE];
int main() {
    float p1=0,p2=0;
    float f_c=40,f_m=80;
    float amplitude=3200000;
    float theta_m=0;
    float theta_c=0;
    float I_t=0;
    int sign=1;
    int index=0;
    char ch;
    WINDOW *w = initscr();
    cbreak();
    nodelay(w, TRUE);
    noecho();
    while(1) {
        if((ch=getch())!=-1) {
            if(ch=='p') { break; }
            else if(ch=='q') { f_c*=2; }
            else if(ch=='w') { f_c/=2; }
            else if(ch=='a') { f_m*=2; }
            else if(ch=='s') { f_m/=2; }
            else if(ch=='z') { theta_m+=.01; }
            else if(ch=='x') { theta_m-=.01; }
            else if(ch=='c') { theta_c+=.01; }
            else if(ch=='v') { theta_c-=.01; }
        }
        p1 += f_c*2*M_PI/S_RATE+theta_c;
        p2 += f_m*2*M_PI/S_RATE+theta_m;
        I_t += 0.0001*sign;
        if(I_t>100||I_t<0) sign=-sign;

        buffer[index]=(int) (amplitude*(cos(p1+I_t*(cos(p2)))));

        index++;
        if(index%B_SIZE==0) {
            fprintf(stderr,"f_c=%02.2f f_m=%02.2f theta_c=%02.2f theta_m=%02.2f I_t=%02.2f\r",f_c,f_m,theta_c,theta_m,I_t);
            fwrite(buffer, sizeof(int), B_SIZE, stdout);
            index=0;
        }
    }
    endwin();
}
