#include "loader3/loader.h"
#include <inc/pnglist.h>
#include <inc/png.h>
#include <inc/swilib.h>
#include "config_struct.h"


#define number 8

const char Pointer[1]={0xFF};
const IMGHDR empty_img = {0,0,0x1,(char *)Pointer};

static __arm void LockSched_a() {
  LockSched();
}

static __arm void UnlockSched_a() {
  UnlockSched();
}

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

volatile PNGTOP_DESC pngtop = { 0 }; 

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


/* ������� ����� �������� */
static unsigned int hash_strcpy(char *dst, const char* name)
{
    unsigned int hash = 0;
    unsigned int hi;
    /* ��� ���� *name ������� ������ ������� */
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


/**
 * � ��� � �� ����� ��� ��� ��������, �����-�� ������������ ����������. ����-�� ������������� %)
 * ������� ������ ������� �������� �� ����.
 */

#pragma optimize=no_inline
static IMGHDR *find_png_in_cache(const char *png_name)
{
  PNGLIST *pl;
  PNGLIST *pl_prev;
  LockSched_a();
  pl=(PNGLIST *)(&(pngtop.pltop));
  pl_prev=NULL;
  unsigned int hash = elfhash(png_name);
  
  while((pl = pl->next))
  {
    /* ���� ��� ������, ������� �� ��� */
    if (pl->hash == hash && !__direct_strcmp(pl->pngname, png_name))
    {
      //������, ��������� � ������ � �������
      if (pl_prev)
      {
        //������ ���� �� � ����� ������
        pl_prev->next = pl->next; //������� �� ��������� �����
        pl->next = (PNGLIST *)(pngtop.pltop); //��������� - ���� ������
        pngtop.pltop = pl; //� ������ � ������ - ��������
      }
      UnlockSched_a();
      return(pl->img);
    }
    pl_prev = pl; //������� ������������ - ������ ����������
  }
  UnlockSched_a();
  return (0);
}


static IMGHDR *add_png_in_cache(const char *fname, IMGHDR *img)
{
  PNGLIST *cur = malloc(sizeof(PNGLIST)), *pl_prev; //������� ������� ������
  cur->pngname = malloc(strlen(fname)+1);
  
  /* �������� ��� � ������� ��� */
  cur->hash = hash_strcpy(cur->pngname, fname);
  
  cur->img = img;
  int i = 0; //��� ���������� ��������� � ������
  LockSched_a();
  cur->next=(PNGLIST *)(pngtop.pltop); //��������� - ���� ������
  pngtop.pltop=cur; //������ � ������ - ����� �������
  //������ ��������� �����
  PNGLIST *pl=(PNGLIST *)(&(pngtop.pltop));
  do
  {
    pl_prev=pl;
    pl=pl->next;
    if (!pl)
    {
      //����������� �������� ������
      UnlockSched_a();
      return (cur->img);
    }
    i++;
  }
  while(i<= config->CACHE_PNG); //���� ���������� ��������� ������ �����������
  pl_prev->next=NULL; //������� ������
  UnlockSched_a();
  //��������� ����� ������� � ����������� ����������������
  do
  {
    //������� �������
    if (pl->img)
    { 
      mfree(pl->img->bitmap);
      mfree(pl->img);
    }
    mfree(pl->pngname);
    pl_prev=pl;
    pl=pl->next;
    mfree(pl_prev);
  }
  while(pl); //���� ���� ��������, ����������� ��
  return img;
}



__arm IMGHDR* PatchGetPIT(unsigned int pic)
{
  IMGHDR * img = 0;
  unsigned int i;
  char fname[256];

  unsigned int mask80;
  unsigned int mask40;
  char *bp;

  // ���� ELFPack ��� �� ����������, ��, ��� �� �����, ��� ��������� �������� �� ������� ���� (��� ����)
  if (!pngtop.bitmap) {
    if ((pic >> 28) == 0xA) {
      strcpy_tolow(fname, (char *) pic);
      img = create_imghdr(fname, 0);
      return img ? img : (IMGHDR *) &empty_img;
    }
    return NULL;
  }

  if ((pic>>28)==0xA)
  {
    strcpy_tolow(fname, (char*)pic);
    img = find_png_in_cache(fname);
    if (img) return (img);
    img=create_imghdr(fname,0);
    if (!img) return ((IMGHDR *)&empty_img);
  }
  else
  {
    //���� � ������ ������������ ������
    {
      DYNPNGICONLIST *dynp;
      LockSched();
      dynp=pngtop.dyn_pltop;
      while(dynp)
      {
        if (dynp->icon==pic)
        {
          IMGHDR *i=dynp->img;
          if (i)
          {
            UnlockSched();
            return(i);
          }       
        }
        dynp=dynp->next;
      }
      UnlockSched();
    }
    if ((pic<20000))
    {
      mask40=(mask80=0x80UL>>((pic&3)<<1))>>1;
      bp=pngtop.bitmap+(pic>>2);
      if ((i=*bp)&mask80)  // ���� ������ � �������
      {
        if (i&mask40)  
        {
          char *next=strcpy_tolow(fname, config->IMAGES_PATH); // �������� ����� ��� ���� �� �����
          //*fname=DEFAULT_DISK_N+'0';
          print10(next,pic);
          img=find_png_in_cache(fname);
          if (img) return (img);
          img=create_imghdr(fname,0);          
        } 
        else return(0);                                // �������� ��� - �������
      }
      else 
      {
        LockSched();
        *bp|=mask80; // ������ ���, ������ ���� ��� ����
        UnlockSched();
        char *next=strcpy_tolow(fname, config->IMAGES_PATH);
        //*fname=DEFAULT_DISK_N+'0';
        print10(next,pic);
        img=find_png_in_cache(fname);
        if (img)
        {
          LockSched();
          *bp|=mask40;
          UnlockSched();
          return (img);
        }
        img=create_imghdr(fname,0);                 // ������� �������
        if (img)
        {
          LockSched();
          *bp|=mask40;
          UnlockSched();
        }
        else  return (0);
      }
    }
    else return(0);
  }
  //������ �� �����, ������ ������� ��������
  
  //if (!img) return (0); //������ ���������

  return add_png_in_cache(fname, img);
}

void InitPngBitMap(void)
{
  if (!pngtop.bitmap)
  {
    pngtop.bitmap=malloc(20000/8*2);
  }
  memset((void*)(pngtop.bitmap), 0,20000/8*2);
}

#pragma diag_suppress=Pe177
__root static const int SWILIB_FUNC1E9 @ "SWILIB_FUNC1E9" = (int)create_imghdr;
#pragma diag_default=Pe177
