# coding=gbk

from pyffmpeg import *
ff = PyFFmpeg()# ��������
# print(help(PyFFmpeg))
try:

    ff.open("video.mp4");

    print("In python Totalms = ", ff.GetTotalms)


except Exception as e:
    print("pyffmpeg Exception",type(e),e)


input()