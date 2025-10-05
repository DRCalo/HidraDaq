#ifndef _HIDRAEVENTHEADER_H_
#define _HIDRAEVENTHEADER_H_

/*****************************************

  hidraEventHeader.h
  ------------------

  Definition of the hidraEventHeader class

*****************************************/

#include <cstdint>

#define HIDRA_EVENT_MARKER 0xccaaffee
#define HEADER_END_MARKER 0xaccadead
#define EVENT_END_MARKER 0xbbeeddaa

typedef struct hidraEventHeader
 {
	uint32_t eventMarker;
	uint32_t eventNumber;
	uint32_t spillNumber;
	uint32_t headerSize;
	uint32_t trailerSize;
	uint32_t dataSize;
	uint32_t eventSize;
	uint32_t eventTimeSecs;
	uint32_t eventTimeMicroSecs;
	uint32_t triggerMask;
	uint32_t isPedMask;
	uint32_t isPedFromScaler;
	uint32_t sanityFlag;
	uint32_t headerEndMarker;
 } hidraEventHeader;

typedef union headerU
 {
   hidraEventHeader heh_s;
   uint32_t heh_a [ sizeof (hidraEventHeader) / sizeof (uint32_t) ];
 } headerU;

typedef uint32_t hidraEventTrailer;

#define U32_SZ ( sizeof (uint32_t) )
#define HEH_SZ ( sizeof (hidraEventHeader) )
#define HEH_NW ( HEH_SZ / U32_SZ )
#define HET_SZ ( sizeof (hidraEventTrailer) )
#define HET_NW ( HET_SZ / U32_SZ )

#endif // _HIDRAEVENTHEADER_H_
