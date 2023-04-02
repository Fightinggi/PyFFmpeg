#include <Python.h>
#include "XFFmpeg.h"
class PyFFmpeg
{
public:
	PyObject_HEAD
	XFFmpeg* ff;

//���Ÿ�python���õ�
public:
	static PyObject* Create(PyTypeObject* type, PyObject* args, PyObject* kw);
	static int Init(PyFFmpeg* self, PyObject* args, PyObject* kw);
	static void Close(PyFFmpeg* self);
	static PyObject* Open(PyFFmpeg* self, PyObject* args);
	static PyObject* Read(PyFFmpeg* self, PyObject* args);
	static PyObject* Decode(PyFFmpeg* self, PyObject* args);
	//���Ժ���get
	static PyObject* GetTotalms(PyFFmpeg* self, void *closure);
	static PyObject* GetFps(PyFFmpeg* self, void *closure);

};