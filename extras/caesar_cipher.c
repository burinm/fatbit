#include <stdio.h>
#include <ctype.h>

#define LETTERS_IN_ALPHA    26

int is_alpha_lower(int c);

int main(int argc, char* argv[]) {

if (argc < 2 || argc > 3) { printf("caesar <file> [word]\n"); return 0;}
char c=0;
char* decode_word=argv[2];
char* file_name=argv[1];
int shifter=0;
int shifter_count=0;

int loop=0;

if (decode_word) {
    loop=1;
} else {
    loop=LETTERS_IN_ALPHA;
}

for (int i=0; i<loop; i++) { //Brute force all 26 combinations
    FILE* f=fopen(file_name,"r");
    if (f == NULL) { printf("Can't open %s\n",file_name); return 0;}

        shifter_count=0;
        shifter = i;

        if ( !decode_word ) {
            printf("----------shift = +%d  -%d-----------\n",shifter,LETTERS_IN_ALPHA-shifter);
        }

    while (1) {
        c = getc(f);
        if (c == EOF) { break; }
        c=tolower(c);

        if (decode_word) { //In this case we are encoding a message with a polyaplabetic cipher
            shifter=tolower(((char)*(decode_word + shifter_count)));
            if (is_alpha_lower(c) ) {
                shifter_count++;
            }
            if (((char)*(decode_word + shifter_count)) == '\0') {
                    shifter_count = 0;
            }
            shifter -= ('a' - 1);
        }

        //Basically, if it isn't punctuation, decode/encode it
        if (is_alpha_lower(c) ) {
            c = 'a' + (((c - 'a') + shifter) % LETTERS_IN_ALPHA);
        }

        printf("%c",c);
    }
    fclose(f);

    printf("\n");
}

return 0;
}

int is_alpha_lower(int c) {
    return (c >= 'a' && c <= 'z');
}
