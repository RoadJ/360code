#ifndef G2D_YUYV_BGRA_H
#define G2D_YUYV_BGRA_H

#include "g2d.h"

void YUYV2BGRA(struct g2d_buf *buf_yuyv,int img_width,int img_height,
              struct g2d_buf *buf_bgra,int disp_width,int disp_height,int rot);

#endif // G2D_YUYV_RGB_H
