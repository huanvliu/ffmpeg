#include "VideoWriter.h"

static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile) {
    int ret;
    ret = avcodec_send_frame(enc_ctx, frame);
    if(ret < 0) {
        printf("Error sending a frame for encoding\n");
        exit(0);
    }
    while(ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        }
        else if(ret < 0) {
            printf("Error during encoding\n");
            exit(0);
        }
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}




_VWirter::_VWirter(std::string filename, int fps, int width, int height): 
m_filename(filename), m_fps(fps), m_width(width), m_height(height) {
	m_isClosed = false;
	m_codec = avcodec_find_encoder_by_name("h264_nvenc");
	if(!m_codec) {
		printf("Codec 'h264_nvenc' not found\n");
		exit(0);
	}
	
	m_ctx = avcodec_alloc_context3(m_codec);
	if(!m_ctx) {
		printf("Could not allocate video codec context\n");
		exit(0);
	}
	
	m_pkt = av_packet_alloc();
	if(!m_pkt) {
		printf("Could not allocate packet\n");
		exit(0);
	}
	
	m_ctx->bit_rate = 400000;
	m_ctx->width = m_width;
	m_ctx->height = m_height;
    m_ctx->time_base = (AVRational){1, m_fps};
    m_ctx->framerate = (AVRational){m_fps, 1};
    m_ctx->gop_size = 10;
    m_ctx->max_b_frames = 1;
    m_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    if(m_codec->id == AV_CODEC_ID_H264) {
        av_opt_set(m_ctx->priv_data, "preset", "slow", 0);
    }
    
    int ret = avcodec_open2(m_ctx, m_codec, NULL);
    if(ret < 0) {
        printf("Could not open codec\n");
        exit(0);
    }
    
    m_fp = fopen(filename.c_str(), "wb");
    if (!m_fp) {
        printf("Could not open %s\n", filename.c_str());
        exit(0);
    }
    
    m_frame = av_frame_alloc();
    if (!m_frame) {
        printf("Could not allocate video frame\n");
        exit(0);
    }
    
    m_frame->format = m_ctx->pix_fmt;
    m_frame->width  = m_ctx->width;
    m_frame->height = m_ctx->height;
    
    ret = av_frame_get_buffer(m_frame, 32);
    if(ret < 0) {
        printf("Could not allocate the video frame data\n");
        exit(0);
    }
    
	m_swsctx = sws_getContext(m_width, m_height, AV_PIX_FMT_BGR24,
        m_width, m_height, m_ctx->pix_fmt, 
        0, NULL, NULL, NULL);
    if(!m_swsctx){
        printf("Could not create sws context\n");
        exit(0);
    }
}


void _VWirter::write(const unsigned char* data, int stride) {
	int ret = av_frame_make_writable(m_frame);
	if(ret < 0) {
        printf("Could not make frame writable\n");
        exit(0);
	}
	sws_scale(m_swsctx, &data, &stride, 0, m_height, m_frame->data, m_frame->linesize);
	m_frame->pts++;
	encode(m_ctx, m_frame, m_pkt, m_fp);
}

void _VWirter::close() {
	if(!m_isClosed) {
		uint8_t endcode[] = { 0, 0, 1, 0xb7 };
		encode(m_ctx, NULL, m_pkt, m_fp);
        if(m_codec->id == AV_CODEC_ID_MPEG1VIDEO || m_codec->id == AV_CODEC_ID_MPEG2VIDEO) {
           fwrite(endcode, 1, sizeof(endcode), m_fp); 
        }
        fclose(m_fp);
        avcodec_free_context(&m_ctx);
        av_frame_free(&m_frame);
        av_packet_free(&m_pkt);
        sws_freeContext(m_swsctx);
        m_isClosed = true;
	}
}

_VWirter::~_VWirter() {
	if(!m_isClosed) {
		uint8_t endcode[] = { 0, 0, 1, 0xb7 };
		encode(m_ctx, NULL, m_pkt, m_fp);
        if(m_codec->id == AV_CODEC_ID_MPEG1VIDEO || m_codec->id == AV_CODEC_ID_MPEG2VIDEO) {
           fwrite(endcode, 1, sizeof(endcode), m_fp); 
        }
        fclose(m_fp);
        avcodec_free_context(&m_ctx);
        av_frame_free(&m_frame);
        av_packet_free(&m_pkt);
        sws_freeContext(m_swsctx);
        m_isClosed = true;
	}
}




