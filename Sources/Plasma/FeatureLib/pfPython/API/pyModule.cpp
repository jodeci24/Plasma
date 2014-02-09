#include "pyModule.h"

// Include all the classes that are exposed to Python
#include "ptStream.h"

#define ADD_PYTHON_CLASS(MOD, TYPE) \
    TYPE::init_type(); \
    Py::Object TYPE##_class(TYPE::type()); \
    MOD[ #TYPE ] = TYPE##_class;


pyPlasmaModule::pyPlasmaModule()
: Py::ExtensionModule<pyPlasmaModule>("plasma")
{
    initialize("The Plasma Python API");

    Py::Dict dict(moduleDictionary());
    ADD_PYTHON_CLASS(dict, ptStream)
}

static pyPlasmaModule* module;

extern "C" void initplasma()
{
    module = new pyPlasmaModule();
}
