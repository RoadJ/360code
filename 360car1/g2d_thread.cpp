#include "g2d_thread.h"
#include "loopuptable.h"
#include <QDebug>
#include <QTime>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <QCoreApplication>

struct fb_fix_screeninfo FixedInfo;
struct fb_var_screeninfo OrigVarInfo;
static int FrameBufferFD = -1;
void *FrameBuffer = (void *) -1;

#define __ANDROID__

typedef struct fbdev{
    int fdfd; //open "dev/fb0"
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize;
    char *map_fb;

}FBDEV;

#ifndef __ANDROID__
#define FRAMEBUFFER "/dev/fb0"
#else
#define FRAMEBUFFER "/dev/graphics/fb0"
#endif //__ANDROID__

#define topCameraWhich 0
#define bottomCameraWhich 1
#define leftCameraWhich 2
#define rightCameraWhich 3

FBDEV fb_dev;

ushort rgb_24_2_565(int r, int g, int b)
{
    return (ushort)(((unsigned(r) << 8) & 0xF800) |
            ((unsigned(g) << 3) & 0x7E0)  |
            ((unsigned(b) >> 3)));
}

void init_dev(FBDEV *dev)
{
    FBDEV *fr_dev=dev;

    fr_dev->fdfd=open(FRAMEBUFFER,O_RDWR);
    //qDebug("the framebuffer device was opended successfully.\n");

    /* Get the fixed screen info */
    if (ioctl(fr_dev->fdfd, FBIOGET_FSCREENINFO, &(fr_dev->finfo)))
    {
        //qDebug("error: ioctl(FBIOGET_FSCREENINFO) failed\n");
        exit(1);
    }

    ioctl(fr_dev->fdfd,FBIOGET_VSCREENINFO,&(fr_dev->vinfo)); //获取可变参数

    fr_dev->screensize=fr_dev->vinfo.xres*fr_dev->vinfo.yres*fr_dev->vinfo.bits_per_pixel/8;

    fr_dev->map_fb=(char *)mmap(NULL,fr_dev->finfo.smem_len,PROT_READ|PROT_WRITE,MAP_SHARED,fr_dev->fdfd,0);

    //qDebug()<<"fr_dev->vinfo.bits_per_pixel = " <<fr_dev->vinfo.bits_per_pixel;

    //qDebug("init_dev successfully.\n");
}

void draw_dot(FBDEV *dev,int x,int y,unsigned char r,unsigned char g,unsigned char b) //(x.y) 是坐标
//void draw_dot(FBDEV *dev,int x,int y) //(x.y) 是坐标
{
    FBDEV *fr_dev=dev;
    int *xx=&x;
    int *yy=&y;

    long int location=0;
    location=(*xx+fr_dev->vinfo.xoffset)*(fr_dev->vinfo.bits_per_pixel/8)+
                 (*yy+fr_dev->vinfo.yoffset)*fr_dev->finfo.line_length;
    //int b=100;
    //int g=100;
    //int r=100;
    unsigned short int t=r<<11|g<<5|b;
    *((unsigned short int *)(fr_dev->map_fb+location))=t;
}



//QImage  *finishImage;

G2DThread::G2DThread(QObject *parent)
{
//    fb_dev.fdfd=-1;
//    init_dev(&fb_dev);


    status = false;

    int out_buffer_size = srcWidth * srcheight *4;

      g_g2d_src_cale_buffer = g2d_alloc(out_buffer_size, 0);

        if (g_g2d_src_cale_buffer == 0)
        {
            qDebug("can not malloc memory for g2d dst\n");
            g2d_free(g_g2d_src_cale_buffer);
            exit(-1);
        }

        memset(g_g2d_src_cale_buffer->buf_vaddr,0x0,out_buffer_size);

        g_g2d_desc_cale_buffer = g2d_alloc(out_buffer_size, 0);

          if (g_g2d_desc_cale_buffer == 0)
          {
              qDebug("can not malloc memory for g2d dst\n");
              g2d_free(g_g2d_desc_cale_buffer);
              exit(-1);
          }

    memset(g_g2d_desc_cale_buffer->buf_vaddr,0x0,out_buffer_size);

    timer = new QTimer(this);
    timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(cleanFrameSpeed()));

    for (int i=0;i<bufferCount;i++)
    {
        stitchingImage[i] = new QImage(imageWidth,imageHeight,QImage::Format_ARGB32);
    }

    finishImage = stitchingImage[0];

    cameraFourInThread = new CameraWorkFourin();
    cameraFourInThread->setCameraData(&cameraBuffer);
    connect(cameraFourInThread,SIGNAL(Frame(unsigned int)),this,SLOT(copyData(unsigned int)));
    cameraFourInThread->start();
}

