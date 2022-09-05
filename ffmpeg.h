/************************************************************************/
/* By: Chen Yu (chenyu@visionular.com)                                  */
/* version 1.0 (2022-08-20)                                             */
/* 1. Initial version.                                                  */
/************************************************************************/
#ifndef __FFMPEG_H__
#define __FFMPEG_H__

#ifndef OUT
#   define OUT
#   define IN
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#define __USE_LARGEFILE64  1
#define __LARGE64_FILES
#include <sys/stat.h>

typedef unsigned char uint8, Byte;

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#define stat64 _stat64
#define fseek64 _fseeki64
#define forceinline      __forceinline
typedef           __int64    int64;
typedef  unsigned __int64    uint64;
#define IO_NULL "nul"
#define IO_R "rb"
#define IO_W "wb"
#define FFMPEG_BIN "ffmpeg"
#else
#define IO_NULL "/dev/null"
#define stricmp strcasecmp
#define strnicmp strncasecmp
#  ifdef __ANDROID__
#pragma message("__ANDROID__ defined")
#define fseek64 fseeko
#  else
#define fseek64 fseeko64
#  endif
#define forceinline  inline __attribute__((always_inline))
typedef           long long  int64;
typedef  unsigned long long  uint64;
#define IO_R "r"
#define IO_W "w"
#define FFMPEG_BIN "ffmpeg"
#endif

#define IS_NUMBER(x) ('0'<=(x) && (x)<='9')
#ifndef MAX
#   define MAX(a, b) ((a)<(b)?(b):(a))
#   define MIN(a, b) ((a)<(b)?(a):(b))
#   define MAX3(a,b,c) MAX(MAX(a,b),c)
#   define MIN3(a,b,c) MIN(MIN(a,b),c) 
#endif
#define BETWEEN(x, xMin, xMax) ((x)<(xMin)?(xMin):((x)>(xMax)?(xMax):(x)))

#define LIBX264 "libx264"
#define LIBX265 "libx265"

#define SDR_PARAM_BT709  "colorprim=bt709:transfer=bt709:colormatrix=bt709"
#define HDR_PARAM_HLG "colorprim=bt2020:transfer=arib-std-b67:colormatrix=bt2020nc"
#define HDR_PARAM_PQ  "colorprim=bt2020:transfer=smpte2084:colormatrix=bt2020nc"
#define MAKE_X265_PARAM(hdr) "-x265-params " hdr " -tag:v hvc1 "
#define MAKE_X264_PARAM(hdr) "-x264-params " hdr " "

#define FF_CRF_AUTO -1

typedef enum FFPixFmt
{
    FF_NONE=-1, FF_MIN, FF_YUV=FF_MIN,
    FF_I420, FF_I422, FF_I444, FF_J420, FF_J422, FF_J444,
    FF_I420P10, FF_I422P10, FF_I444P10, FF_BGR48, FF_RGB48,
    FF_BGR, FF_RGB, FF_BGRA, FF_RGBA, FF_ABGR, FF_ARGB,
    FF_GRAY, FF_MAX=FF_GRAY
}FFPixFmt;
typedef enum FFColSpc
{
    FF_COL_SPC_UNKNOWN, FF_COL_SPC_BT709, FF_COL_SPC_BT601, FF_COL_SPC_BT2020, FF_COL_SPC_SMPTE240M, FF_COL_SPC_MAX=FF_COL_SPC_SMPTE240M
}FFColSpc;
// gamut
typedef enum FFColPri
{
    FF_COL_PRI_UNKNOWN, FF_COL_PRI_BT709, FF_COL_PRI_BT601PAL, FF_COL_PRI_BT601NTSC, FF_COL_PRI_BT2020, FF_COL_PRI_BT470M, FF_COL_PRI_MAX=FF_COL_PRI_BT470M
}FFColPri;
// EOTF
typedef enum FFColTrc
{
    FF_COL_TRC_UNKNOWN, FF_COL_TRC_BT709, FF_COL_TRC_BT601, FF_COL_TRC_HLG, FF_COL_TRC_PQ, FF_COL_TRC_SMPTE240M, FF_COL_TRC_MAX=FF_COL_TRC_SMPTE240M
}FFColTrc;
typedef struct FFInfo
{
    FFPixFmt pixfmt;
    int width, height;
    double fps;
    double sec;
    int frame_num;
    float total_bitrate, video_bitrate;
    FFColSpc col_spc;
    FFColPri col_pri;
    FFColTrc col_trc;
    char pCodec[16];
}FFInfo;

