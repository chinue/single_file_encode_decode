#include "ffmpeg.h"
int main(int argc, const char* argv[])
{
    if(argc<3)
    {
        printf("usage: test input.mp4 output.mp4 [-crf xx]\n");
        return -1;
    }
    const char *pSrcName=argv[1], *pDstName=argv[2];
    double crf=(argc>=5 && strcmp(argv[3], "-crf")==0)?atof(argv[4]):FF_CRF_AUTO;
    FFInfo *p=ffmpeg_get_video_info(pSrcName, 0);
    int width=p->width, height=p->height, frameNum=p->frame_num;
    int frameSize=ffmpeg_yuv_compute_frame_size(width, height, p->pixfmt);
    printf("width=%d  height=%d  pix_fmt=%s  frameNum=%d\n", p->width, p->height, ffmpeg_pixfmt2string(p->pixfmt), frameNum);
    printf("col_spc=%s  col_pri=%s col_trc=%s\n", ffmpeg_GetColorSpaceName(p->col_spc), ffmpeg_GetColorPrimsName(p->col_pri), ffmpeg_GetColorTransName(p->col_trc));
    uint8* pX=(uint8*)malloc(frameSize);
    const char* pParam=(p->col_trc==FF_COL_TRC_HLG?MAKE_X265_PARAM(HDR_PARAM_HLG):(p->col_trc==FF_COL_TRC_PQ?MAKE_X265_PARAM(HDR_PARAM_PQ):MAKE_X265_PARAM(SDR_PARAM_BT709)));
    FILE *pReader=ffmpeg_create_reader(pSrcName, p->pixfmt);
    FILE *pWriter=ffmpeg_create_writer(pDstName, p->pixfmt, width, height, p->fps, crf, LIBX265, pParam);
    for(int i=0; i<frameNum; i++)
    {
        printf("\r%4d/%d ", i+1, frameNum);
        ffmpeg_get_frame(pReader, pX, frameSize);
        ffmpeg_set_frame(pWriter, pX, frameSize);
    }
    printf("\n");
    ffmpeg_close(pReader); ffmpeg_close(pWriter);
    free(pX);
    return 1;
}