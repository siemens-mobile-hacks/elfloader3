#ifndef _PNG_LIST_H_
#define _PNG_LIST_H_

typedef struct
{
  void *next;
  char *pngname;
  IMGHDR * img;
  unsigned int hash;
}PNGLIST;

typedef struct
{
  void *next;
  int icon;
  IMGHDR * img;
}DYNPNGICONLIST;

typedef struct
{
  PNGLIST * pltop;
  char *bitmap;
  DYNPNGICONLIST *dyn_pltop;
}PNGTOP_DESC;  

#define PNG_8 1
#define PNG_16 2
#define PNG_24 3

#define PNG_1 0xFF

#endif
