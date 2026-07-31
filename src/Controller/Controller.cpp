#include "protected/Controller/Controller.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D
{
    Controller::Controller(){}

    void Controller::init(Internal::Context* context)
    {
        setContext(context);
    }

    void Controller::setContext(Internal::Context* context)
    {
        if (context == _context)
            return;


        delete _context;
        _context = context;
    }
}
