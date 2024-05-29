/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER

/*
 * \file ImportMDLItf.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 8/7/16
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/GeomManager.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Internal/ImportMDLItf.h"
#include "Geom/EntityFactory.h"
#include "Topo/EdgeMeshingPropertyGeometric.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyBigeometric.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/Edge.h"
#include "Topo/CoFace.h"
#include "Topo/TopoHelper.h"
#include "Group/GroupManager.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"
#include "Internal/InfoCommand.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <sstream>
#include <iostream>
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MDL
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
ImportMDLItf::
ImportMDLItf(Internal::Context& c,
        const std::string& n)
: m_context(c), m_filename(n), m_mi(0), m_mdl_info(0), m_do_scale(false), m_scale_factor(1.0)
{}
/*----------------------------------------------------------------------------*/
ImportMDLItf::~ImportMDLItf()
{
    // fermeture du fichier
    if (m_mi)
        ::MdlClose(m_mi);

    // destruction de la structure issue du modèle lu
    if (m_mdl_info)
        delete m_mdl_info;
}
/*----------------------------------------------------------------------------*/
void ImportMDLItf::readMDL()
{
#ifdef _DEBUG_MDL
    std::cout << "ImportMDLItf::readMDL() en cours ..."<<std::endl;
#endif
	// lecture déjà faite
	if (m_mi != 0)
		return;

    // ouverture du fichier
    m_mi = ::MdlOpen(m_filename.c_str());
    if (!m_mi){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur lors de l'ouverture du fichier au format Mdl: "<<m_filename;
        throw TkUtil::Exception(messErr);
    }

    // lecture du modèle
    m_mdl_info = new T_MdlInfo;
    int ret = ::MdlRead(m_mi,m_mdl_info);
    if (ret){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur lors de la lecture du fichier Mdl: "<<m_filename;
        messErr <<", au nombre de "<<(short)m_mdl_info->nb_error
                <<" et avec comme message: \""<<m_mdl_info->error_str<<"\"";
        throw TkUtil::Exception(messErr);
    }
    if (!m_mdl_info){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur lors de l'interprétation du fichier Mdl: "<<m_filename;
        throw TkUtil::Exception(messErr);
    }

}
/*----------------------------------------------------------------------------*/
Utils::Unit::lengthUnit ImportMDLItf::getLengthUnit()
{
	readMDL();

	return (Utils::Unit::lengthUnit)(m_mdl_info->length_unit);
}
/*----------------------------------------------------------------------------*/
void ImportMDLItf::setScale(const double& factor)
{
	m_do_scale = true;
	m_scale_factor = factor;
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D

#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/