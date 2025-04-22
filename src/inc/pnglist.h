#ifndef _PNG_LIST_H_
#define _PNG_LIST_H_

typedef struct PNGLIST PNGLIST;
typedef struct DYNPNGICONLIST DYNPNGICONLIST;
typedef struct PNGTOP_DESC PNGTOP_DESC;

struct PNGLIST
{
  PNGLIST *next;
  char *pngname;
  IMGHDR *img;
  unsigned int hash;
  int icon;
};

struct DYNPNGICONLIST
{
  DYNPNGICONLIST *next;
  int icon;
  IMGHDR *img;
};

struct PNGTOP_DESC {
  PNGLIST *pltop;
  char *bitmap;
  DYNPNGICONLIST *dyn_pltop;
  DYNPNGICONLIST *dyn_pltop2;
};

#define PNG_8 1
#define PNG_16 2
#define PNG_24 3

#define PNG_1 0xFF

#endif
