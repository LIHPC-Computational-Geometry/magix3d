#ifndef _INFO_SERVICE_H_
#define _INFO_SERVICE_H_

#include "Geom/GeomInfo.h"

namespace Mgx3D::Geom {
    class GeomEntity;
}

namespace Mgx3D::Services
{
    struct InfoService
    {
        static Geom::GeomInfo getInfos(const Geom::GeomEntity*);
    };
}

#endif /* _INFO_SERVICE_H_ */
