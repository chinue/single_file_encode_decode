#  single c/c++ header file for video encode/decode using FFmpeg
First make sure FFmpeg command could work in your command line. If you don't set up a FFmpeg yet, you could down load an executable file from [here](https://ffmpeg.org/download.html), and set the path of ffmpeg to you environment variable PATH. After that, open the command line and input `ffmpeg` to ensure that the system can recognize the command correctly.

### compile

```bash
gcc main.c -fPIC -O3 -lm -DNDEBUG -o test
```

### test

```bash
./test video/cactus.mp4 out.mp4 -crf 29
```

