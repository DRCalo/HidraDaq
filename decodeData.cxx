#include <cstdint>
#include <cstdio>

struct head_s
 {
    unsigned u : 8;
    unsigned n : 6;
    unsigned z : 2;
    unsigned c : 8;
    unsigned m : 3;
    unsigned g : 5;
 };

union head_u
 {
    uint32_t v;
    struct head_s h;
 };

struct trail_s
 {
    unsigned c : 24;
    unsigned m : 3;
    unsigned g : 5;
 };

union trail_u
 {
    uint32_t v;
    struct trail_s t;
 };

struct data_s
 {
    unsigned v : 12;
    unsigned f : 3;
    unsigned u : 1;
    unsigned c : 8;
    unsigned m : 3;
    unsigned g : 5;
 };

union data_u
 {
    uint32_t v;
    struct data_s d;
 };

void decodehead ( uint32_t hd, uint32_t* nch )
 {
    union head_u u;
    u.v = hd;
    printf ("header %x geo %x marker %x crate %x zero %d chans %x not used %x\n", hd, u.h.g, u.h.m, u.h.c, u.h.z, u.h.n, u.h.u );
    *nch = u.h.n;
 }

void decodetrail ( uint32_t tr, uint32_t* evc )
 {
    union trail_u u;
    u.v = tr;
    printf ("trail %x geo %x marker %x event counter %d\n", tr, u.t.g, u.t.m, u.t.c );
    *evc = u.t.c;
 }

void decodeqdc ( uint32_t idx, uint32_t dt, bool* valid, uint32_t* val )
 {
    union data_u u;
    u.v = dt;
    u.d.c &= 0x1f;
    u.d.f &= 0x3;
    printf ("QDC index %d data %x geo %x marker %x chan %x not used %d flags %x val %d\n", idx, dt, u.d.g, u.d.m, u.d.c, u.d.u, u.d.f, u.d.v );
    *valid = (u.d.m == 0) && (u.d.f == 0);
    *val = u.d.v;
 }

void decodetdc ( uint32_t idx, uint32_t dt, bool* valid, uint32_t* val )
 {
    union data_u u;
    u.v = dt;
    u.d.c >>= 1;
    u.d.c &= 0xf;
    printf ("TDC index %d data %x geo %x marker %x chan %x not used %d flags %x val %d\n", idx, dt, u.d.g, u.d.m, u.d.c & 0x1f, u.d.u, u.d.f, u.d.v );
    *valid = (u.d.m == 0) && (u.d.f == 0);
    *val = u.d.v;
 }

int main ( int argc, char** argv )
 {
	 return 0;
 }
