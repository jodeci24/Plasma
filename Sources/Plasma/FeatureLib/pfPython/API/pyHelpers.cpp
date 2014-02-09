#include "pyHelpers.h"

plString PyString_plStringify(Py::Object obj)
{
    Py::String tmp(obj);

    if (tmp.isUnicode()) {
#if (Py_UNICODE_SIZE == 2)
        return plString::FromUtf16(reinterpret_cast<const uint16_t *>(PyUnicode_AsUnicode(*tmp)));
#elif (Py_UNICODE_SIZE == 4)
        return plString::FromUtf32(reinterpret_cast<const UniChar *>(PyUnicode_AsUnicode(*tmp)));
#else
#       error "Py_UNICODE is an unexpected size"
#endif
    } else if (tmp.isString()) {
        return plString::FromUtf8(PyString_AsString(*tmp));
    }

    return plString::Null;
}
