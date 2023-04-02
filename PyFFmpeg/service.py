# coding=gbk
# ��web������
from wsgiref.simple_server import make_server
from pyffmpeg import *
from urllib.parse import parse_qs
def www(env, respone):
    respone('200 OK',[('Content-type','text/html')])
    
    # �����ļ�·��
    # QUERY_STRING = http://127.0.0.1:8001/?filepath=F:\WIN10SDK\py_lesson\src\PyFFmpeg\PyFFmpeg\video.mp4
    parse = parse_qs(env['QUERY_STRING'])
    filepath = parse['filepath'][0]
    print("filepath = ", filepath)
    html = '''<h1>filepath is %s</h1>''' %filepath

    ff = PyFFmpeg()
    ff.open(filepath)
    totalms = ff.GetTotalms
    html += '''<h2>totalms = %d</h2>''' %totalms


    # html = "OK"
    return [html.encode("UTF-8")]

# ����һ��web������ IP��ַΪ�� �˿� ���պ���
port = 8001
httpd = make_server("",port,www)
httpd.serve_forever()