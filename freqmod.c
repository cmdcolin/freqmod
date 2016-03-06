#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdint.h>

#define S_RATE  (44100)
#define B_SIZE  (128)
#define max(a,b) (a>b?a:b)

//fmod equation
//x(t)=A(t)*[cos(2pi*fc*t + I(t)cos(2pi*fm*t+osc2_t) + osc1_t]
//A(t) time varying amplitude
//I(t) modulation index
//fc the carrier frequency
//fm the modulating frequency
//osc1_t,osc2_t phase constants

struct synth_state {
    float osc1;
    float osc2;
    float osc3;
    float amplitude;
    float decay;
    int osc1_type;
    int osc2_type;
    int osc3_type;
    int third_mod;
    float depth;
};

int save_synth(char *,struct synth_state *);
int open_synth(char *,struct synth_state *);

float cosFunc(float pos,float theta) { return cos(pos*2*M_PI+theta); }
float squareFunc(float pos,float theta) { return cos(pos*2*M_PI+theta)<=0?-1:1; }
float triangleFunc(float pos,float theta) { return 1-fabs(fmod(pos+theta,1.0)-0.5)*4; }

int main() {
    uint16_t buffer[B_SIZE];
    struct synth_state s = {40,80,40,50,0.0001,0,0,0,0,100};
    if(open_synth(".synthrc",&s)<1) { fprintf(stderr,"Using synth defaults"); }
    
    float t=0;
    float I_t=0;
    int sign=1;
    int index=0;
    float A=s.amplitude;
    char ch;
    WINDOW *w = initscr();
    cbreak();
    nodelay(w, TRUE);
    noecho();
    while((ch = getch())) {
        if(ch=='p') { break; }
        else if(ch=='q') { s.osc1*=2; }
        else if(ch=='w') { s.osc1/=2; }
        else if(ch=='a') { s.osc2*=2; }
        else if(ch=='s') { s.osc2/=2; }
        else if(ch=='z') { s.osc3*=2; }
        else if(ch=='x') { s.osc3/=2; }
        else if(ch=='e') { s.decay*=2; }
        else if(ch=='r') { s.decay/=2; }
        else if(ch=='d') { s.amplitude*=2;A=s.amplitude; }
        else if(ch=='f') { s.amplitude/=2;A=s.amplitude; }
        else if(ch=='t') { s.osc1_type++;s.osc1_type%=3; }
        else if(ch=='g') { s.osc2_type++;s.osc2_type%=3; }
        else if(ch=='b') { s.osc3_type++;s.osc3_type%=3; }
        else if(ch=='m') { s.third_mod++;s.third_mod%=4; }
        else if(ch=='u') { s.depth+=0.1; }
        else if(ch=='y') { s.depth-=0.1; }
        else if(ch=='j') { s.depth*=2; }
        else if(ch=='k') { s.depth/=2; }
        else if(ch=='h') { A=s.amplitude; }
        else if(ch=='o') { if(save_synth(".synthrc",&s)<0) { perror("save_synth"); break; } }


        t += 1.0/S_RATE;
        float p1 = fmod(s.osc1*t,1.0);
        float p2 = fmod(s.osc2*t,1.0);
        float p3 = fmod(s.osc3*t,1.0);

        I_t = s.depth;
        A=max(0,A-s.decay);

        if(s.third_mod==0) {
            float c1 = cosFunc(p1,0);
            if(s.osc1_type==1) c1 = squareFunc(p1,0);
            if(s.osc1_type==2) c1 = triangleFunc(p1,0);
            float c2 = cosFunc(p2,I_t*c1);
            if(s.osc2_type==1) c2 = squareFunc(p2,I_t*c1);
            if(s.osc2_type==2) c2 = triangleFunc(p2,I_t*c1);
            buffer[index]=A*c2;
        }

        else if(s.third_mod==1) {
            float c1 = cosFunc(p1,0);
            if(s.osc1_type==1) c1 = squareFunc(p1,0);
            if(s.osc1_type==2) c1 = triangleFunc(p1,0);
            float c2 = cosFunc(p2,I_t*c1);
            if(s.osc2_type==1) c2 = squareFunc(p2,I_t*c1);
            if(s.osc2_type==2) c2 = triangleFunc(p2,I_t*c1);
            float c3 = cosFunc(p3,I_t*c2);
            if(s.osc3_type==1) c3 = squareFunc(p3,I_t*c2);
            if(s.osc3_type==2) c3 = triangleFunc(p3,I_t*c2); 
            buffer[index]=A*c3;
        }
        else if(s.third_mod==2) {
            float c1 = cosFunc(p1,0);
            if(s.osc1_type==1) c1 = squareFunc(p1,0);
            if(s.osc1_type==2) c1 = triangleFunc(p1,0);
            float c2 = cosFunc(p2,0);
            if(s.osc2_type==1) c2 = squareFunc(p2,0);
            if(s.osc2_type==2) c2 = triangleFunc(p2,0);
            float c3 = cosFunc(p3,I_t*c2+I_t*c1);
            if(s.osc3_type==1) c3 = squareFunc(p3,I_t*c2+I_t*c1);
            if(s.osc3_type==2) c3 = triangleFunc(p3,I_t*c2+I_t*c1);
            buffer[index]=A*c3;
        }
        else if(s.third_mod==3) {
            float c1 = cosFunc(p1,0);
            if(s.osc1_type==1) c1 = squareFunc(p1,0);
            if(s.osc1_type==2) c1 = triangleFunc(p1,0);
            buffer[index]=A*c1;
        }
        


        index++;
        if(index%B_SIZE==0) {
            if(s.third_mod==3) {
                fprintf(stderr,"osc1=%02.2f/%d\t%s\r\n",
                    s.osc1,s.osc1_type,"osc1");
            }
            else if(s.third_mod>0) {
                fprintf(stderr,"osc1=%02.2f/%d\tosc2=%02.2f/%d\tosc3=%02.2f/%d\td=%02.5f\tA=%02.5f\tI_t=%02.2f\t%s\r\n",
                    s.osc1,s.osc1_type,s.osc2,s.osc2_type,s.osc3,s.osc3_type,s.decay,s.amplitude,I_t,
                    s.third_mod==1?"osc1->osc2->osc3":"osc1->osc2<-osc3");
            }
            else {
                fprintf(stderr,"osc1=%02.2f/%d\tosc2=%02.2f/%d\td=%02.5f\tA=%02.5f\tI_t=%02.2f\t%s\r\n",
                    s.osc1,s.osc1_type,s.osc2,s.osc2_type,s.decay,s.amplitude,I_t,"osc1->osc2");
            }
            fwrite(buffer, sizeof(uint16_t), B_SIZE, stdout);
            index=0;
        }
    }
    endwin();
}



