#pragma once

#include <stdint.h>

void put_u8(uint8_t x, unsigned char *b);
void put_lu16(uint16_t x, unsigned char *b);
void put_bu16(uint16_t x, unsigned char *b);
void put_lu32(uint32_t x, unsigned char *b);
void put_bu32(uint32_t x, unsigned char *b);
void put_lu64(uint64_t x, unsigned char *b);
void put_bu64(uint64_t x, unsigned char *b);

void put_i8(int8_t x, unsigned char *b);
void put_li16(int16_t x, unsigned char *b);
void put_bi16(int16_t x, unsigned char *b);
void put_li32(int32_t x, unsigned char *b);
void put_bi32(int32_t x, unsigned char *b);
void put_li64(int64_t x, unsigned char *b);
void put_bi64(int64_t x, unsigned char *b);

void put_lf32(float x, unsigned char *b);
void put_bf32(float x, unsigned char *b);
void put_lf64(double x, unsigned char *b);
void put_bf64(double x, unsigned char *b);
