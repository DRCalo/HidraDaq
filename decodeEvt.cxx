
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#include <vector>

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

#define INPUT_STR_SIZE 65536

void decodeData ( std::vector <uint32_t>& event )
 {
   uint32_t v;
 }

void getData ( char* str, std::vector <uint32_t>& event )
 {
   uint32_t v;
   char* next(NULL);
   
   do
    {
      v = strtol ( str, &next, 16 );
      event.push_back ( v );
      if ( v == 0xbbeeddaa ) break;
      str = next;
    }
   while (1);
 }

int main ( int argc, char** argv )
 {
   FILE* fptr;
   char inputStr [ INPUT_STR_SIZE ];
   int i(0);
   std::vector <uint32_t> event;

   if (argc == 1) return -1;

   fptr = fopen ( argv[1], "r" );

   if (fptr == NULL) return -1;

   for ( int i=0; i<1000; i++ )
    {
      event.clear();
      if ( ! fgets ( inputStr, INPUT_STR_SIZE, fptr ) ) break;
      printf ( "%s" , inputStr );

      getData ( inputStr, event );

      decodeData ( event );
    }

   if (fptr != NULL) fclose ( fptr );

   return 0;
 }
