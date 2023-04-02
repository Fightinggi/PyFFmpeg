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
		avformat_close_input(&ic);//内部置null
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

//初始化准备工作  解封装ic 视频帧pkt 解码器vcodec
int XFFmpeg::Open(const char* url) {

	printf("PyFFmpeg::Open %s \n", url);
	//打开视频  解封装  把压缩的包取出来  &ic指针的地址用**来接
	int re = avformat_open_input(&ic, url, 0, 0);  //ic上下文
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf));//报错信息
		printf("avformat_open_input failed : %s", buf);
		return false;
	}
	
	//获取流信息
	
	//进一步获取视频总时长等其他信息，转成毫秒
	this->Totalms = ic->duration / (AV_TIME_BASE / 1000);
	printf("In XFFmpeg Total Ms = %d\n", Totalms);  //Total Ms = 5069

	//fps 是 分数
	if(ic->streams[0]->avg_frame_rate.num > 0)
		this->fps = ic->streams[0]->avg_frame_rate.num / ic->streams[0]->avg_frame_rate.den;

	///////////////////////////////////////////////////////
	/////打开解码器  0视频  1音频  codecpar 编码的id号
	AVCodecParameters* para = ic->streams[0]->codecpar;

	//找到解码器codec  找到具有指定名称的已注册解码器。
	AVCodec* codec = avcodec_find_decoder(para->codec_id);
	if (!codec)
	{
		printf("avcodec_find_decoder not find %d\n", para->codec_id);
	}
	//分配解码器上下文  分配空间
	vcodec = avcodec_alloc_context3(codec);
	//配置解码器上下文  初始化
	avcodec_parameters_to_context(vcodec, para);

	//其他配置  多线程解码
	vcodec->thread_count = 8;

	//打开上下文 开始解码
	re = avcodec_open2(vcodec, 0, 0);
	if (re != 0)
	{
		//解码失败  清理解码器空间  打印错误信息
		avcodec_free_context(&vcodec);
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		printf("avcodec_open2 failed :%s! \n", buf);
		return false;
	}
	printf("avcodec_open2 succeed！ \n");


	rgb = new unsigned char[4096 * 2160 * 4];

	return true;
}


//读取一帧视频在内部存储
bool XFFmpeg::Read()
{
	if (!ic) return false;
	//视频帧的存储空间
	if (!pkt)
		pkt = av_packet_alloc();//第一次 分配对象空间
	else
		av_packet_unref(pkt);//第二次 引用计数-1  =0 清理视频帧
	int re = 0;
	bool isFindVideo = false;
	//丢掉音频数据
	for (int i=0; i<20; i++)
	{
		//读取一帧数据
		re = av_read_frame(ic, pkt);
		//读取失败/读到文件结尾
		if (re != 0)
		{
			return false;
		}
		//是否是视频帧
		if (pkt->stream_index == 0)
		{

			isFindVideo = true;
			break;
		}
		//音频帧，清理packet
		av_packet_unref(pkt);
	}

	return true;
}

//解码函数
bool XFFmpeg::Decode(int outwidth, int outheight)
{

	if (!vcodec || !pkt) return false;

	//发送到解码线程
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

	//把yuv转成rgb  并调整尺寸
	sws = sws_getCachedContext(sws,
							vcodec->width, vcodec->height, vcodec->pix_fmt, //源宽高像素 YUV420
							outwidth,outheight,AV_PIX_FMT_BGRA,//目标宽高像素 BGRA
							SWS_BICUBIC, //转换算法
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
	//尺寸转换空间申请和释放
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