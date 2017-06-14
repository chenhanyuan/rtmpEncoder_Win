#include "StdAfx.h"
#include "CEncoder.h"
#include "Common.h"

CEncoder::CEncoder()
{
	avCodecContext = NULL;  
	in_c = NULL;
	scxt = NULL;

	m_pRGBFrame =  new AVFrame[1];  //RGB帧数据    
	m_pYUVFrame = new AVFrame[1];;  //YUV帧数据  
}

CEncoder::~CEncoder()
{
	delete []m_pRGBFrame; 
	delete []m_pYUVFrame;
	if (avCodecContext != NULL)
	{
		avcodec_close(avCodecContext);  
		av_free(avCodecContext); 
	}
}

void CEncoder::avInit()
{
	av_register_all();  
	avcodec_register_all();
}

void CEncoder::AV_InitCodec(AVCodecID CodeId, int nWidth, int nHeight)
{
	//查找h264编码器  
	pCodecH264 = avcodec_find_encoder(CodeId);  
	if(!pCodecH264)  
	{  
		fprintf(stderr, "h264 codec not found\n");  
		getchar();  
		exit(1);  
	}  

	avCodecContext= avcodec_alloc_context3(pCodecH264);  
	avCodecContext->bit_rate = 1500000;// put sample parameters   
	avCodecContext->width =nWidth;//   
	avCodecContext->height = nHeight;//   

	// frames per second   
	avCodecContext->time_base.num = 1;//(AVRational){1,25};  
	avCodecContext->time_base.den = 12;
	avCodecContext->gop_size = 10; // emit one intra frame every ten frames   //越小，延迟越小
	avCodecContext->max_b_frames = 1;  
	avCodecContext->thread_count = 1;  
	avCodecContext->pix_fmt = PIX_FMT_YUV420P;//PIX_FMT_RGB24;  

	//av_opt_set(c->priv_data, "libvpx-1080p.ffpreset", NULL, 0);  
	//打开编码器  
	if(avcodec_open2(avCodecContext,pCodecH264,NULL)<0)
	{  
		printf("avcodec_open2 failed\n");  
		//TRACE("不能打开编码库");  
		getchar();  
	} 
}
FILE *yuv_file;
int CEncoder::Sws_ScaleHandle(AVPacket *avpkt, uint8_t *pBits, enum AVPixelFormat srcFormat, uint8_t *yuv_buff, enum AVPixelFormat dstFormat)
{
	//将RGB转化为YUV  

	if (scxt == NULL)
	{
		scxt = sws_getContext(avCodecContext->width, avCodecContext->height, srcFormat, avCodecContext->width, avCodecContext->height, dstFormat, SWS_BICUBIC, NULL, NULL, NULL);
		m_pYUVFrame->pts = 0;
		yuv_file = fopen("stream.yuv", "wb");
	}

	avpicture_fill((AVPicture*)m_pRGBFrame, pBits, srcFormat, avCodecContext->width, avCodecContext->height);  

	//将YUV buffer 填充YUV Frame  
	avpicture_fill((AVPicture*)m_pYUVFrame, yuv_buff, dstFormat, avCodecContext->width, avCodecContext->height);  

	sws_scale(scxt, m_pRGBFrame->data, m_pRGBFrame->linesize, 0, avCodecContext->height, m_pYUVFrame->data, m_pYUVFrame->linesize); //色彩空间转换
	int got_packet_ptr = 0; 
	//av_init_packet(&avpkt);  
	//avpkt.data = outbuf;  
	//avpkt.size = outbuf_size; 

	/*{
	//保存为YUV420p
		unsigned char* buf = new unsigned char[avCodecContext->height * avCodecContext->width * 3 / 2];
		memset(buf, 0, avCodecContext->height * avCodecContext->width * 3 / 2);
		int height = avCodecContext->height;
		int width = avCodecContext->width;
		printf("decode video ok\n");
		int a = 0, i;
		for (i = 0; i<height; i++)
		{
			memcpy(buf + a, m_pYUVFrame->data[0] + i * m_pYUVFrame->linesize[0], width);
			a += width;
		}
		for (i = 0; i<height / 2; i++)
		{
			memcpy(buf + a, m_pYUVFrame->data[1] + i * m_pYUVFrame->linesize[1], width / 2);
			a += width / 2;
		}
		for (i = 0; i<height / 2; i++)
		{
			memcpy(buf + a, m_pYUVFrame->data[2] + i * m_pYUVFrame->linesize[2], width / 2);
			a += width / 2;
		}
		fwrite(buf, 1, avCodecContext->height * avCodecContext->width * 3 / 2, yuv_file);
		delete buf;
		buf = NULL;
	}*/
	
	int rec = avcodec_encode_video2(avCodecContext, avpkt, m_pYUVFrame, &got_packet_ptr);  
	if (rec==0&&got_packet_ptr==1)
	{
		m_pYUVFrame->pts++;
		DebugPrint("packet pts : %d", m_pYUVFrame->pts);
	}
	else
		DebugPrint("Encoder error");

	
	return got_packet_ptr;
}