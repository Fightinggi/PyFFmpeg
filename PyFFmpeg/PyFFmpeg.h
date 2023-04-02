#include <Python.h>
#include "XFFmpeg.h"
class PyFFmpeg
{
public:
	PyObject_HEAD
	XFFmpeg* ff;

//开放给python调用的
public:
	static PyObject* Create(PyTypeObject* type, PyObject* args, PyObject* kw);
	static int Init(PyFFmpeg* self, PyObject* args, PyObject* kw);
	static void Close(PyFFmpeg* self);
	static PyObject* Open(PyFFmpeg* self, PyObject* args);
	static PyObject* Read(PyFFmpeg* self, PyObject* args);
	static PyObject* Decode(PyFFmpeg* self, PyObject* args);
	//属性函数get
	static PyObject* GetTotalms(PyFFmpeg* self, void *closure);
	static PyObject* GetFps(PyFFmpeg* self, void *closure);

};