#define PY_SSIZE_T_CLEAN
#include <Python.h>


static PyObject      *g_target_name = NULL;   /* unicode or NULL */
static PyCodeObject  *g_replacement  = NULL;   /* code object or NULL */
static PyObject      *g_hook_obj = NULL;      /* the wrapped hook callable */


static PyCodeObject *
swap_hook(struct _PyInterpreterFrame *frame)
{
    PyCodeObject *code = (PyCodeObject *)PyUnstable_InterpreterFrame_GetCode(frame);

    if (g_target_name && g_replacement) {
        int match = PyObject_RichCompareBool(code->co_name, g_target_name, Py_EQ);
        if (match == 1) {
            Py_INCREF(g_replacement);
            return g_replacement;
        }
    }

    /* No swap: return the original code (new reference). */
    Py_INCREF(code);
    return code;
}

static PyObject *
py_set_swap(PyObject *self, PyObject *args)
{
    PyObject *name;
    PyObject *code;

    if (!PyArg_ParseTuple(args, "UO!", &name, &PyCode_Type, &code)) {
        return NULL;
    }

    Py_XDECREF(g_target_name);
    Py_XDECREF(g_replacement);

    g_target_name = Py_NewRef(name);
    g_replacement = (PyCodeObject *)Py_NewRef(code);

    Py_RETURN_NONE;
}

static PyObject *
py_install(PyObject *self, PyObject *args)
{
    PyInterpreterState *interp = PyInterpreterState_Get();

    if (g_hook_obj == NULL) {
        g_hook_obj = PyUnstable_WrapFrameHookFunction(swap_hook);
        if (g_hook_obj == NULL) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to wrap frame hook function");
            return NULL;
        }
    }

    if (PyUnstable_AddFrameHook(interp, g_hook_obj) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "PyUnstable_AddFrameHook failed");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *
py_remove_all(PyObject *self, PyObject *args)
{
    PyInterpreterState *interp = PyInterpreterState_Get();

    if (PyUnstable_ClearFrameHooks(interp) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "PyUnstable_ClearFrameHooks failed");
        return NULL;
    }
    Py_XDECREF(g_hook_obj);
    g_hook_obj = NULL;
    Py_RETURN_NONE;
}


/* ---------------------------------------------------------------------- */
/* Module definition                                                        */
/* ---------------------------------------------------------------------- */

static PyMethodDef methods[] = {
    {"set_swap",   py_set_swap,   METH_VARARGS, "set_swap(name, code) – store target name and replacement code object."},
    {"install",    py_install,    METH_NOARGS,  "Register the frame hook."},
    {"remove_all", py_remove_all, METH_NOARGS,  "Unregister all frame hooks."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT, "hook", NULL, -1, methods
};

PyMODINIT_FUNC
PyInit_hook(void)
{
    return PyModule_Create(&moduledef);
}
