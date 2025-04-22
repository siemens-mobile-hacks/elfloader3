#include "imgcache.h"
#include "config_struct.h"
#include "inc/pnglist.h"
#include "loader3/loader.h"

#define PIT_SIZE          20000 // меньше нельзя из-за старых эльфов
#define PIT_BITMAP_SIZE   ((PIT_SIZE + 7) / 8 * 2)
#define number 8


#define IMG_FILE_EXISTS (1 << 0)
#define IMG_IS_DYNAMIC  (1 << 1)

const char empty_img_data[] = { 0xFF };
const IMGHDR empty_img = { 0, 0, 0x1, (char *) empty_img_data };
PNGTOP_DESC pngtop = { 0 };

__arm IMGHDR *PatchGetPIT(unsigned int pic);

static void* xmalloc(int x,int n)
{
  return malloc(n);
}

static void xmfree(int x,void* ptr)
{
  mfree(ptr);
}

static __arm void read_data_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
  unsigned int err;
  int f;
  f=(int)png_get_io_ptr(png_ptr);
  fread(f, data, length, &err);
}

/* ====================================================================== */
static __arm int png_check_sig_a(png_bytep sig, int num) {
  return png_check_sig(sig, num);
}

static __arm png_structp png_create_read_struct_2_a(png_const_charp user_png_ver, png_voidp error_ptr,
                                                 png_error_ptr error_fn, png_error_ptr warn_fn, png_voidp mem_ptr,
                                                 png_malloc_ptr malloc_fn, png_free_ptr free_fn) {
  return png_create_read_struct_2(user_png_ver, error_ptr, error_fn, warn_fn, mem_ptr, malloc_fn, free_fn);
}

static __arm png_infop png_create_info_struct_a(png_structp png_ptr) {
  return png_create_info_struct(png_ptr);
}

static __arm void png_destroy_read_struct_a(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr) {
  png_destroy_read_struct(png_ptr_ptr, info_ptr_ptr, end_info_ptr_ptr);
}

static __arm void png_set_read_fn_a(png_structp png_ptr, png_voidp io_ptr, png_rw_ptr read_data_fn) {
  png_set_read_fn(png_ptr, io_ptr, read_data_fn);
}

static __arm void png_set_sig_bytes_a(png_structp png_ptr, int num_bytes) {
  png_set_sig_bytes(png_ptr, num_bytes);
}

static __arm void png_read_info_a(png_structp png_ptr, png_infop info_ptr) {
  png_read_info(png_ptr, info_ptr);
}

static __arm png_uint_32 png_get_IHDR_a(png_structp png_ptr, png_infop info_ptr, png_uint_32 *width, png_uint_32 *height, int *bit_depth, int *color_type,
  int *interlace_method, int *compression_method, int *filter_method)
{
  return png_get_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, interlace_method, compression_method, filter_method);
}

static __arm png_uint_32 png_get_valid_a(png_structp png_ptr, png_infop info_ptr, png_uint_32 flag) {
  return png_get_valid(png_ptr, info_ptr, flag);
}

static __arm void png_set_strip_16_a(png_structp png_ptr) {
  png_set_strip_16(png_ptr);
}

static __arm void png_set_gray_1_2_4_to_8_a(png_structp png_ptr) {
  png_set_gray_1_2_4_to_8(png_ptr);
}

static __arm void png_set_tRNS_to_alpha_a(png_structp png_ptr) {
  png_set_tRNS_to_alpha(png_ptr);
}

static __arm void png_read_end_a(png_structp png_ptr, png_infop info_ptr) {
  png_read_end(png_ptr, info_ptr);
}

static __arm void png_set_packing_a(png_structp png_ptr) {
  png_set_packing(png_ptr);
}

static __arm void png_set_gray_to_rgb_a(png_structp png_ptr) {
  png_set_gray_to_rgb(png_ptr);
}

static __arm void png_set_palette_to_rgb_a(png_structp png_ptr) {
  png_set_palette_to_rgb(png_ptr);
}

static __arm void png_set_filler_a(png_structp png_ptr, png_uint_32 filler, int flags) {
  png_set_filler(png_ptr, filler, flags);
}

static __arm void png_read_update_info_a(png_structp png_ptr, void *info_ptr) {
  png_read_update_info(png_ptr, info_ptr);
}

static __arm png_uint_32 png_get_rowbytes_a(png_structp png_ptr, png_infop info_ptr) {
  return png_get_rowbytes(png_ptr, info_ptr);
}

