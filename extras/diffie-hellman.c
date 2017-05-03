#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint64_t pow64(uint8_t base, uint8_t exponent);

int main(int argc, char* argv[]) {

if (argc != 5) {
    printf("diffie [generator] [mod] [rand1] [rand2]\n");
    return 0; 
}

uint8_t generator=atoi(argv[1]);
uint8_t modulus=atoi(argv[2]);
uint8_t rand1=atoi(argv[3]);
uint8_t rand2=atoi(argv[4]);

printf("gen: %u mod: %u rand1: %u rand2 %u\n",generator,modulus,rand1,rand2);

uint64_t Alice;
Alice=pow64(generator,rand1);
Alice = Alice % modulus;
printf("Alice sends -> %lu\n",Alice);

uint64_t Bob;
Bob=pow64(generator,rand2);
Bob = Bob % modulus;
printf("Bob sends -> %lu\n",Bob);

uint64_t Alice_secret=pow64(Bob,rand1);
Alice_secret = Alice_secret % modulus;
printf("Alice decodes secret -> %lu\n",Alice_secret);

uint64_t Bob_secret=pow64(Alice,rand2);
Bob_secret = Bob_secret % modulus;
printf("Bob decodes secret -> %lu\n",Bob_secret);

}

uint64_t pow64(uint8_t base, uint8_t exponent) {
uint64_t result=1;
uint64_t last_result=1;

    for (uint8_t i=0;i < exponent; i++) {
        result= result * base;
        if (result < last_result) {
            printf("Overflow. base %u exponent %u\n",base, exponent);
            exit(-1);
        }
        last_result=result;
    }
return result;
}