void G2DThread::setWhichCamera(int whichCamera)
{
    this->whichCamera = whichCamera;
}

void G2DThread::cleanFrameSpeed()
{
    qDebug() << "G2DThread::countFrameSpeed = " << countFrameSpeed;
    countFrameSpeed = 0;
}

#if 0
void G2DThread::copyData(unsigned int frameOffset)
{
    int nr= imageHeight; // number of rows
    int nc= imageWidth; // number of columns
    int lenght = 4;
    char* destdata;
    int j =0;
    int i =0;
    int x = 0;
    int y = 0;
    uchar* srcdata;
    tablePos tempTablePos;
    int widthOfLine = srcWidth*lenght;

    //if (mutex.tryLock(1))
    {
        //QTime time;
        //time.start();

        //qDebug()<<time.elapsed()/1000.0<<"s";

        //QTime time;
        //time.start();
            for (j=0; j<nr; j++)
            {
                    destdata = (char*)stitchingImage[frameOffset]->scanLine(j);

                    for (i=0; i<nc; i++)
                    {
                            tempTablePos = loopupTable[j][i];

                            x = tempTablePos.x;
                            y = tempTablePos.y;

                            if (!( (x == 0) && (y == 0) ))
                            {
                                srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr))+(((y))*widthOfLine)+((x)*lenght);
                                *((int*)destdata) = *((int*)srcdata) ;
                            }

                            destdata += lenght;

                    } // end of row
            }
            //qDebug()<<time.elapsed()/1000.0<<"s";
            //QTime time;
            //time.start();
            finishImage = stitchingImage[frameOffset];
            //fisheyePixmap= QPixmap::fromImage(*finishImage);
            emit caleFinish(frameOffset);
            //update();
            //qDebug()<<time.elapsed()/1000.0<<"s";
        countFrameSpeed++;
        //mutex.unlock();
        //qDebug()<<time.elapsed()/1000.0<<"s";
    }

}
#endif

