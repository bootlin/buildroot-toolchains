This patch enables linphone to use uclinux-dist/lib/libbfgdots/g729 library.


1. Apply g729 patch:
   cd linphone-xxx/
   patch -p1 < ../g729_patch/linphone-g729.patch
2. Configure uclinux to build libbfgdots and linphone
3. Build kernel and load 
3. Configure linphone to use g729 codec, e.g, in .linphonerc: 
   [audio_codec_4]
   mime=G729
   rate=8000
   enabled=1
