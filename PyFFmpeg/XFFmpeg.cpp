#include "pch.h"
#include "XFFmpeg.h"
#include "stdio.h"
extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavdevice/avdevice.h>
	#include <libswresample/swresample.h>
	#include <libavutil/imgutils.h>
	#include <libswscale/swscale.h>
}


bool XFFmpeg::Close()
{
	if (ic)
	{
		avformat_close_input(&ic);//�ڲ���null
	}
	if (vcodec)
	{
		avcodec_free_context(&vcodec);
	}
	if (pkt)
	{
		av_packet_free(&pkt);
	}
	if (frame)
	{
		av_frame_free(&frame);
	}
	if (rgb)
	{
		delete rgb;
		rgb = NULL;
	}
	return true;
}

//��ʼ��׼������  ���װic ��Ƶ֡pkt ������vcodec
int XFFmpeg::Open(const char* url) {

	printf("PyFFmpeg::Open %s \n", url);
	//����Ƶ  ���װ  ��ѹ���İ�ȡ����  &icָ��ĵ�ַ��**����
	int re = avformat_open_input(&ic, url, 0, 0);  //ic������
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf));//������Ϣ
		printf("avformat_open_input failed : %s", buf);
		return false;
	}
	
	//��ȡ����Ϣ
	
	//��һ����ȡ��Ƶ��ʱ����������Ϣ��ת�ɺ���
	this->Totalms = ic->duration / (AV_TIME_BASE / 1000);
	printf("In XFFmpeg Total Ms = %d\n", Totalms);  //Total Ms = 5069

	//fps �� ����
	if(ic->streams[0]->avg_frame_rate.num > 0)
		this->fps = ic->streams[0]->avg_frame_rate.num / ic->streams[0]->avg_frame_rate.den;

	///////////////////////////////////////////////////////
	/////�򿪽�����  0��Ƶ  1��Ƶ  codecpar �����id��
	AVCodecParameters* para = ic->streams[0]->codecpar;

	//�ҵ�������codec  �ҵ�����ָ�����Ƶ���ע���������
	AVCodec* codec = avcodec_find_decoder(para->codec_id);
	if (!codec)
	{
		printf("avcodec_find_decoder not find %d\n", para->codec_id);
	}
	//���������������  ����ռ�
	vcodec = avcodec_alloc_context3(codec);
	//���ý�����������  ��ʼ��
	avcodec_parameters_to_context(vcodec, para);

	//��������  ���߳̽���
	vcodec->thread_count = 8;

	//�������� ��ʼ����
	re = avcodec_open2(vcodec, 0, 0);
	if (re != 0)
	{
		//����ʧ��  ����������ռ�  ��ӡ������Ϣ
		avcodec_free_context(&vcodec);
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		printf("avcodec_open2 failed :%s! \n", buf);
		return false;
	}
	printf("avcodec_open2 succeed�� \n");


	rgb = new unsigned char[4096 * 2160 * 4];

	return true;
}


//��ȡһ֡��Ƶ���ڲ��洢
bool XFFmpeg::Read()
{
	if (!ic) return false;
	//��Ƶ֡�Ĵ洢�ռ�
	if (!pkt)
		pkt = av_packet_alloc();//��һ�� �������ռ�
	else
		av_packet_unref(pkt);//�ڶ��� ���ü���-1  =0 ������Ƶ֡
	int re = 0;
	bool isFindVideo = false;
	//������Ƶ����
	for (int i=0; i<20; i++)
	{
		//��ȡһ֡����
		re = av_read_frame(ic, pkt);
		//��ȡʧ��/�����ļ���β
		if (re != 0)
		{
			return false;
		}
		//�Ƿ�����Ƶ֡
		if (pkt->stream_index == 0)
		{

			isFindVideo = true;
			break;
		}
		//��Ƶ֡������packet
		av_packet_unref(pkt);
	}

	return true;
}

//���뺯��
bool XFFmpeg::Decode(int outwidth, int outheight)
{

	if (!vcodec || !pkt) return false;

	//���͵������߳�
	int re = avcodec_send_packet(vcodec, pkt);
	if (re!=0) return false;

	
	if (!frame)
		frame = av_frame_alloc();
	re = avcodec_receive_frame(vcodec, frame);
	if (re != 0) 
	{
		//av_frame_free(&frame); 
		return false;
	}

	//��yuvת��rgb  �������ߴ�
	sws = sws_getCachedContext(sws,
							vcodec->width, vcodec->height, vcodec->pix_fmt, //Դ������� YUV420
							outwidth,outheight,AV_PIX_FMT_BGRA,//Ŀ�������� BGRA
							SWS_BICUBIC, //ת���㷨
							0,0,0
							);
	if (!sws)
	{
		printf("sws_getCachedContext failed!");
		return false;
	}
	if (!rgb)
	{
		printf("rgb is null !");
		return false;
	}
	//�ߴ�ת���ռ�������ͷ�
	uint8_t* data[AV_NUM_DATA_POINTERS] = { 0 };
	data[0] = rgb;
	int linesize[AV_NUM_DATA_POINTERS] = { 0 };
	linesize[0] = outwidth * 4;
	int h = sws_scale(sws, frame->data, frame->linesize, 0, frame->height, data, linesize);

	printf("[%d]",h);

	return true;


	

}


XFFmpeg::XFFmpeg() {
	printf("Create XFFmpeg\n");
}
XFFmpeg::~XFFmpeg() {
	printf("Delete XFFmpeg\n");
}