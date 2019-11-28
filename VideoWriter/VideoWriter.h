#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>



#ifdef __cplusplus  
extern "C" {
#endif  
  
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
  
#ifdef __cplusplus  
}  
#endif  



class _VWirter {
public: 
	_VWirter(std::string filename, int fps, int width, int height);
	void write(const unsigned char* data, int stride);
	void close();
	~_VWirter();
private:
	std::string m_filename;
	AVCodec *m_codec;
	AVCodecContext *m_ctx;
	FILE *m_fp;
	AVFrame *m_frame;
	AVPacket *m_pkt;
	SwsContext* m_swsctx;
	
	int m_fps;
	int m_width;
	int m_height;
	bool m_isClosed;
};