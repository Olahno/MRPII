#ifndef MRPII_H_
#define MRPII_H_
typedef unsigned char uint8_t;
int getCM();

void latch_tx(void);

void enable();

void DCMotorInit(uint8_t num);

void DCMotorRun(uint8_t motornum, uint8_t cmd);

void left(unsigned int);

void lleft();

void right(unsigned int);

void rright();

void backwd(unsigned int);

void bbackwd();

void forwd(unsigned int);

void fforwd();

void stop();

#endif // MRPII_H_