#define ffmpeg_is_10bit(pixfmt) (FF_I420P10<=(pixfmt) && (pixfmt)<=FF_RGB48)
#define ffmpeg_bit_depth(pixfmt) (ffmpeg_is_10bit(pixfmt)?10:8)
#define ffmpeg_yuv_channel(pixfmt) ((pixfmt)==FF_GRAY?1:((FF_BGRA<=(pixfmt) && (pixfmt)<=FF_ARGB)+3))
#define ffmpeg_yuv_isRGB(pixfmt) (FF_BGR48<=(pixfmt) && (pixfmt)<=FF_ARGB)
static forceinline FFPixFmt ffmpeg_yuv_set_default_pixfmt(FFPixFmt pixfmt)
{
    static FFPixFmt def_fmt=FF_I420;
    return def_fmt=(pixfmt>FF_YUV?pixfmt:def_fmt);
}
static forceinline FFPixFmt ffmpeg_yuv_get_default_pixfmt()
{
    return ffmpeg_yuv_set_default_pixfmt(FF_NONE);
}
static int ffmpeg_is_imagefile(const char* pName)
{
    static const char* ppName[]={".jpg", ".jpe", ".jpeg", ".png", ".bmp", ".webp"};
    const int num=(int)(sizeof(ppName)/sizeof(*ppName));
    const char* p = strrchr(pName, '.');
    if(p)
    {
        for(int i=0; i<num; i++)
        {
            if(stricmp(p, ppName[i])==0)
                return 1;
        }
    }
    return 0;
}
static int ffmpeg_is_videofile(const char* pName)
{
    static const char* ppName[]={".mp4", ".avi", ".rm", ".rmvb", ".wmv", ".flv", ".mov", ".3GP", ".mkv", ".asf", ".ts", ".mpeg", ".webm", ".264", ".h264", ".x264", ".avc", ".265", ".h265", ".x265", ".hevc", ".ivf", ".av1", ".mxf", ".y4m",
        ".I420", ".I422", ".I444", ".yuv", ".I420P10", ".I422P10", ".I422H10", ".I444P10", ".gif", ".jpg", ".jpe", ".jpeg", ".png", ".bmp", ".webp"};
    const int num=(int)(sizeof(ppName)/sizeof(*ppName));
    const char* p = strrchr(pName, '.');
    if(p)
    {
        for(int i=0; i<num; i++)
        {
            if(stricmp(p, ppName[i])==0)
                return 1;
        }
    }
    return 0;
}
static const char* ffmpeg_pixfmt2string(const FFPixFmt pixfmt)
{
    static const char* pPixFmt[]={"unknown", "yuv", "yuv420p", "yuv422p", "yuv444p", "yuvj420p", "yuvj422p", "yuvj444p", "yuv420p10le", "yuv422p10le", "yuv444p10le", "bgr48le", "rgb48le", "bgr24", "rgb24", "bgra", "rgba", "abgr", "argb", "gray"};
    return pPixFmt[pixfmt-FF_NONE];
}
static const FFPixFmt ffmpeg_string2pixfmt(const char* pPixfmt)
{
    for(int i=FF_MIN; i<=FF_MAX; i++)
    {
        if(stricmp(ffmpeg_pixfmt2string((FFPixFmt)i), pPixfmt)==0)
            return (FFPixFmt)i;
    }
    return FF_NONE;
}
static const FFPixFmt ffmpeg_yuv_str2pixfmt(const char* pPixFmt)
{
    static const char* ppFmt[]={"yuv", "I420", "I422", "I444", "J420", "J422", "J444", "I420P10", "I422P10", "I444P10", "BGR48", "RGB48", "BGR", "RGB", "BGRA", "RGBA", "ABGR", "ARGB", "Gray"};
    const int num=(int)(sizeof(ppFmt)/sizeof(*ppFmt));
    for(int i=num-1; i>=0; i--)
    {
        size_t n=strlen(ppFmt[i]), offset=(ppFmt[i][0]=='I');
        if(strnicmp(pPixFmt, ppFmt[i], n)==0 || strnicmp(pPixFmt+3, ppFmt[i]+offset, n-offset)==0)
            return (FFPixFmt)i;
    }
    return FF_NONE;
}
#define ffmpeg_yuv_half_height(pixfmt) ((pixfmt)==FF_I420 || (pixfmt)==FF_I420P10 || (pixfmt)==FF_J420)
#define ffmpeg_yuv_half_width(pixfmt) ((pixfmt)==FF_I420 || (pixfmt)==FF_I422 || (pixfmt)==FF_I420P10 || (pixfmt)==FF_I422P10 || (pixfmt)==FF_J420 || (pixfmt)==FF_J422)
static forceinline int ffmpeg_yuv_compute_frame_size(int width, int height, FFPixFmt pixfmt)
{
    int scale=ffmpeg_is_10bit(pixfmt)+1, yshift=ffmpeg_yuv_half_height(pixfmt), xshift=ffmpeg_yuv_half_width(pixfmt);
    int dataSize=width*height+(height>>yshift)*(width>>xshift)*(ffmpeg_yuv_channel(pixfmt)-1);
    return dataSize*scale;
}
static forceinline int64 ffmpeg_yuv_get_filesize(const char* pFileName)
{
    struct stat64 buf;
    if(stat64(pFileName, &buf)!=0)
    {
        printf("Get '%s' size failed\n", pFileName);
        return 0;
    }
    int64 fileSize=(int64)buf.st_size;
    return fileSize;
}

