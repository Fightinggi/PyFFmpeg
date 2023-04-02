#include "pch.h"
#include "PyFFmpeg.h"
#include "XFFmpeg.h"


//开放给python调用的
PyObject* PyFFmpeg::Create(PyTypeObject* type, PyObject* args, PyObject* kw)
{
	printf("PyFFmpeg::Create new\n");
	//先给PyFFmpeg分配空间 再给他的成员XFFmpeg* ff;分配空间
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
	//先清理成员XFFmpeg* ff的空间，在清理self
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

//属性函数get
PyObject* PyFFmpeg::GetTotalms(PyFFmpeg* self, void* closure)
{
	return PyLong_FromLong(self->ff->Totalms);
}

PyObject* PyFFmpeg::GetFps(PyFFmpeg* self, void* closure)
{
	return PyLong_FromLong(self->ff->fps);
}

//PyInit_模块名
//1. 扩展库入口函数  PyMODINIT_FUNC宏 extern c __declspec(dllexport)
PyMODINIT_FUNC PyInit_pyffmpeg(void)  // define PyMODINIT_FUNC __declspec(dllexport) PyObject*
{
	//========定义模块=======
	PyObject* mod = NULL;
	static PyModuleDef ffmod = {
	PyModuleDef_HEAD_INIT,
	"pyffmpeg",                           //"模块名"，
	"pyffmpeg is first module test",      //"模块说明",    help(模块名)
	-1,                               //模块空间，
	0                    //模块函数列表 
	};
	printf("PyInit_pyffmpeg\n");
	mod = PyModule_Create(&ffmod);

	//===添加设置类型基本属性====
	static PyTypeObject type;
	memset(&type, 0, sizeof(type));
	type.ob_base = {PyObject_HEAD_INIT(NULL) 0};
	type.tp_name = "pyffmpeg.PyFFmpeg";
	type.tp_basicsize = sizeof(PyFFmpeg);
	type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
	type.tp_itemsize = 0;
	//===创建类的对象函数、类的构造析构函数======
	type.tp_new = PyFFmpeg::Create;//类内静态函数，类外初始化，函数在代码区，不占用对象内存空间
	type.tp_init = (initproc)PyFFmpeg::Init;
	type.tp_dealloc = (destructor)PyFFmpeg::Close;
	//===创建类的成员函数============
	static PyMethodDef ffmeth[] = {
		{"open",(PyCFunction)PyFFmpeg::Open,METH_VARARGS,"PyFFmpeg::Open"},
		{"read",(PyCFunction)PyFFmpeg::Read,METH_NOARGS,"PyFFmpeg::Read"},
		{"decode",(PyCFunction)PyFFmpeg::Decode,METH_VARARGS,"PyFFmpeg::Decode"},
		{NULL}
	};
	type.tp_methods = ffmeth;
	//===创建属性函数set、get=========
	static PyGetSetDef getsets[] = {
		{"GetTotalms",(getter)PyFFmpeg::GetTotalms,0,0,0 },
		{"fps",(getter)PyFFmpeg::GetFps,0,0,0 },
		{NULL }
	};
	type.tp_getset = getsets;
	//初始化类
	if (PyType_Ready(&type) < 0)
		return NULL;
	//引用计数+1
	Py_INCREF(&type);
	
	PyModule_AddObject(mod, "PyFFmpeg", (PyObject*) &type);	
	return mod;

}