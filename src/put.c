#include "put.h"

void put_u8(uint8_t x, unsigned char *b) {
  b[0] = x;
}

void put_lu16(uint16_t x, unsigned char *b) {
  b[0] = x;
  b[1] = x >> 8;
}

void put_bu16(uint16_t x, unsigned char *b) {
  b[0] = x >> 8;
  b[1] = x;
}

void put_lu32(uint32_t x, unsigned char *b) {
  b[0] = x;
  b[1] = x >> 8;
  b[2] = x >> 16;
  b[3] = x >> 24;
}

void put_bu32(uint32_t x, unsigned char *b) {
  b[0] = x >> 24;
  b[1] = x >> 16;
  b[2] = x >> 8;
  b[3] = x;
}

void put_lu64(uint64_t x, unsigned char *b) {
  b[0] = x;
  b[1] = x >> 8;
  b[2] = x >> 16;
  b[3] = x >> 24;
  b[4] = x >> 32;
  b[5] = x >> 40;
  b[6] = x >> 48;
  b[7] = x >> 56;
}

void put_bu64(uint64_t x, unsigned char *b) {
  b[0] = x >> 56;
  b[1] = x >> 48;
  b[2] = x >> 40;
  b[3] = x >> 32;
  b[4] = x >> 24;
  b[5] = x >> 16;
  b[6] = x >> 8;
  b[7] = x;
}

void put_i8(int8_t x, unsigned char *b) { put_u8(x, b); }
void put_li16(int16_t x, unsigned char *b) { put_lu16(x, b); }
void put_bi16(int16_t x, unsigned char *b) { put_bu16(x, b); }
void put_li32(int32_t x, unsigned char *b) { put_lu32(x, b); }
void put_bi32(int32_t x, unsigned char *b) { put_bu32(x, b); }
void put_li64(int64_t x, unsigned char *b) { put_lu64(x, b); }
void put_bi64(int64_t x, unsigned char *b) { put_bu64(x, b); }

void put_lf32(float x, unsigned char *b) { put_lu32(*(uint32_t*) &x, b); }
void put_bf32(float x, unsigned char *b) { put_bu32(*(uint32_t*) &x, b); }
void put_lf64(double x, unsigned char *b) { put_lu64(*(uint64_t*) &x, b); }
void put_bf64(double x, unsigned char *b) { put_bu64(*(uint64_t*) &x, b); }
