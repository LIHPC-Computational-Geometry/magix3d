/*----------------------------------------------------------------------------*/
#include "Geom/GeomExport.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomExport::
GeomExport(Internal::Context& c, const std::string& n)
: m_context(c), m_geomEntities(), m_filename(n)
{}
/*----------------------------------------------------------------------------*/
GeomExport::
GeomExport(Internal::Context& c,
		const std::vector<GeomEntity*>& ge,
		const std::string& n)
: m_context(c), m_geomEntities(ge.begin(), ge.end()), m_filename(n)
{}
/*----------------------------------------------------------------------------*/
GeomExport::
GeomExport(Internal::Context& c,
		GeomEntity* ge,
		const std::string& n)
: m_context(c), m_geomEntities(), m_filename(n)
{
	m_geomEntities.push_back(ge);
}
/*----------------------------------------------------------------------------*/
GeomExport::~GeomExport()
{}
/*----------------------------------------------------------------------------*/
void GeomExport::perform(Internal::InfoCommand* icmd)
{
    Mgx3D::Geom::GeomManager& gm = m_context.getGeomManager();

    // si rien de sélectionné, on prend tout ce qui est dans le manager
    if (m_geomEntities.empty()){

    	//=============================================================
    	//recuperation de tous les volumes
    	//=============================================================
    	std::vector<Volume*> vols = gm.getVolumesObj();
    	for(unsigned int i=0;i<vols.size();i++)
    	{
    		Volume* ge = vols[i];
    		addGeomEntityToExport(ge);
    	}

    	//=============================================================
    	//recuperation de toutes les surfaces
    	//=============================================================
    	std::vector<Surface*> surfs = gm.getSurfacesObj();
    	for(unsigned int i=0;i<surfs.size();i++)
    	{
    		Surface* ge = surfs[i];
    		auto adj_vol = ge->getVolumes();
    		if(adj_vol.empty()){
    			addGeomEntityToExport(ge);
    		}
    	}

    	//=============================================================
    	//recuperation de toutes les courbes
    	//=============================================================
    	std::vector<Curve*>  curvs = gm.getCurvesObj();
    	for(unsigned int i=0;i<curvs.size();i++)
    	{
    		Curve* ge = curvs[i];
    		auto adj_surf = ge->getSurfaces();
    		if(adj_surf.empty()){
    			addGeomEntityToExport(ge);
    		}
    	}

    	//=============================================================
    	//recuperation de tous les sommets
    	//=============================================================
    	std::vector<Vertex*> verts  = gm.getVerticesObj();
    	for(unsigned int i=0;i<verts.size();i++)
    	{
    		Vertex* ge = verts[i];
    		auto adj_curves = ge->getCurves();
    		if(adj_curves.empty()){
    			addGeomEntityToExport(ge);
    		}
    	}

    } // end  if (m_geomEntities.empty())
    else {
    	for (unsigned int i=0;i<m_geomEntities.size();i++)
    		addGeomEntityToExport(m_geomEntities[i]);
    }


    write();

}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
