###ffmpge 常用命令
- 生成静音文件：ffmpeg -f lavfi -i anullsrc=sample_rate=${sample_rate} -y -t ${time.toFixed(3)} ${output}
- 音视频裁剪： ffmpeg -i ${audio_path}  -vn -acodec copy -ss ${start} -t ${dur} ${output}
- 多音频拼接： ffmpeg -i test1.mp3 -i silent.mp3 -i silent.mp3 -filter_complex '[0:0] [1:0] concat=n=3:v=0:a=1 [a]' -map [a] 3new.mp3
- MP3转pcm： ffmpeg -i test1.mp3 -f s16be -ar 16000 -ac 1 -acodec pcm_s16be pcm16k.pcm
- 使用list文件拼接：ffmpeg -f concat -i list.txt -c copy ./split/all.mp3
