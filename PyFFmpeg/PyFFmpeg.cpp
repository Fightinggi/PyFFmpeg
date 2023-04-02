#include "pch.h"
#include "PyFFmpeg.h"
#include "XFFmpeg.h"


//���Ÿ�python���õ�
PyObject* PyFFmpeg::Create(PyTypeObject* type, PyObject* args, PyObject* kw)
{
	printf("PyFFmpeg::Create new\n");
	//�ȸ�PyFFmpeg����ռ� �ٸ����ĳ�ԱXFFmpeg* ff;����ռ�
	PyFFmpeg* Pyf = (PyFFmpeg*)type->tp_alloc(type, 0);
	Pyf->ff = new XFFmpeg();

	return (PyObject*)Pyf;
}
int PyFFmpeg::Init(PyFFmpeg* self, PyObject* args, PyObject* kw)
{
	printf("PyFFmpeg::Init\n");
	return 0;
} 
void PyFFmpeg::Close(PyFFmpeg* self)
{
	printf("PyFFmpeg::Close\n");
	//�������ԱXFFmpeg* ff�Ŀռ䣬������self
	self->ff->Close();
	delete self->ff;
	Py_TYPE(self)->tp_free(self);
	
}

PyObject* PyFFmpeg::Decode(PyFFmpeg* self, PyObject* args)
{
	if (!self->ff) Py_RETURN_FALSE;

	int w = 0;
	int h = 0;

	if (!PyArg_ParseTuple(args, "ii", &w, &h))
	{
		Py_RETURN_FALSE;
	}

	if (self->ff->Decode(w,h))
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

PyObject* PyFFmpeg::Read(PyFFmpeg* self, PyObject* args)
{
	if(!self->ff) Py_RETURN_FALSE;

	if (self->ff->Read())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

PyObject* PyFFmpeg::Open(PyFFmpeg* self, PyObject* args)
{

	const char* url = NULL;
	if (!PyArg_ParseTuple(args, "s", &url))
	{
		return NULL;
	}
	printf("PyFFmpeg::Open %s \n",url);

	if (self->ff->Open(url))
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;

	Py_RETURN_TRUE;
}

//���Ժ���get
PyObject* PyFFmpeg::GetTotalms(PyFFmpeg* self, void* closure)
{
	return PyLong_FromLong(self->ff->Totalms);
}

PyObject* PyFFmpeg::GetFps(PyFFmpeg* self, void* closure)
{
	return PyLong_FromLong(self->ff->fps);
}

//PyInit_ģ����
//1. ��չ����ں���  PyMODINIT_FUNC�� extern c __declspec(dllexport)
PyMODINIT_FUNC PyInit_pyffmpeg(void)  // define PyMODINIT_FUNC __declspec(dllexport) PyObject*
{
	//========����ģ��=======
	PyObject* mod = NULL;
	static PyModuleDef ffmod = {
	PyModuleDef_HEAD_INIT,
	"pyffmpeg",                           //"ģ����"��
	"pyffmpeg is first module test",      //"ģ��˵��",    help(ģ����)
	-1,                               //ģ��ռ䣬
	0                    //ģ�麯���б� 
	};
	printf("PyInit_pyffmpeg\n");
	mod = PyModule_Create(&ffmod);

	//===����������ͻ�������====
	static PyTypeObject type;
	memset(&type, 0, sizeof(type));
	type.ob_base = {PyObject_HEAD_INIT(NULL) 0};
	type.tp_name = "pyffmpeg.PyFFmpeg";
	type.tp_basicsize = sizeof(PyFFmpeg);
	type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
	type.tp_itemsize = 0;
	//===������Ķ���������Ĺ�����������======
	type.tp_new = PyFFmpeg::Create;//���ھ�̬�����������ʼ���������ڴ���������ռ�ö����ڴ�ռ�
	type.tp_init = (initproc)PyFFmpeg::Init;
	type.tp_dealloc = (destructor)PyFFmpeg::Close;
	//===������ĳ�Ա����============
	static PyMethodDef ffmeth[] = {
		{"open",(PyCFunction)PyFFmpeg::Open,METH_VARARGS,"PyFFmpeg::Open"},
		{"read",(PyCFunction)PyFFmpeg::Read,METH_NOARGS,"PyFFmpeg::Read"},
		{"decode",(PyCFunction)PyFFmpeg::Decode,METH_VARARGS,"PyFFmpeg::Decode"},
		{NULL}
	};
	type.tp_methods = ffmeth;
	//===�������Ժ���set��get=========
	static PyGetSetDef getsets[] = {
		{"GetTotalms",(getter)PyFFmpeg::GetTotalms,0,0,0 },
		{"fps",(getter)PyFFmpeg::GetFps,0,0,0 },
		{NULL }
	};
	type.tp_getset = getsets;
	//��ʼ����
	if (PyType_Ready(&type) < 0)
		return NULL;
	//���ü���+1
	Py_INCREF(&type);
	
	PyModule_AddObject(mod, "PyFFmpeg", (PyObject*) &type);	
	return mod;

}