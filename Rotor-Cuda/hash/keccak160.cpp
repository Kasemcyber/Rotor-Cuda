#include <cstdint>
#include <cstring>
#include "keccak160.h"

// ---------------------------------------------------------------------------------
// KECCAK/SHA3
// ---------------------------------------------------------------------------------

typedef union {
	uint8_t b[200];
	uint64_t q[25];
	uint32_t d[50];
} _KECCAK_STATE;

const uint64_t _KECCAKF_RNDC[24] = {
	0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
	0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
	0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
	0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
	0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
	0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
	0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
	0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

#define ROTL64(a,b) (((a) << (b)) | ((a) >> (64 - b)))

#define bswap32(v) (((v) >> 24) | (((v) >> 8) & 0xff00) | (((v) << 8) & 0xff0000) | ((v) << 24))

void keccak160(uint64_t* x, uint64_t* y, uint32_t* hash)
{
	_KECCAK_STATE e;
	memset(&e, 0, 200);
	uint32_t* X = (uint32_t*)x;
	uint32_t* Y = (uint32_t*)y;
	e.d[0] = bswap32(X[7]);
	e.d[1] = bswap32(X[6]);
	e.d[2] = bswap32(X[5]);
	e.d[3] = bswap32(X[4]);
	e.d[4] = bswap32(X[3]);
	e.d[5] = bswap32(X[2]);
	e.d[6] = bswap32(X[1]);
	e.d[7] = bswap32(X[0]);
	e.d[8] = bswap32(Y[7]);
	e.d[9] = bswap32(Y[6]);
	e.d[10] = bswap32(Y[5]);
	e.d[11] = bswap32(Y[4]);
	e.d[12] = bswap32(Y[3]);
	e.d[13] = bswap32(Y[2]);
	e.d[14] = bswap32(Y[1]);
	e.d[15] = bswap32(Y[0]);
	uint64_t* s = e.q;
	e.d[16] ^= 0x01;
	e.d[33] ^= 0x80000000;
	int i;
	uint64_t v, w, t[5], u[5];
	for (i = 0; i < 24; i++) {
		/* theta: c = a[0,i] ^ a[1,i] ^ .. a[4,i] */
		t[0] = s[0] ^ s[5] ^ s[10] ^ s[15] ^ s[20];
		t[1] = s[1] ^ s[6] ^ s[11] ^ s[16] ^ s[21];
		t[2] = s[2] ^ s[7] ^ s[12] ^ s[17] ^ s[22];
		t[3] = s[3] ^ s[8] ^ s[13] ^ s[18] ^ s[23];
		t[4] = s[4] ^ s[9] ^ s[14] ^ s[19] ^ s[24];
		/* theta: d[i] = c[i+4] ^ ROTL64(c[i+1],1) */
		u[0] = t[4] ^ ROTL64(t[1], 1);
		u[1] = t[0] ^ ROTL64(t[2], 1);
		u[2] = t[1] ^ ROTL64(t[3], 1);
		u[3] = t[2] ^ ROTL64(t[4], 1);
		u[4] = t[3] ^ ROTL64(t[0], 1);
		/* theta: a[0,i], a[1,i], .. a[4,i] ^= d[i] */
		s[0] ^= u[0]; s[5] ^= u[0]; s[10] ^= u[0]; s[15] ^= u[0]; s[20] ^= u[0];
		s[1] ^= u[1]; s[6] ^= u[1]; s[11] ^= u[1]; s[16] ^= u[1]; s[21] ^= u[1];
		s[2] ^= u[2]; s[7] ^= u[2]; s[12] ^= u[2]; s[17] ^= u[2]; s[22] ^= u[2];
		s[3] ^= u[3]; s[8] ^= u[3]; s[13] ^= u[3]; s[18] ^= u[3]; s[23] ^= u[3];
		s[4] ^= u[4]; s[9] ^= u[4]; s[14] ^= u[4]; s[19] ^= u[4]; s[24] ^= u[4];
		/* rho pi: b[..] = ROTL64(a[..], ..) */
		v = s[1];
		s[1] = ROTL64(s[6], 44);
		s[6] = ROTL64(s[9], 20);
		s[9] = ROTL64(s[22], 61);
		s[22] = ROTL64(s[14], 39);
		s[14] = ROTL64(s[20], 18);
		s[20] = ROTL64(s[2], 62);
		s[2] = ROTL64(s[12], 43);
		s[12] = ROTL64(s[13], 25);
		s[13] = ROTL64(s[19], 8);
		s[19] = ROTL64(s[23], 56);
		s[23] = ROTL64(s[15], 41);
		s[15] = ROTL64(s[4], 27);
		s[4] = ROTL64(s[24], 14);
		s[24] = ROTL64(s[21], 2);
		s[21] = ROTL64(s[8], 55);
		s[8] = ROTL64(s[16], 45);
		s[16] = ROTL64(s[5], 36);
		s[5] = ROTL64(s[3], 28);
		s[3] = ROTL64(s[18], 21);
		s[18] = ROTL64(s[17], 15);
		s[17] = ROTL64(s[11], 10);
		s[11] = ROTL64(s[7], 6);
		s[7] = ROTL64(s[10], 3);
		s[10] = ROTL64(v, 1);
		/* chi: a[i,j] ^= ~b[i,j+1] & b[i,j+2] */
		v = s[0]; w = s[1]; s[0] ^= (~w) & s[2]; s[1] ^= (~s[2]) & s[3]; s[2] ^= (~s[3]) & s[4]; s[3] ^= (~s[4]) & v; s[4] ^= (~v) & w;
		v = s[5]; w = s[6]; s[5] ^= (~w) & s[7]; s[6] ^= (~s[7]) & s[8]; s[7] ^= (~s[8]) & s[9]; s[8] ^= (~s[9]) & v; s[9] ^= (~v) & w;
		v = s[10]; w = s[11]; s[10] ^= (~w) & s[12]; s[11] ^= (~s[12]) & s[13]; s[12] ^= (~s[13]) & s[14]; s[13] ^= (~s[14]) & v; s[14] ^= (~v) & w;
		v = s[15]; w = s[16]; s[15] ^= (~w) & s[17]; s[16] ^= (~s[17]) & s[18]; s[17] ^= (~s[18]) & s[19]; s[18] ^= (~s[19]) & v; s[19] ^= (~v) & w;
		v = s[20]; w = s[21]; s[20] ^= (~w) & s[22]; s[21] ^= (~s[22]) & s[23]; s[22] ^= (~s[23]) & s[24]; s[23] ^= (~s[24]) & v; s[24] ^= (~v) & w;
		/* iota: a[0,0] ^= round constant */
		s[0] ^= _KECCAKF_RNDC[i];
	}
	hash[0] = e.d[3];
	hash[1] = e.d[4];
	hash[2] = e.d[5];
	hash[3] = e.d[6];
	hash[4] = e.d[7];
}