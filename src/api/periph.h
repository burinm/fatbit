#ifndef __PERIPH_H__
#define __PERIPH_H__

/* Turn on necessary clocks for application
    Currently unused
*/
void CMU_setup();

/* Set LED0 to push/pull configuration
*/
void LED0_setup();

/* Turn LED0 on 
*/
void led0_on();

/* Turn LED0 off 
*/
void led0_off();


#endif