int open_synth(char *filename,struct synth_state* state) {
    FILE *f=fopen(filename, "r");
    if(f) {
        fscanf(f, "osc1=%f\n",&state->osc1);
        fscanf(f, "osc2=%f\n",&state->osc2);
        fscanf(f, "osc3=%f\n",&state->osc3);
        fscanf(f, "amplitude=%f\n",&state->amplitude);
        fscanf(f, "decay=%f\n",&state->decay);
        fscanf(f, "osc1_type=%d\n",&state->osc1_type);
        fscanf(f, "osc2_type=%d\n",&state->osc2_type);
        fscanf(f, "osc3_type=%d\n",&state->osc3_type);
        fscanf(f, "third_mod=%d\n",&state->third_mod);
        fscanf(f, "depth=%f\n",&state->depth);
        fclose(f);
        return 1;
    }
    else return -1;
}

int save_synth(char *filename,struct synth_state* state) {
    FILE *f=fopen(filename, "w");
    if(f) {
        fprintf(f, "osc1=%f\n",state->osc1);
        fprintf(f, "osc2=%f\n",state->osc2);
        fprintf(f, "osc3=%f\n",state->osc3);
        fprintf(f, "amplitude=%f\n",state->amplitude);
        fprintf(f, "decay=%f\n",state->decay);
        fprintf(f, "osc1_type=%d\n",state->osc1_type);
        fprintf(f, "osc2_type=%d\n",state->osc2_type);
        fprintf(f, "osc3_type=%d\n",state->osc3_type);
        fprintf(f, "third_mod=%d\n",state->third_mod);
        fprintf(f, "depth=%f\n",state->depth);
        fclose(f);
        return 1;
    }
    else return -1;
}
