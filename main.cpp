
/*
  The way to make an iter object in Python.
  https://stackoverflow.com/questions/1815812/how-to-create-a-generator-iterator-with-the-python-c-api

  requires Admin
*/
#include <python.h>
#include <stringobject.h>
#include <unicodeobject.h>
#include <string>
#include "file_iterator_usn.h"

typedef struct {
  PyObject_HEAD
    base::file_iterator_usn_t fiu;
}PyFileIteratorUsn_Iter;


static
PyObject *
PyFileIteratorUsn_Iter_iter(PyObject * self)
{
  Py_INCREF(self);
  return self;
}

static
PyObject *
PyFileIteratorUsn_Iter_iternext(PyObject * self)
{
  PyFileIteratorUsn_Iter * p = (PyFileIteratorUsn_Iter*)self;
  int err;

  err = p->fiu.next();
  if (err)
  {
    PyErr_SetString(PyExc_StopIteration, "<PyFileIteratorUsn_Iter_iternext> no more files");
    return NULL;
  }
  else if (sizeof(Py_UNICODE) != sizeof(wchar_t))
  {
    PyErr_SetString(PyExc_StopIteration, "<PyFileIteratorUsn_Iter_iternext>sizeof(Py_UNICODE) != sizeof(wchar_t)");
    return NULL;
  }
  else
  {
    return Py_BuildValue("u#", p->fiu.value().c_str(), (int)p->fiu.value().size());
  }
}

void 
PyFileIteratorUsn_Iter_destructor(PyObject * self)
{
  /* use AppVerify to detect memory leak */
  PyFileIteratorUsn_Iter * p = (PyFileIteratorUsn_Iter*)self;
  p->fiu.release();
}

static
PyTypeObject
PyFileIteratorUsn_IterType = {
  PyObject_HEAD_INIT(NULL)
  0,                         /*ob_size*/
  "PyFileIteratorUsn._Iter",            /*tp_name*/
  sizeof(PyFileIteratorUsn_Iter),       /*tp_basicsize*/
  0,                         /*tp_itemsize*/
  PyFileIteratorUsn_Iter_destructor, /*tp_dealloc*/
  0,                         /*tp_print*/
  0,                         /*tp_getattr*/
  0,                         /*tp_setattr*/
  0,                         /*tp_compare*/
  0,                         /*tp_repr*/
  0,                         /*tp_as_number*/
  0,                         /*tp_as_sequence*/
  0,                         /*tp_as_mapping*/
  0,                         /*tp_hash */
  0,                         /*tp_call*/
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_ITER,
  /* tp_flags: Py_TPFLAGS_HAVE_ITER tells python to
  use tp_iter and tp_iternext fields. */
  "PyFileIteratorUsn_Iter iterator object.",           /* tp_doc */
  0,  /* tp_traverse */
  0,  /* tp_clear */
  0,  /* tp_richcompare */
  0,  /* tp_weaklistoffset */
  PyFileIteratorUsn_Iter_iter,  /* tp_iter: __iter__() method */
  PyFileIteratorUsn_Iter_iternext  /* tp_iternext: next() method */
};


static
PyObject *
iter_drive(PyObject * self, PyObject * args)
{
  int err;
  int r;
  PyObject * drive = 0;
  Py_UNICODE * addr;
  Py_ssize_t size;
  std::wstring temp;
  PyFileIteratorUsn_Iter * obj;

  /* unicode type check */
  if (sizeof(Py_UNICODE) != sizeof(wchar_t))
  {
    PyErr_SetString(PyExc_ValueError, "<PyFileIteratorUsn_iter_drive> sizeof(Py_UNICODE) != sizeof(wchar_t)");
    Py_INCREF(Py_None);
    return Py_None;
  }

  r = PyArg_ParseTuple(args, "O", &drive);
  if (r == 0) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  if (!PyUnicode_Check(drive)) {
    Py_INCREF(Py_None);
    return Py_None;
  }
  addr = PyUnicode_AS_UNICODE(drive);
  size = PyUnicode_GET_SIZE(drive);

  if (!(addr && size)) {
    PyErr_SetString(PyExc_ValueError, "<PyFileIteratorUsn_iter_drive> UNICODE data and size are invalid");
    Py_INCREF(Py_None);
    return Py_None;
  }
  temp.assign(addr, size);

  /* PyObject_CallObject replace PyObject_New(), PyObject_Init() */
  obj = (PyFileIteratorUsn_Iter*)PyObject_CallObject((PyObject*)&PyFileIteratorUsn_IterType, NULL);
  if (!obj) {
    PyErr_SetString(PyExc_ValueError, "<PyFileIteratorUsn_iter_drive> fail PyObject_CallObject()");
    Py_INCREF(Py_None);
    return Py_None;
  }

  //   obj = PyObject_New(PyFileIteratorUsn_Iter, &PyFileIteratorUsn_IterType);
  //   if (!obj) {
  //     PyErr_SetString(PyExc_ValueError, "<PyFileIteratorUsn_iter_drive> fail PyObject_New()");
  //     Py_INCREF(Py_None);
  //     return Py_None;
  //   }
  //   if (!PyObject_Init((PyObject *)obj, &PyFileIteratorUsn_IterType)) {
  //     PyErr_SetString(PyExc_ValueError, "<PyFileIteratorUsn_iter_drive> fail PyObject_Init()");
  //     Py_DECREF(obj);
  //     Py_INCREF(Py_None);
  //     return Py_None;
  //   }

  err = obj->fiu.create();
  if (err) {
    PyErr_SetString(PyExc_ValueError, "<PyFileIteratorUsn_iter_drive> fail file_iterator_usn.create");
    Py_DECREF(obj);
    Py_INCREF(Py_None);
    return Py_None;
  }

  obj->fiu.clear();
  err = obj->fiu.set_drive(temp);
  if (err)
  {
    PyErr_SetString(PyExc_ValueError, "<PyFileIteratorUsn_iter_drive> fail file_iterator_usn.set_drive");
    Py_DECREF(obj);
    Py_INCREF(Py_None);
    return Py_None;
  }
  return (PyObject *)obj;
}


static
PyMethodDef
SpamMethods[] = {
  { "iter_drive",  iter_drive, METH_VARARGS, "Iterate paths from given drive letter, such as c c: c:\\" },
  { NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC
initlibfileiterusn(void)
{
  PyObject* m;

  PyFileIteratorUsn_IterType.tp_new = PyType_GenericNew;
  if (PyType_Ready(&PyFileIteratorUsn_IterType) < 0)  return;

  m = Py_InitModule("libfileiterusn", SpamMethods);

  Py_INCREF(&PyFileIteratorUsn_IterType);
  PyModule_AddObject(m, "_Iter_Drive", (PyObject *)&PyFileIteratorUsn_IterType);
}