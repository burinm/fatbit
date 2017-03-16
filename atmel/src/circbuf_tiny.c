/* circbuf_tiny.c - 2016, 2017 - burin 

    3/2017 - Modifying to hold generic pointers instead of uint8_t
*/

#include "circbuf_tiny.h"
#include <stdlib.h>
#include <string.h>

uint8_t circbuf_tiny_init(circbuf_tiny_t *c) {
    if (c) {
        c->data=(uint32_t*)malloc(CIRCBUF_TINY_MAX * sizeof(uint32_t*));
        if (c->data == NULL) { return 0; }

        memset(c->data,0,CIRCBUF_TINY_MAX * sizeof(uint32_t*));

        c->read_i=0;
        c->write_i=0;
        return 1;
    }
return 0;
}
uint8_t circbuf_tiny_destroy(circbuf_tiny_t *c) {
uint8_t i=0;

    if (c) {
        if (c->data) {
            for (i=0;i<CIRCBUF_TINY_MAX;i++) {
                // This cleans up externally allocaed memory
                //  consider removing 
                if ( *((c->data) + i) ) {
                    free( (uint32_t*)*((c->data) + i) );
                }
            }
            free (c->data);
        }
    }
return 0;
}

uint8_t circbuf_tiny_write(circbuf_tiny_t *c, uint32_t *p) {
    if ( CIRCBUF_TINY_SIZE(c) < (CIRCBUF_TINY_MAX)) {
        *((c->data) + c->write_i) = p;
        c->write_i++;
        return 1;
    }
return 0;
}

uint8_t circbuf_tiny_read(circbuf_tiny_t *c, uint32_t **p) {
    if ( CIRCBUF_TINY_SIZE(c) > 0 ) {
        *p = (uint32_t*)*((c->data) + c->read_i);
        c->read_i++;
        return 1;
    }
return 0;
}
