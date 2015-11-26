#include <stdio.h>
#include <math.h>
#include <stdio.h>


#define S_RATE  (44100)
 
int buffer[S_RATE];
int main() {
    float phase=0;
    float freq_Hz=440;
    float amplitude=32000;
    float freq_radians_per_sample = freq_Hz*2*M_PI/S_RATE;
    while(1) {
        for(int i=0;i<44100;i++) {
            phase += freq_radians_per_sample; 
            buffer[i]=(int) (amplitude*sin(phase));
        }
        fwrite(buffer, sizeof(int), S_RATE, stdout);
    }
}