void G2DThread::copyData(unsigned int frameOffset)
{
    int nr= imageHeight; // number of rows
    int nc= imageWidth; // number of columns
    int lenght = 4;
    char* destdata;
    int j =0;
    int i =0;
    int x = 0;
    int y = 0;
    uchar* srcdata;
    tablePos tempTablePos;
    int b;
    int g;
    int r;



    unsigned short int t;
    long int location=0;
    int widthOfLine = srcWidth*4;



    if (status == false)
    {
        status = true;

        fb_dev.fdfd=-1;
        init_dev(&fb_dev);

//        QTime time;
//        time.start();

        //qDebug()<<time.elapsed()/1000.0<<"s";
        //memset(fb_dev.map_fb, 0xff, fb_dev.finfo.smem_len);
        //QTime time;
        //time.start();
#if 1
            //拼接完后的图像。
            for (j=0; j<nr; j++)
            {

                 QCoreApplication::processEvents();

                    unsigned int offsetLine = (j+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;
                    for (i=0; i<nc; i++)
                    {
                            tempTablePos = loopupTable[j][i];

                            x = tempTablePos.x;
                            y = tempTablePos.y;

                            if (!( (x == 0) && (y == 0) ))
                            {
                                srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr))+((y)*widthOfLine)+((x)*lenght);
                                location=(i+(fb_dev.vinfo.xoffset))*(fb_dev.vinfo.bits_per_pixel/8)+offsetLine;
                                *((unsigned int *)(fb_dev.map_fb+location))=*((unsigned int *)(srcdata));//rgb_24_2_565(r,g,b);
//                                memcpy(fb_dev.map_fb+location,srcdata,4);
                            }
                    } // end of row
            }
#endif

#if 0
                    //鱼眼效果图
                    //srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr))+(((j))*widthOfLine)+((i)*lenght);
                    srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;

                    for (j=0; j<srcheight; j++)
                    {
                            int offsetLine = (j+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;

                            //i = 500;
                            location=(500+fb_dev.vinfo.xoffset)*(fb_dev.vinfo.bits_per_pixel/8)+offsetLine;
                            memcpy(fb_dev.map_fb+location,srcdata,srcWidth*4);
                            srcdata += srcWidth * 4;
                    }

 #endif

//                    QTime time;
//                    time.start();
#if 1
                    //朝向向左
                    if (whichCamera == topCameraWhich)
                    {
                        int tmpCounti = 0;
                        int heightLimit = srcheight - 600;
                        srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
                        for(int i=0;i<srcheight;i++)
                        {
                            QCoreApplication::processEvents();
                            int basePoint =  srcWidth/2;

                            if (i > heightLimit)
                            {
                                uchar * tmpdata;
                                int offsetLine = (tmpCounti+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;
                                int tmpCountj = 0;
                                for(int j=0;j<srcWidth;j++)
                                {
                                    if ( (j>(basePoint-(800/2)))&& (j<(basePoint+(800/2))))
                                    {
                                        int tmpOffset;
                                        tmpOffset = (800);
                                        tmpdata =  srcdata + j * 4;
                                        location=(480+tmpCountj+fb_dev.vinfo.xoffset)*(fb_dev.vinfo.bits_per_pixel/8)+offsetLine;
//                                        *((unsigned int *)(fb_dev.map_fb+location))=*((unsigned int *)(tmpdata));//rgb_24_2_565(r,g,b);
                                        memcpy(fb_dev.map_fb+location,tmpdata,tmpOffset*4);
                                        j+=tmpOffset;
                                        tmpCountj++;
                                        break;
                                    }
                                }
                                tmpCounti++;
                            }
                            srcdata += srcWidth * 4;
                        }
                    }


                    //朝向向右
                    if (whichCamera == bottomCameraWhich)
                    {
                        int tmpCounti = 0;
                        int heightLimit = 600;
                        srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
                        memcpy( g_g2d_src_cale_buffer->buf_vaddr, srcdata,  srcheight*widthOfLine);
                        matrixRotate(g_g2d_src_cale_buffer,srcWidth,srcheight,g_g2d_desc_cale_buffer,srcWidth,srcheight,G2D_ROTATION_180);

                        srcdata = ((uchar *)(g_g2d_desc_cale_buffer->buf_vaddr));
                        for(int i=0;i<srcheight;i++)
                        {
                            QCoreApplication::processEvents();
                            int basePoint =  srcWidth/2;

                            if (i < heightLimit)
                            {
                                uchar * tmpdata;
                                int offsetLine = (tmpCounti+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;
                                int tmpCountj = 0;
                                for(int j=0;j<srcWidth;j++)
                                {
                                    if ( (j>(basePoint-(800/2)))&& (j<(basePoint+(800/2))))
                                    {
                                        int tmpOffset;
                                        tmpOffset = (800);
                                        tmpdata =  srcdata + j * 4;
                                        location=(480+tmpCountj+fb_dev.vinfo.xoffset)*(fb_dev.vinfo.bits_per_pixel/8)+offsetLine;
//                                        *((unsigned int *)(fb_dev.map_fb+location))=*((unsigned int *)(tmpdata));//rgb_24_2_565(r,g,b);
                                        memcpy(fb_dev.map_fb+location,tmpdata,tmpOffset*4);
                                        j+=tmpOffset;
                                        tmpCountj++;
                                    }
                                }
                                tmpCounti++;
                            }
                            srcdata += srcWidth * 4;
                        }
                    }


                    //朝向向前
                    if (whichCamera == leftCameraWhich)
                    {
//                                            QTime time;
//                                            time.start();

                        srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
                        memcpy( g_g2d_src_cale_buffer->buf_vaddr, srcdata,  srcheight*widthOfLine);

//                        qDebug()<<time.elapsed()/1000.0<<"s";

//                        QTime time;
//                        time.start();

                        matrixRotate(g_g2d_src_cale_buffer,srcWidth,srcheight,g_g2d_desc_cale_buffer,srcheight,srcWidth,G2D_ROTATION_270);
                        srcdata = ((uchar *)(g_g2d_desc_cale_buffer->buf_vaddr));


//                        qDebug()<<time.elapsed()/1000.0<<"s";

                        int tmpCounti = 0;
                        int heightLimit = 800-800;
                        int basePoint =  1280/2;
                        for(int i=0;i<1280;i++)
                        {
                //            int basePoint =  imgRatate->width()/2;

                            if ( (i > (basePoint - (600/2)))&& (i < (basePoint + (600/2))) )
//                            if ( i< heightLimit)
                            {
                                int offsetLine = (tmpCounti+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;
                                int tmpCountj = 0;
                                uchar * tmpdata;
                                for(int j=0;j<800;j++)
                                {
                                    if ( j> heightLimit)
//                                       if ( (j > (basePoint - (600/2)))&& (j < (basePoint + (600/2))) )
                                    {
                                        tmpdata =  srcdata + j * 4;
                                        location=(480+tmpCountj+fb_dev.vinfo.xoffset)*(fb_dev.vinfo.bits_per_pixel/8)+offsetLine;
//                                        *((unsigned int *)(fb_dev.map_fb+location))=*((unsigned int *)(tmpdata));//rgb_24_2_565(r,g,b);
                                        memcpy(fb_dev.map_fb+location,tmpdata,800*4);
                                        j=800;
                                        tmpCountj++;
                                       break;
                                    }
                                }

                                tmpCounti++;
                            }
                            srcdata += srcheight * 4;
                        }
                    }

                    //朝向向后
                    if (whichCamera == rightCameraWhich)
                    {
                        srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
                        memcpy( g_g2d_src_cale_buffer->buf_vaddr, srcdata,  srcheight*widthOfLine);
                        matrixRotate(g_g2d_src_cale_buffer,srcWidth,srcheight,g_g2d_desc_cale_buffer,srcheight,srcWidth,G2D_ROTATION_90);
                        srcdata = ((uchar *)(g_g2d_desc_cale_buffer->buf_vaddr));

                        int tmpCounti = 0;
                        int heightLimit = 0;
                        int basePoint =  1280/2;
                        for(int i=0;i<1280;i++)
                        {
                //            int basePoint =  imgRatate->width()/2;

                            if ( (i > (basePoint - (600/2)))&& (i < (basePoint + (600/2))) )
//                            if ( i< heightLimit)
                            {
                                int offsetLine = (tmpCounti+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;
                                int tmpCountj = 0;
                                uchar * tmpdata;
                                for(int j=0;j<800;j++)
                                {
                                    if ( j > heightLimit)
//                                       if ( (j > (basePoint - (600/2)))&& (j < (basePoint + (600/2))) )
                                    {
                                        tmpdata =  srcdata + j * 4;
                                        location=(480+tmpCountj+fb_dev.vinfo.xoffset)*(fb_dev.vinfo.bits_per_pixel/8)+offsetLine;
//                                        *((unsigned int *)(fb_dev.map_fb+location))=*((unsigned int *)(tmpdata));//rgb_24_2_565(r,g,b);
                                         memcpy(fb_dev.map_fb+location,tmpdata,800*4);
                                        tmpCountj++;
                                        j=800;
                                        break;
                                    }
                                }

                                tmpCounti++;
                            }
                            srcdata += srcheight * 4;
                        }
                    }

#endif
//        qDebug()<<time.elapsed()/1000.0<<"s";
        countFrameSpeed++;

        munmap(fb_dev.map_fb,fb_dev.finfo.smem_len);
        ::close(fb_dev.fdfd);

        status = false;
    }
}

G2DThread::~G2DThread()
{
    cameraFourInThread->exit(0);
}