#define ffmpeg_yuv_frame_num(pFileName, width, height, pixfmt) (int)(ffmpeg_yuv_get_filesize(pFileName)/ffmpeg_yuv_compute_frame_size(width, height, pixfmt))
static FFColSpc _ffmpeg_str2ColorSpace(const char* pName)
{
    if(strcmp(pName, "bt709")==0) return FF_COL_SPC_BT709;
    if(strcmp(pName, "bt470bg")==0 || strcmp(pName, "smpte170m")==0) return FF_COL_SPC_BT601;
    if(strcmp(pName, "bt2020nc")==0 || strcmp(pName, "bt2020c")==0) return FF_COL_SPC_BT2020;
    if(strcmp(pName, "smpte240m")==0) return FF_COL_SPC_SMPTE240M;
    return FF_COL_SPC_UNKNOWN;
}
static FFColPri _ffmpeg_str2ColorPrims(const char* pName)
{
    if(strcmp(pName, "bt709")==0) return FF_COL_PRI_BT709;
    if(strcmp(pName, "bt470bg")==0) return FF_COL_PRI_BT601PAL;
    if(strcmp(pName, "smpte170m")==0 || strcmp(pName, "smpte240m")==0) return FF_COL_PRI_BT601NTSC;
    if(strcmp(pName, "bt2020")==0) return FF_COL_PRI_BT2020;
    if(strcmp(pName, "bt470m")==0) return FF_COL_PRI_BT470M;
    return FF_COL_PRI_UNKNOWN;
}
static FFColTrc _ffmpeg_str2ColorTrans(const char* pName)
{
    if(strcmp(pName, "bt709")==0) return FF_COL_TRC_BT709;
    if(strcmp(pName, "smpte170m")==0) return FF_COL_TRC_BT601;
    if(strcmp(pName, "arib-std-b67")==0) return FF_COL_TRC_HLG;
    if(strcmp(pName, "smpte2084")==0) return FF_COL_TRC_PQ;
    if(strcmp(pName, "smpte240m")==0) return FF_COL_TRC_SMPTE240M;
    return FF_COL_TRC_UNKNOWN;
}

