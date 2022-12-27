#ifndef __TEST_FOPEN__
#define __TEST_FOPEN__
enum operation {R, W};
void readOpen();
void readWriteOpen();
FILE *  operateFopen(const char * pathname, const char * mode);
void read(FILE * file);
void write(FILE * file);
#endif