#pragma once

#include <stdbool.h>
#include <inc/swilib.h>
#include <inc/pnglist.h>
#include <inc/png.h>

void ImgCache_Init(void);
void PIT_SetImage(unsigned int pic, IMGHDR *img);
void PIT_ResetImage(unsigned int pic);
void PIT_ClearCache();
