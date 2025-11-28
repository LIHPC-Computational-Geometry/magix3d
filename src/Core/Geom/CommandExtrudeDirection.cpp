/*----------------------------------------------------------------------------*/
#include "Geom/CommandExtrudeDirection.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomExtrudeImplementation.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandExtrudeDirection::
CommandExtrudeDirection(Internal::Context& c,
        std::vector<GeomEntity*>& entities,
		const Vector& dp, const bool keep)
: CommandExtrusion(c, "Direction", entities, keep)
, m_vector(dp)
{
    if (m_vector.abs2()==0.0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Le vecteur pour l'extrusion ne doit pas être nul", TkUtil::Charset::UTF_8));

    m_impl = new GeomExtrudeImplementation(c,m_entities,m_vector,m_keep);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Extrusion de";
	for (uint i=0; i<m_entities.size() && i<5; i++)
		comments << " " << m_entities[i]->getName();
	if (m_entities.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
void CommandExtrudeDirection::
internalSpecificExecute()
{
    getImpl()->perform(m_createdEntities,m_v2v, m_v2v_opp,m_v2c,
            m_c2c, m_c2c_opp, m_c2s, m_s2s, m_s2s_opp, m_s2v);

    // transmet les groupes du 2D vers le 3D
    groups2DTo3D();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/


















