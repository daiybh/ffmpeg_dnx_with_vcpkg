

ffmpeg -f rawvideo -pix_fmt uyvy422  -s:v 1920x1080 -r 30 -i d:\\11.yuv -c:v dnxhd -b:v 120M -f dnxhd ffmpeg_120m.dnx -y
