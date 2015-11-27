#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdint.h>

#define S_RATE  (44100)
#define B_SIZE  (1024)

//fmod equation
//x(t)=A(t)*[cos(2pi*fc*t + I(t)cos(2pi*fm*t+theta_m) + theta_c]
//A(t) time varying amplitude
//I(t) modulation index
//fc the carrier frequency
//fm the modulating frequency
//theta_m,c phase constants


int main() {
    uint16_t buffer[B_SIZE];
    float p1=0,p2=0;
    float f_c=40,f_m=80;
    float A=50;
    float theta_m=0,theta_c=0;
    bool square1=false,square2=false;
    float I_t=0;
    int sign=1;
    int index=0;
    char ch;
    WINDOW *w = initscr();
    cbreak();
    nodelay(w, TRUE);
    noecho();
    while((ch = getch())) {
        if(ch=='p') { break; }
        else if(ch=='q') { f_c*=2; }
        else if(ch=='w') { f_c/=2; }
        else if(ch=='a') { f_m*=2; }
        else if(ch=='s') { f_m/=2; }
        else if(ch=='z') { theta_m+=.01; }
        else if(ch=='x') { theta_m-=.01; }
        else if(ch=='c') { theta_c+=.01; }
        else if(ch=='v') { theta_c-=.01; }
        else if(ch=='b') { square1=!square1; }
        else if(ch=='g') { square2=!square2; }

        p1 += f_c*2*M_PI/S_RATE+theta_c;
        p2 += f_m*2*M_PI/S_RATE+theta_m;
        I_t += 0.0001*sign;
        if(I_t>100||I_t<0) sign=-sign;

        float c1=cos(p2);
        if(square1) c1=c1<=0?-1:1;
        float c2=cos(p1+I_t*c1);
        if(square2) c2=c2<=0?-1:1;
        buffer[index]=A*c2;


        index++;
        if(index%B_SIZE==0) {
            fprintf(stderr,"f_c=%02.2f f_m=%02.2f theta_c=%02.2f theta_m=%02.2f I_t=%02.2f s1=%s s2=%s\r",
                f_c,f_m,theta_c,theta_m,I_t,square1?"true":"false",square2?"true":"false");
            fwrite(buffer, sizeof(uint16_t), B_SIZE, stdout);
            index=0;
        }
    }
    endwin();
}
