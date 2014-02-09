#ifndef _pyModule_h_
#define _pyModule_h_

#include "CXX/Objects.hxx"
#include "CXX/Extensions.hxx"

class pyPlasmaModule : public Py::ExtensionModule<pyPlasmaModule>
{
public:
    pyPlasmaModule();
    virtual ~pyPlasmaModule() { };
};

#endif //_pyModule_h_