static __arm void png_read_row_a(png_structp png_ptr, png_bytep row, png_bytep dsp_row) {
  png_read_row(png_ptr, row, dsp_row);
}

static __arm int setjmp_a(jmp_buf jmpbuf) {
  return setjmp(jmpbuf);
}
/* ====================================================================== */

__thumb IMGHDR* create_imghdr(const char *fname, int type)
{
  int f;
  char buf[number];
  unsigned int err;
  struct PP
  {
    char *row;
    char *img;
    IMGHDR * img_h;
  } pp;
  IMGHDR * img_hc;
  png_structp png_ptr=NULL;
  png_infop info_ptr=NULL;
  png_uint_32 rowbytes;
  
  if ((f=fopen(fname, A_ReadOnly+A_BIN, P_READ, &err))==-1) return 0;
  pp.row=NULL;
  pp.img=NULL;
  pp.img_h=NULL;
  
  if (fread(f, &buf, number, &err)!=number) goto L_CLOSE_FILE;
  if  (!png_check_sig_a((png_bytep)buf,number)) goto  L_CLOSE_FILE;
  
  png_ptr = png_create_read_struct_2_a("1.2.5", (png_voidp)0, 0, 0, (png_voidp)0,(png_malloc_ptr)xmalloc,(png_free_ptr)xmfree);
  if (!png_ptr) goto L_CLOSE_FILE;
  
  info_ptr = (png_infop)png_create_info_struct_a(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct_a(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    goto L_CLOSE_FILE;
  }
  if (setjmp_a(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct_a(&png_ptr, &info_ptr, (png_infopp)NULL);
    goto L_CLOSE_FILE;
  }
  
  png_set_read_fn_a(png_ptr, (void *)f, read_data_fn);
  
  png_set_sig_bytes_a(png_ptr, number);
  
  png_read_info_a(png_ptr, info_ptr);
  
  png_uint_32 width, height;
  int bit_depth, color_type;
  
  png_get_IHDR_a(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);
  
  if (type==0)
  {
    if (color_type == PNG_COLOR_TYPE_GRAY) 
      type=PNG_1;
    else type = config->DEFAULT_COLOR+1;
  }
  
  if (bit_depth < 8) png_set_gray_1_2_4_to_8_a(png_ptr);
    
  if (png_get_valid_a(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha_a(png_ptr); 
  
  if (bit_depth == 16) png_set_strip_16_a(png_ptr);
  
  if (bit_depth < 8) png_set_packing_a(png_ptr);
  
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb_a(png_ptr);
  
  if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA || color_type == PNG_COLOR_TYPE_GRAY)
    png_set_gray_to_rgb_a(png_ptr);
  
  png_set_filler_a(png_ptr,0xFF,PNG_FILLER_AFTER);
  png_read_update_info_a(png_ptr, info_ptr);
  
  rowbytes = png_get_rowbytes_a(png_ptr, info_ptr);
  
  pp.row = malloc(rowbytes);
  pp.img_h = img_hc = malloc(sizeof(IMGHDR));
  
  if (type == PNG_1)
  {
    int rowc_w=(width+7)>>3;
    int size=height*rowc_w;
    unsigned char *iimg=(unsigned char *)(pp.img=malloc(size));
    memset(iimg, 0, size);
    for (unsigned int y = 0; y<height; ++y)
    {
      png_read_row_a(png_ptr, (png_bytep)pp.row, NULL);
      for (unsigned int x = 0; x<width; x++)
      {
        if (!pp.row[x*4+0] && !pp.row[x*4+1] && !pp.row[x*4+2])
          iimg[x>>3]|=(0x80>>(x&7));
      }
      iimg+=rowc_w;
    }
    pp.img_h->bpnum=1;
  }
  else
  {
    switch (type)
    {
    case PNG_8:
      {
        unsigned char *iimg=(unsigned char *)(pp.img=malloc(width*height));
        for (unsigned int y = 0; y<height; ++y)
        {
          png_read_row_a(png_ptr, (png_bytep)pp.row, NULL);
          for (unsigned int x = 0; x<width; ++x)
          {
            if (pp.row[x*4+3] < config->ALPHA_THRESHOLD)
              *iimg++=0xC0;
            else
            {
              unsigned char c=(pp.row[x*4+0] & 0xE0);
              c|=((pp.row[x*4+1]>>3)&0x1C);
              c|=((pp.row[x*4+2]>>6)&0x3);
              *iimg++=c;
            }
          }
        }
        pp.img_h->bpnum=5;
        break;
      }
    case PNG_16:
      {
        unsigned short *iimg=(unsigned short *)(pp.img=malloc(width*height*2));
        for (unsigned int y = 0; y<height; ++y)
        {
          png_read_row_a(png_ptr, (png_bytep)pp.row, NULL);
          for (unsigned int x = 0; x<width; ++x)
          {
            if (pp.row[x*4+3] < config->ALPHA_THRESHOLD)
              *iimg++=0xE000;
            else
            {
              unsigned int c=((pp.row[x*4+0]<<8)&0xF800);
              c|=((pp.row[x*4+1]<<3)&0x7E0);
              c|=((pp.row[x*4+2]>>3)&0x1F);
              *iimg++=c;
            }
          }
        }
        pp.img_h->bpnum=8;
        break;
      }
//#if NEWSGOLD || X75
    case PNG_24:
      {
        unsigned char *iimg=(unsigned char *)(pp.img=malloc((width*height)<<2));
        for (unsigned int y = 0; y<height; ++y)
        {
          png_read_row_a(png_ptr, (png_bytep)pp.row, NULL);
          for (unsigned int x = 0; x<width; ++x)
          {
            unsigned int c;
            *iimg++=pp.row[x*4+2];
            *iimg++=pp.row[x*4+1];
            *iimg++=pp.row[x*4+0];
            c=pp.row[x*4+3];
//          if (c>=128) c++;
//          c*=100;
//          c>>=8;
            *iimg++=c;//(pp.row[x*4+3]*100)/0xFF;
          }
        }
        pp.img_h->bpnum=0xA;
        break;
      }
//#endif
    }
  }
  pp.img_h->w=width;
  pp.img_h->h=height;
  //pp->img_h->zero=0;
  pp.img_h->bitmap=pp.img;
  
  png_read_end_a(png_ptr, info_ptr);
  png_destroy_read_struct_a(&png_ptr, &info_ptr, (png_infopp)NULL);
  if (!pp.img)
  {
  L_CLOSE_FILE:
    mfree(pp.row);
    mfree(pp.img);
    mfree(pp.img_h);
    fclose(f, &err);
    return NULL;
  }
  mfree(pp.row);
  fclose(f, &err);
  return (img_hc);
}

#pragma inline
static int tolower(int C)
{
  if ((C>='A' && C<='Z')) C-='A'-'a';
  return(C);
}

static char* strcpy_tolow(char *s1,const char *s2)
{
  while(*s2)
  {
    *s1++=tolower(*s2++);   
  }
  *s1=0;
  return s1;
}

#pragma optimize=no_inline
static void print10(char *s, unsigned int v)
{
  unsigned int buf=0xF;
  while(v>=10)
  {
    extern int __e_div(int delitelb, int delimoe);
    buf=(buf<<4)|(__e_div(10, v)/*v%10*/);
    v = udiv(10, v); //v/=10;
  }
  *s++=v+'0';
  while((v=buf&0x0F)<10) {*s++=v+'0'; buf>>=4;}
  *s++='.';
  *s++='p';
  *s++='n';
  *s++='g';
  *s='\0';
}


/* хешовый поиск картинок */
static unsigned int hash_strcpy(char *dst, const char* name)
{
    unsigned int hash = 0;
    unsigned int hi;
    /* два раза *name требует больше времени */
    register unsigned char c = *name++;

    while (c != '\0')
    {
        hash = (hash << 4) + c;
        hi = hash & 0xf0000000;
        hash ^= hi;
        hash ^= hi >> 24;

        *dst++ = c;
        c = *name++;
    }
    *dst = 0;
    return hash;
}

#pragma optimize=no_inline
// Парсим число из начала имени файла
static int ImgCache_GetIdFromFile(const char *filename) {
  int result = 0;
  bool digits_found = false;
  for (int i = 0; filename[i] != '\0'; i++) {
    char c = filename[i];
    if (c >= '0' && c <= '9') {
      digits_found = true;
      result = result * 10 + (c - '0');
    } else {
      if (c != '.')
        return -1;
      break;
    }
  }

  if (!digits_found)
    return -1;

  return result;
}

static unsigned char ImgCache_GetFlags(unsigned int index) {
  unsigned int byte_index = index >> 2;
  unsigned int shift = (index & 3) << 1;
  return (pngtop.bitmap[byte_index] >> shift) & 3;
}

static void ImgCache_SetFlagsNoLock(unsigned int index, unsigned char bit, bool value) {
  unsigned int byte_index = index >> 2;
  unsigned int shift = (index & 3) << 1;
  bit &= 3;
  pngtop.bitmap[byte_index] &= ~(bit << shift);
  if (value)
    pngtop.bitmap[byte_index] |= bit << shift;
}

__arm static void ImgCache_SetFlags(unsigned int index, unsigned char bit, bool value) {
    __direct_LockSched();
    ImgCache_SetFlagsNoLock(index, bit, value);
    __direct_UnlockSched();
}

__arm static void ImgCache_ScanFiles() {
  DIR_ENTRY de;
  unsigned int err;
  char mask[256];
  sprintf(mask, "%s*.png", config->IMAGES_PATH);

  __direct_LockSched();
  memset(pngtop.bitmap, 0, PIT_BITMAP_SIZE);
  DYNPNGICONLIST *dynp = pngtop.dyn_pltop2;
  while (dynp) {
      ImgCache_SetFlagsNoLock(dynp->icon, IMG_IS_DYNAMIC, true);
      dynp = dynp->next;
  }
  __direct_UnlockSched();

  if (FindFirstFile(&de, mask, &err)) {
    do {
      int pic = ImgCache_GetIdFromFile(de.file_name);
      if (pic >= 0 && pic < PIT_SIZE)
        ImgCache_SetFlags(pic, IMG_FILE_EXISTS, true);
    } while(FindNextFile(&de, &err));
  }
  FindClose(&de, &err);
}

void ImgCache_Init(void) {
  if (!pngtop.bitmap) {
    pngtop.bitmap = malloc(PIT_BITMAP_SIZE);
    ImgCache_ScanFiles();
  }
}

// Формируем путь по типу X:\ZBin\img\XXXX.png
static void ImgCache_GetPathToIMG(char *buffer, unsigned int pic) {
  print10(strcpy_tolow(buffer, config->IMAGES_PATH), pic);
}

static void ImgCache_Free(PNGLIST *item) {
  if (item->img) {
    mfree(item->img->bitmap);
    mfree(item->img);
  }
  mfree(item->pngname);
  mfree(item);
}

// Замена картинки в PIT по ID
void PIT_SetImage(unsigned int pic, IMGHDR *img) {
  DYNPNGICONLIST *new_item = malloc(sizeof(DYNPNGICONLIST));
  new_item->icon = pic;
  new_item->img = img;

  PIT_ResetImage(pic);

  __direct_LockSched();
  new_item->next = pngtop.dyn_pltop2;
  pngtop.dyn_pltop2 = new_item;
  __direct_UnlockSched();

  ImgCache_SetFlags(pic, IMG_IS_DYNAMIC, true);
}

// Вернуть стандартную картинку
void PIT_ResetImage(unsigned int pic) {
  __direct_LockSched();
  DYNPNGICONLIST *cursor = pngtop.dyn_pltop2;
  DYNPNGICONLIST *prev = NULL;
  while (cursor) {
    if (cursor->icon == pic) {
      if (prev) {
        prev->next = cursor->next;
      } else {
        pngtop.dyn_pltop2 = cursor->next;
      }
      mfree(cursor);
      __direct_UnlockSched();

      ImgCache_SetFlags(pic, IMG_IS_DYNAMIC, false);
      return;
    }
    prev = cursor;
    cursor = cursor->next;
  }
  __direct_UnlockSched();
}

// Очистить весь кэш
void PIT_ClearCache() {
  __direct_LockSched();
  PNGLIST *cursor = pngtop.pltop;
  pngtop.pltop = NULL;

  while (cursor) {
    PNGLIST *next = cursor->next;
    ImgCache_Free(cursor);
    cursor = next;
  }
  __direct_UnlockSched();

  ImgCache_ScanFiles();
}

// Медленный поиск по имени файла
static IMGHDR *ImgCache_FindByName(const char *fname) {
  __direct_LockSched();
  PNGLIST *cursor = pngtop.pltop;
  PNGLIST *prev = NULL;
  unsigned int hash = elfhash(fname);
  while (cursor) {
    if (cursor->hash == hash && __direct_strcmp(cursor->pngname, fname) == 0) {
      if (prev) {
        // Переносим элемент в начало списка
        prev->next = cursor->next;
        cursor->next = pngtop.pltop;
        pngtop.pltop = cursor;
      }
      __direct_UnlockSched();
      return cursor->img;
    }
    prev = cursor;
    cursor = cursor->next;
  }
  __direct_UnlockSched();
  return NULL;
}

// Быстрый поиск по ID (для PIT)
static IMGHDR *ImgCache_FindById(int icon) {
  __direct_LockSched();
  PNGLIST *cursor = pngtop.pltop;
  PNGLIST *prev = NULL;
  while (cursor) {
    if (cursor->icon == icon) {
      if (prev) {
        // Переносим элемент в начало списка
        prev->next = cursor->next;
        cursor->next = pngtop.pltop;
        pngtop.pltop = cursor;
      }
      __direct_UnlockSched();
      return cursor->img;
    }
    prev = cursor;
    cursor = cursor->next;
  }
  __direct_UnlockSched();
  return NULL;
}

// Поиск среди динамических изображений
static IMGHDR *ImgCache_FindDynImage(DYNPNGICONLIST *dynp, unsigned int pic) {
  while (dynp) {
    if (dynp->icon == pic) {
      IMGHDR *img = dynp->img;
      if (img)
        return img;
    }
    dynp = dynp->next;
  }
  return NULL;
}

static void ImgCache_Add(const char *fname, IMGHDR *img, int icon) {
  if (!pngtop.bitmap)
    return;

  // Новый элемент в кэше
  PNGLIST *new_item = malloc(sizeof(PNGLIST));
  new_item->pngname = malloc(strlen(fname) + 1);
  new_item->hash = hash_strcpy(new_item->pngname, fname);
  new_item->img = img;
  new_item->icon = icon;

  __direct_LockSched();

  // Добавляем в начало
  new_item->next = pngtop.pltop;
  pngtop.pltop = new_item;

  // Теперь подрезаем конец
  PNGLIST *cursor = pngtop.pltop;
  int count = 0;
  while (cursor) {
    count++;
    if (count >= config->CACHE_PNG)
      break;
    cursor = cursor->next;
  }

  if (!cursor) {
    // Пока ещё есть свободные элементы в списке
    __direct_UnlockSched();
    return;
  }

  // Обрезаем список, чтобы соблюдать заданный лимит
  PNGLIST *delete_cursor = cursor->next;
  cursor->next = NULL;
  __direct_UnlockSched();

  // Удаляем все лишние элементы
  while (delete_cursor) {
    PNGLIST *next = delete_cursor->next;
    ImgCache_Free(delete_cursor);
    delete_cursor = next;
  }
}

__arm IMGHDR *PatchGetPIT(unsigned int pic) {
  char fname[256];
  IMGHDR *img;

  // Передали строку в которой путь до картинки
  if ((pic >> 28) == 0xA) {
    strcpy_tolow(fname, (char *) pic);
    img = ImgCache_FindByName(fname);
    if (img)
      return img;
    img = create_imghdr(fname, 0);
    if (!img)
      return (IMGHDR *) &empty_img;
    ImgCache_Add(fname, img, -1);
    return img;
  }

  if (!pngtop.bitmap || pic >= PIT_SIZE)
    return NULL;

  unsigned char flags = ImgCache_GetFlags(pic);

  // Динамические картинки
  if (pngtop.dyn_pltop || (pngtop.dyn_pltop2 && (flags & IMG_IS_DYNAMIC))) {
    __direct_LockSched();
    img = ImgCache_FindDynImage(pngtop.dyn_pltop, pic);
    if (img) {
      __direct_UnlockSched();
      return img;
    }
    img = ImgCache_FindDynImage(pngtop.dyn_pltop2, pic);
    if (img) {
      __direct_UnlockSched();
      return img;
    }
    __direct_UnlockSched();
  }

  if (!(flags & IMG_FILE_EXISTS))
    return NULL;

  img = ImgCache_FindById(pic);
  if (img)
    return img;

  ImgCache_GetPathToIMG(fname, pic);

  img = create_imghdr(fname, 0);
  if (!img) {
    ImgCache_SetFlags(pic, IMG_FILE_EXISTS, false);
    return NULL;
  }

  ImgCache_Add(fname, img, pic);
  return img;
}

#pragma diag_suppress=Pe177
__root static const int SWILIB_FUNC1E9 @ "SWILIB_FUNC1E9" = (int)create_imghdr;
#pragma diag_default=Pe177