#define FF_COL_SPC_LIST_NAME {"unknown", "bt709", "bt601", "bt2020nc", "smpte240m"}
#define FF_COL_PRI_LIST_NAME {"unknown", "bt709", "bt470bg", "smpte170m", "bt2020", "bt470m"}
#define FF_COL_TRC_LIST_NAME {"unknown", "bt709", "smpte170m", "HLG", "PQ", "smpte240m"}
static const char* ffmpeg_GetColorSpaceName(FFColSpc spc) { const char* ppName[]=FF_COL_SPC_LIST_NAME; return ppName[BETWEEN(spc, FF_COL_SPC_UNKNOWN, FF_COL_SPC_MAX)]; }
static const char* ffmpeg_GetColorPrimsName(FFColPri pri) { const char* ppName[]=FF_COL_PRI_LIST_NAME; return ppName[BETWEEN(pri, FF_COL_PRI_UNKNOWN, FF_COL_PRI_MAX)]; }
static const char* ffmpeg_GetColorTransName(FFColTrc trc) { const char* ppName[]=FF_COL_TRC_LIST_NAME; return ppName[BETWEEN(trc, FF_COL_TRC_UNKNOWN, FF_COL_TRC_MAX)]; }

// file name should like this: "xxxx_wxh_fps.fmt.yuv" or "xxxx_wxh_fps.fmt"
static FFPixFmt _ffmpeg_yuv_split_width_height(const char* pFileName, OUT int* pWidth, OUT int* pHeight, OUT double* pFps, double defFps)
{
    FFPixFmt fmt0=FF_NONE, fmt1=FF_NONE;
    int len=(int)strlen(pFileName);
    int namePos=len-1, extPos=len-1;
    while(namePos>=0 && pFileName[namePos]!='\\' && pFileName[namePos]!='/')
        namePos--;
    namePos++;
    while(extPos>=namePos && pFileName[extPos]!='.')
        extPos--;
    if(pFileName[extPos]=='.' && (fmt0=ffmpeg_yuv_str2pixfmt(pFileName+extPos+1))==FF_YUV)
    {
        int j=extPos-1;
        while(j>0 && pFileName[j]!='.')
            j--;
        if(pFileName[j]=='.'&& (fmt1=ffmpeg_yuv_str2pixfmt(pFileName+j+1))>FF_YUV)
            fmt0=fmt1;
    }
    fmt0=(fmt0<=FF_YUV?ffmpeg_yuv_get_default_pixfmt():fmt0);
    int i, width=0, height=0;
    double fps=0;
    for(i=extPos-1; i>namePos; i--)
    {
        if((pFileName[i]=='x' || pFileName[i]=='X') && (IS_NUMBER(pFileName[i-1]) && IS_NUMBER(pFileName[i+1])))
        {
            int j=i-1, k, radix=1;
            for(j=i-1, width=0; j>=namePos && IS_NUMBER(pFileName[j]); j--, radix*=10)
                width=radix*(pFileName[j]-'0')+width;
            for(k=i+1, height=0; k<extPos && IS_NUMBER(pFileName[k]); k++)
                height=10*height+(pFileName[k]-'0');
            if(k<extPos && IS_NUMBER(pFileName[k+1]))
                fps=atof(pFileName+k+1);
            break;
        }
    }
    if(pWidth) *pWidth=width;
    if(pHeight) *pHeight=height;
    if(pFps) *pFps=(fps<=0?defFps:fps);
    return fmt0;
}
static forceinline FFPixFmt ffmpeg_yuv_split_width_height(const char* pFileName, OUT int* pWidth, OUT int* pHeight, OUT double* pFps)
{
    return _ffmpeg_yuv_split_width_height(pFileName, pWidth, pHeight, pFps, 25);
}
// ffmpeg_yuv_set_default_pixfmt() before ffmpeg_video_info() if pName is xxx_wxh_fps.yuv
static FFInfo* ffmpeg_get_video_info(const char* pName, OUT FFInfo* pInfo)
{
    static FFInfo info;
    if(pInfo==0) pInfo=&info;
    memset(pInfo, 0, sizeof(FFInfo));
    if(ffmpeg_is_videofile(pName))
    {
        char pPixfmt[32]={0}, pCodec[128]={0};
        int cmdLen=1024, textLen=128*1024, len=cmdLen+textLen;
        char *pText=(char*)malloc(len);
        memset(pText, 0, len);
        char* pCmd=pText+textLen;
        sprintf(pCmd, "%s -i \"%s\" 2>&1", FFMPEG_BIN, pName);
        FILE *fp=popen(pCmd, "r");
        int isFindVideo=0, isFindAudio=0, limit=1;
        double fps=-1, tbr=-1, tbn=-1, tbc=-1;
        if(fp)
        {
            const char* p=0;
            int isFindVideo=0;
            //Printf_DEBUG(TEXT_COLOR_YELLOW, "%s\n", pCmd);
            while(fgets(pText, textLen, fp))
            {
                //Printf_DEBUG(TEXT_COLOR_YELLOW, pText);
                if(isFindVideo==0 && strncmp(pText, "  Duration: ", strlen("  Duration: "))==0)
                {
                    //Printf_DEBUG(TEXT_COLOR_WHITE, pText);
                    int h, m, s, ms;
                    if(sscanf(pText, "  Duration: %d:%d:%d.%d", &h, &m, &s, &ms)==4)
                        pInfo->sec=h*3600+m*60+s+ms/100.0;
                    if((p=strstr(pText, "bitrate: ")))
                        pInfo->total_bitrate=(float)atof(p+strlen("bitrate: "));
                        //sscanf(p, "bitrate: %d kb/s", &pInfo->total_bitrate);
                }
                if(sscanf(pText, "  Stream #%*[^ ] Video: %127[^,], %30[0-9a-zA-Z]", pCodec, pPixfmt)==2)
                {
                    isFindVideo=1;
                    //Printf_DEBUG(TEXT_COLOR_MAGENTA, pText);
                    if(sscanf(pCodec, "%15[0-9a-zA-Z]", pInfo->pCodec)<1)
                        strcpy(pInfo->pCodec, "unknown");
                    pInfo->pixfmt=ffmpeg_string2pixfmt(pPixfmt);
                    p=strstr(pText, pPixfmt)+strlen(pPixfmt);
                    if(p[0]=='(')
                    {
                        char pRange[16]={0}, pColorSpace[16]={0}, pColorPrims[16]={0}, pColorTrans[16]={0};
                        int nRet=0;
                        if((nRet=sscanf(p+1, "%15[0-9a-zA-Z], %15[0-9a-zA-Z]/%15[0-9a-zA-Z]/%15[-0-9a-zA-Z]", pRange, pColorSpace, pColorPrims, pColorTrans))<4)
                        {
                            if(_ffmpeg_str2ColorSpace(pRange)>FF_COL_SPC_UNKNOWN && _ffmpeg_str2ColorSpace(pColorSpace)==FF_COL_SPC_UNKNOWN)
                                strcpy(pColorSpace, pRange);
                            if(nRet<=2) { strcpy(pColorPrims, pColorSpace); strcpy(pColorTrans, pColorSpace); }
                        }
                        pInfo->col_spc=_ffmpeg_str2ColorSpace(pColorSpace);
                        pInfo->col_pri=_ffmpeg_str2ColorPrims(pColorPrims);
                        pInfo->col_trc=_ffmpeg_str2ColorTrans(pColorTrans);
                        p=strstr(p, ")")+1;
                    }
                    if(sscanf(p+1, "%dx%d", &pInfo->width, &pInfo->height)<2)
                        pInfo->width=pInfo->height=0;
                    double fps=0, tbr=0, tbn=0;
                    if(NULL!=(p=strstr(pText, " fps"))) { while(p>pText && p[-1]!=' ') p--; fps=atof(p); }
                    if(NULL!=(p=strstr(pText, " tbr"))) { while(p>pText && p[-1]!=' ') p--; tbr=atof(p); }
                    if(NULL!=(p=strstr(pText, " tbn"))) { while(p>pText && p[-1]!=' ') p--; tbn=atof(p); }
                    if(fps>0 && tbr>0)
                        pInfo->fps=(0.5<=fps/tbr && fps/tbr<=2?tbr:fps);
                    else
                        pInfo->fps=(tbr>0?tbr:fps);
                    if(NULL!=(p=strstr(pText, " kb/s"))) { while(p>pText && p[-1]!=' ') p--; pInfo->video_bitrate=(float)atof(p); }
                    pInfo->frame_num=(int)(pInfo->fps*pInfo->sec+0.5);
                    break;
                }
            }
            fclose(fp);
        }
        free(pText);
    }
    else
    {
        int64 filesize=ffmpeg_yuv_get_filesize(pName);
        pInfo->pixfmt=ffmpeg_yuv_split_width_height(pName, &pInfo->width, &pInfo->height, &pInfo->fps);
        int framesize=ffmpeg_yuv_compute_frame_size(pInfo->width, pInfo->height, pInfo->pixfmt);
        pInfo->frame_num=(int)(filesize/MAX(1, framesize));
        pInfo->sec=pInfo->frame_num/pInfo->fps;
        pInfo->video_bitrate=pInfo->total_bitrate=(float)((double)filesize*8e-3/MAX(1e-4, pInfo->sec));
        strcpy(pInfo->pCodec, "rawvideo");
    }
    return pInfo;
}
static forceinline int ffmpeg_yuv_had_fps(const char* pName)
{
    if(!ffmpeg_is_videofile(pName))
    {
        double fps=0;
        _ffmpeg_yuv_split_width_height(pName, 0, 0, &fps, -1);
        return (fps>0);
    }
    return 1;
}
static FILE* ffmpeg_create_reader_full(const char* pName, FFPixFmt pixfmt, int width, int height, int idxFrame, int threads, const char* pFFmpeg, const char* pParam)
{
    FILE *fp=0;
    char pCmd[1024], pSrcInfo[128]="", pSS[32]="", pThread[32]="";
    int isVideo=ffmpeg_is_videofile(pName);
    pFFmpeg=(pFFmpeg==0?FFMPEG_BIN:pFFmpeg);
    pParam=(pParam==0?"":pParam);
    if(pixfmt<=FF_YUV || width<=0 || height<=0 || !isVideo || idxFrame>0)
    {
        FFInfo info;
        ffmpeg_get_video_info(pName, &info);
        if(pixfmt<=FF_YUV)
            pixfmt=info.pixfmt;
        if(width<=0 || height<=0)
            width=info.width, height=info.height;
        if(!isVideo)
            sprintf(pSrcInfo, "-s %dx%d -pix_fmt %s -f rawvideo -r %g", info.width, info.height, ffmpeg_pixfmt2string(info.pixfmt), info.fps);
        if(idxFrame>0)
            sprintf(pSS, "-ss %.6f", (idxFrame-0.5)/info.fps);
    }
    if(threads>0) sprintf(pThread, "-threads %d", threads);
    //if(threads<=0) threads=2;
    int n=sprintf(pCmd, "%s %s %s %s -i \"%s\" -loglevel error -f image2pipe -pix_fmt %s -vcodec rawvideo -s %dx%d -sws_flags bicubic %s - ", pFFmpeg, pSS, pSrcInfo, pThread, pName, ffmpeg_pixfmt2string(pixfmt), width, height, pParam);
    n+=sprintf(pCmd+n, " 2>" IO_NULL);
    // Printf_DEBUG(TEXT_COLOR_BLUE, "%s\n", pCmd);
    fp=popen(pCmd, IO_R);
    return fp;
}
static forceinline FILE* ffmpeg_create_reader_ex(const char* pName, FFPixFmt pixfmt, int width, int height, int threads, const char* pFFmpeg)
{
    return ffmpeg_create_reader_full(pName, pixfmt, width, height, 0, threads, pFFmpeg, NULL);
}
static forceinline FILE* ffmpeg_create_reader(const char* pName, FFPixFmt pixfmt)
{
    return ffmpeg_create_reader_ex(pName, pixfmt, 0, 0, 0, NULL);
}
static int ffmpeg_get_frame(FILE* fp, void* pData, int dataSize)
{
    return (int)fread(pData, 1, dataSize, fp);
}
static int ffmpeg_get_frame2(FILE* fp, void* pY, int ySize, void* pU, int uSize, void* pV, int vSize)
{
    int y=(int)fread(pY, 1, ySize, fp), u=(int)fread(pU, 1, uSize, fp), v=(int)fread(pV, 1, vSize, fp);
    return y+u+v;
}
static int ffmpeg_get_frame3(FILE* fp, uint8* ppData[4], FFPixFmt pixfmt, int width, int height)
{
    int scale=ffmpeg_is_10bit(pixfmt)+1, yshift=ffmpeg_yuv_half_height(pixfmt), xshift=ffmpeg_yuv_half_width(pixfmt), nSize=0;
    int pSize[4]={width*height, (height>>yshift)*(width>>xshift), (height>>yshift)*(width>>xshift), (height>>yshift)*(width>>xshift)}, cn=ffmpeg_yuv_channel(pixfmt);
    if(ffmpeg_yuv_isRGB(pixfmt))
        pSize[0]=width*height*cn, cn=1;
    for(int i=0; i<cn; i++)
        nSize+=(int)fread(ppData[i], scale, pSize[i], fp);
    return nSize;
}
static FILE* ffmpeg_create_writer_ex(const char* pName, FFPixFmt pixfmt, int width, int height, double fps, double crf, const char* pCodec, const char* pFFmpegParam, const char* pFFmpeg)
{
    FILE *fp=0;
    pFFmpeg=(pFFmpeg==0?FFMPEG_BIN:pFFmpeg);
    if(ffmpeg_is_videofile(pName))
    {
        char pCmd[1024];
        int n=sprintf(pCmd, "%s -y -loglevel error -f rawvideo -vcodec rawvideo -s %dx%d -pix_fmt %s -r %g -i - -an ", pFFmpeg, width, height, ffmpeg_pixfmt2string(pixfmt), fps);
        if(pCodec) n+=sprintf(pCmd+n, "-vcodec %s ", pCodec);
        if(crf>=0) n+=sprintf(pCmd+n, "-crf %g ", crf);
        if(pFFmpegParam)
            n+=sprintf(pCmd+n, "%s ", pFFmpegParam);
        else if(pCodec && strcmp(pCodec, LIBX265)==0)
            n+=sprintf(pCmd+n, "-tag:v hvc1 ");
        n+=sprintf(pCmd+n, "-threads 4 ");
        n+=sprintf(pCmd+n, "\"%s\" 2>" IO_NULL " 1>" IO_NULL, pName);
        fp=popen(pCmd, IO_W);
    }
    else
    {
        fp=fopen(pName, "wb");
    }
    return fp;
}
static forceinline FILE* ffmpeg_create_writer(const char* pName, FFPixFmt pixfmt, int width, int height, double fps, double crf, const char* pCodec, const char* pFFmpegParam)
{
    return ffmpeg_create_writer_ex(pName, pixfmt, width, height, fps, crf, pCodec, pFFmpegParam, 0);
}
static int ffmpeg_set_frame(FILE* fp, void* pData, int dataSize)
{
    return (int)fwrite(pData, 1, dataSize, fp);
}
static int ffmpeg_set_frame2(FILE* fp, void* pY, int ySize, void* pU, int uSize, void* pV, int vSize)
{
    int y=(int)fwrite(pY, 1, ySize, fp), u=(int)fwrite(pU, 1, uSize, fp), v=(int)fwrite(pV, 1, vSize, fp);
    return y+u+v;
}
static int ffmpeg_set_frame3(FILE* fp, uint8* ppData[4], FFPixFmt pixfmt, int width, int height)
{
    int scale=ffmpeg_is_10bit(pixfmt)+1, yshift=ffmpeg_yuv_half_height(pixfmt), xshift=ffmpeg_yuv_half_width(pixfmt), nSize=0;
    int pSize[4]={width*height, (height>>yshift)*(width>>xshift), (height>>yshift)*(width>>xshift), (height>>yshift)*(width>>xshift)}, cn=ffmpeg_yuv_channel(pixfmt);
    if(ffmpeg_yuv_isRGB(pixfmt))
        pSize[0]=width*height*cn, cn=1;
    for(int i=0; i<cn; i++)
        nSize+=(int)fwrite(ppData[i], scale, pSize[i], fp);
    return nSize;
}
static void ffmpeg_close(FILE* fp)
{
    fclose(fp);
}
static int ffmpeg_get_frame_num(const char* pName)
{
    int frameNum=0;
    FFInfo info;
    ffmpeg_get_video_info(pName, &info);
    if(info.frame_num>0)
        return info.frame_num;
    if(info.width*info.height>0 && info.pixfmt>FF_YUV)
    {
        int frameSize=ffmpeg_yuv_compute_frame_size(info.width, info.height, info.pixfmt);
        uint8 *pData=(uint8*)malloc(frameSize);
        FILE* fp=ffmpeg_create_reader(pName, info.pixfmt);
        while(ffmpeg_get_frame(fp, pData, frameSize)==frameSize)
            frameNum++;
        free(pData);
        ffmpeg_close(fp);
    }
    return frameNum;
}

#endif // __FFMPEG_H__
