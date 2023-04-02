#pragma once
struct AVFormatContext;
struct AVPacket;
struct AVCodec;
struct AVCodecContext;
struct AVFrame;
struct AVFrame;
struct SwsContext;
class XFFmpeg
{
public:

	//打开视频源
	int Open(const char *url);
	//读取一帧视频在内部存储
	bool Read();

	//传出解码出的rgb数据  通过成员
	bool Decode(int outweight, int outheight);

	bool Close();
	int Totalms = 0;
	int fps = 0;
	XFFmpeg();
	~XFFmpeg();

	unsigned char* rgb = 0;

protected:
	//解封装上下文
	AVFormatContext* ic = 0;
	
	//读取的视频帧
	AVPacket* pkt = 0;

	//解码器上下文 
	AVCodec* codec = 0;
	//解码器成员  其他线程也可以用
	AVCodecContext* vcodec = 0;
	

	//解码后的yuv帧
	AVFrame* frame = 0;

	//yuv转成rgb
	SwsContext* sws = 0;

};


