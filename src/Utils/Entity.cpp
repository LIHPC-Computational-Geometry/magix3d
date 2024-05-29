/*----------------------------------------------------------------------------*/
/*
 * \file Entity.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#include <string>
#include <memory>				// unique_ptr

#include "Utils/Entity.h"
#include "Utils/Common.h"
#include "Utils/DisplayProperties.h"
#include "Utils/Property.h"
#include "Utils/MgxNumeric.h"
#include "Utils/Vector.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/LogOutputStream.h>

#include <values.h>				// CP : DBL_MAX
#include <sys/types.h>			// CP : uint sur Bull
#include <TkUtil/NumericConversions.h>
#include <TkUtil/NumericServices.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MEMORY
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
static void complete (TkUtil::UTF8String& name, FilterEntity::objectType ot)
{
	if(0 != name.length ( ))
		name << "|";
	name << FilterEntity::filterToObjectTypeName (ot);
}
/*----------------------------------------------------------------------------*/
std::string FilterEntity::filterToObjectTypeName (FilterEntity::objectType ot)
{
	switch (ot)
	{
		case NoneEntity	    : return "";
		case GeomVertex		: return "Points";
		case GeomCurve		: return "Courbes";
		case GeomSurface	: return "Surfaces";
		case GeomVolume		: return "Volumes";
		case TopoVertex		: return "Sommets";
		case TopoCoEdge		: return "Arêtes";
		case TopoEdge		: return "Arêtes";
		case TopoCoFace		: return "Faces";
		case TopoFace		: return "Faces";
		case TopoBlock		: return "Blocs";
		case MeshCloud		: return "Nuages";
		case MeshLine 		: return "Lignes";
		case MeshSurface	: return "Surfaces";
		case MeshSubSurface	: return "Sous Surfaces";
		case MeshVolume		: return "Volumes";
		case MeshSubVolume	: return "Sous Volumes";
		case Group0D		: return "Groupes 0D";
		case Group1D		: return "Groupes 1D";
		case Group2D		: return "Groupes 2D";
		case Group3D		: return "Groupes 3D";
		case SysCoord		: return "Repères";
		case StructuredMesh	: return "Maillage structuré";
	}	// switch (ot)

	// C'est une combinaison de types :
	TkUtil::UTF8String	typeName (TkUtil::Charset::UTF_8);
	if (0 != (ot & GeomVertex))
		complete (typeName, GeomVertex);
	if (0 != (ot & GeomCurve))
		complete (typeName,GeomCurve);
	if (0 != (ot & GeomSurface))
		complete (typeName,GeomSurface);
	if (0 != (ot & GeomVolume))
		complete (typeName,GeomVolume);
	if (0 != (ot & TopoVertex))
		complete (typeName,TopoVertex);
	if (0 != (ot & TopoCoEdge))
		complete (typeName,TopoCoEdge);
	if (0 != (ot & TopoEdge))
		complete (typeName,TopoEdge);
	if (0 != (ot & TopoCoFace))
		complete (typeName,TopoCoFace);
	if (0 != (ot & TopoFace))
		complete (typeName,TopoFace);
	if (0 != (ot & TopoBlock))
		complete (typeName,TopoBlock);
	if (0 != (ot & MeshCloud))
		complete (typeName,MeshCloud);
	if (0 != (ot & MeshLine))
		complete (typeName,MeshLine);
	if (0 != (ot & MeshSurface))
		complete (typeName,MeshSurface);
	if (0 != (ot & MeshSubSurface))
		complete (typeName,MeshSubSurface);
	if (0 != (ot & MeshVolume))
		complete (typeName,MeshVolume);
	if (0 != (ot & MeshSubVolume))
		complete (typeName,MeshSubVolume);
	if (0 != (ot & Group0D))
		complete (typeName,Group0D);
	if (0 != (ot & Group1D))
		complete (typeName,Group1D);
	if (0 != (ot & Group2D))
		complete (typeName,Group2D);
	if (0 != (ot & Group3D))
		complete (typeName,Group3D);
	if (0 != (ot & SysCoord))
		complete (typeName,SysCoord);
	if (0 != (ot & StructuredMesh))
		complete (typeName,StructuredMesh);

	return typeName.iso ( );
}
/*----------------------------------------------------------------------------*/
std::string FilterEntity::filterToFullObjectTypeName (FilterEntity::objectType ot)
{
	switch (ot)
	{
		case GeomVertex		: return "GeomVertex";
		case GeomCurve		: return "GeomCurve";
		case GeomSurface	: return "GeomSurface";
		case GeomVolume		: return "GeomVolume";
		case TopoVertex		: return "TopoVertex";
		case TopoCoEdge		: return "TopoCoEdge";
		case TopoEdge		: return "TopoEdge";
		case TopoCoFace		: return "TopoCoFace";
		case TopoFace		: return "TopoFace";
		case TopoBlock		: return "TopoBlock";
		case MeshCloud		: return "MeshCloud";
		case MeshLine		: return "MeshLine";
		case MeshSurface	: return "MeshSurface";
		case MeshSubSurface	: return "MeshSubSurface";
		case MeshVolume		: return "MeshVolume";
		case MeshSubVolume	: return "MeshSubVolume";
		case Group0D		: return "Group0D";
		case Group1D		: return "Group1D";
		case Group2D		: return "Group2D";
		case Group3D		: return "Group3D";
		case SysCoord		: return "Repère";
		case StructuredMesh	: return "StructuredMesh";
	}	// switch (ot)

	TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
	error << "Type d'entité invalide : " << (unsigned long)ot;
	INTERNAL_ERROR(exc, error, "FilterEntity::filterToFullObjectTypeName")
	throw TkUtil::Exception (exc);
}
/*----------------------------------------------------------------------------*/
std::string Entity::objectTypeToObjectTypeName (objectType t)
{
	switch (t)
	{
		case Entity::GeomVertex		: return "GeomVertex";
		case Entity::GeomCurve		: return "GeomCurve";
		case Entity::GeomSurface	: return "GeomSurface";
		case Entity::GeomVolume		: return "GeomVolume";
		case Entity::TopoVertex		: return "TopoVertex";
		case Entity::TopoCoEdge		: return "TopoCoEdge";
		case Entity::TopoEdge		: return "TopoEdge";
		case Entity::TopoCoFace		: return "TopoCoFace";
		case Entity::TopoFace		: return "TopoFace";
		case Entity::TopoBlock		: return "TopoBlock";
		case Entity::MeshCloud		: return "MeshCloud";
		case Entity::MeshLine		: return "MeshLine";
		case Entity::MeshSurface	: return "MeshSurface";
		case Entity::MeshSubSurface	: return "MeshSubSurface";
		case Entity::MeshVolume		: return "MeshVolume";
		case Entity::MeshSubVolume	: return "MeshSubVolume";
		case Entity::Group0D		: return "Group0D";
		case Entity::Group1D		: return "Group1D";
		case Entity::Group2D		: return "Group2D";
		case Entity::Group3D		: return "Group3D";
		case Entity::SysCoord		: return "Repère";
		case Entity::StructuredMesh	: return "StructuredMesh";
		case Entity::undefined		: return "undefined";
	}	// switch (t)

	TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
	error << "Erreur dans Entity::objectTypeToObjectTypeName : type "
	      << (unsigned long)t << " non recensé.";
	throw TkUtil::Exception (error);
}
/*----------------------------------------------------------------------------*/
int Entity::objectTypeToDimension (objectType t)
{
	switch (t)
	{
		case Entity::GeomVertex		:
		case Entity::TopoVertex		:
		case Entity::MeshCloud		:
        case Entity::Group0D        : return 0;
		case Entity::GeomCurve		:
		case Entity::TopoCoEdge		:
		case Entity::TopoEdge		:
		case Entity::MeshLine		:
        case Entity::Group1D        : return 1;
		case Entity::GeomSurface	:
		case Entity::TopoCoFace		:
		case Entity::TopoFace		:
		case Entity::MeshSurface	:
		case Entity::MeshSubSurface	:
        case Entity::Group2D        : return 2;
		case Entity::GeomVolume		:
		case Entity::TopoBlock		:
		case Entity::MeshVolume		:
        	case Entity::MeshSubVolume  	:
		case Entity::StructuredMesh	:
        case Entity::Group3D        : return 3;
        	case Entity::SysCoord       : return -1;
		case Entity::undefined		: ;
	}	// switch (t)

	TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
	error << "Erreur dans Entity::objectTypeToDimension : type "
	      << (unsigned long)t << " non recensé.";
	throw TkUtil::Exception (error);
}
/*----------------------------------------------------------------------------*/
Entity::Entity(unsigned long id, Property* prop,
        DisplayProperties* disp, TkUtil::LogOutputStream* logStream)
: m_destroyed(false)
, m_unique_id(id)
, m_prop(prop)
, m_displayProperties (disp)
, m_log_stream(logStream)
{
#ifdef _DEBUG_MEMORY
    std::cout<<"Entity::Entity() de "<<getName()<<" (uid "<< m_unique_id<<")"<<std::endl;
//    std::cout<<"  m_prop en "<<m_prop<<std::endl;
//    std::cout<<"  m_displayProperties en "<<m_displayProperties<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
Entity::~Entity()
{
#ifdef _DEBUG_MEMORY
    std::cout<<"Entity::~Entity() de "<<getName()<<" (uid "<< m_unique_id<<")"<<std::endl;
//    std::cout<<"  m_prop en "<<m_prop<<std::endl;
//    std::cout<<"  m_displayProperties en "<<m_displayProperties<<std::endl;
#endif

	if (0 != m_prop)
	    delete m_prop;
	m_prop	= 0;
	if (0 != m_displayProperties)
	    delete m_displayProperties;
	m_displayProperties	= 0;
}
/*----------------------------------------------------------------------------*/
void Entity::getBounds (double bounds[6]) const
{
	bounds [0]	= bounds [2]	= bounds [4]	= DBL_MAX;
	bounds [1]	= bounds [3]	= bounds [5]	= -DBL_MAX;
}
/*----------------------------------------------------------------------------*/
void Entity::getRepresentation(
						DisplayRepresentation& dr, bool checkDestroyed) const
{
	throw TkUtil::Exception(TkUtil::UTF8String ("Entity::getRepresentation doit être redéfini", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
SerializedRepresentation* Entity::getDescription(bool alsoComputed) const
{
	//std::cout<<"Entity::getDescription( ) pour "<<getName()<<std::endl;

	std::unique_ptr<SerializedRepresentation>	representation (new SerializedRepresentation ("Description", ""));

	Mgx3D::Utils::SerializedRepresentation  proprietes ("Propriétés génériques", "");

	// Inutile car déjà affiché
//	representation->addProperty (
//		SerializedRepresentation::Property ("Nom", getName ( )));
	proprietes.addProperty (
		SerializedRepresentation::Property ("Nom unique", getUniqueName ( )));
	proprietes.addProperty (
		SerializedRepresentation::Property ("Identifiant unique", getUniqueId ( )));
	proprietes.addProperty (
		SerializedRepresentation::Property ("Type", getTypeName ( )));
	proprietes.addProperty (
		SerializedRepresentation::Property ("Dimension", (long)getDim ( )));
	proprietes.addProperty (
        SerializedRepresentation::Property ("Est détruit", isDestroyed ( )));
	proprietes.addProperty (
        SerializedRepresentation::Property ("Est visible", isVisible ( )));


	// la boite englobante pour les entités autres que des groupes
	if (true == alsoComputed)
	{
		if (getType() < Utils::Entity::Group0D){
		    Utils::SerializedRepresentation  boundingBox ("Boite englobante", "");
		    double  bounds [] = {
	            TkUtil::NumericServices::doubleMachMax(),
				-TkUtil::NumericServices::doubleMachMax(),
	            TkUtil::NumericServices::doubleMachMax(),
				-TkUtil::NumericServices::doubleMachMax(),
	            TkUtil::NumericServices::doubleMachMax(),
				-TkUtil::NumericServices::doubleMachMax()};
		    getBounds (bounds);

		    boundingBox.addProperty (Utils::SerializedRepresentation::Property ("X min", bounds[0]));
		    boundingBox.addProperty (Utils::SerializedRepresentation::Property ("X max", bounds[1]));
		    boundingBox.addProperty (Utils::SerializedRepresentation::Property ("Y min", bounds[2]));
		    boundingBox.addProperty (Utils::SerializedRepresentation::Property ("Y max", bounds[3]));
		    boundingBox.addProperty (Utils::SerializedRepresentation::Property ("Z min", bounds[4]));
		    boundingBox.addProperty (Utils::SerializedRepresentation::Property ("Z max", bounds[5]));

		    // calcul de la longueur de la diagonale de la boite
		    Utils::Math::Vector v1 (bounds[1]-bounds[0], bounds[3]-bounds[2], bounds[5]-bounds[4]);
		    double lg = v1.abs();
		    boundingBox.setSummary(Utils::Math::MgxNumeric::userRepresentation(lg));

		    proprietes.addPropertiesSet(boundingBox);
		}
	}	// if (true == alsoComputed)

	representation->addPropertiesSet (proprietes);

	return representation.release ( );
}
/*----------------------------------------------------------------------------*/
std::string Entity::getSummary ( ) const
{
	return "";
}
/*----------------------------------------------------------------------------*/
DisplayProperties* Entity::setDisplayProperties (DisplayProperties* dp)
{
    DisplayProperties* old_disp=0;
    if(dp==0){
        throw   TkUtil::Exception("Null display property");
    }

    old_disp = m_displayProperties;
    m_displayProperties = dp;
    return old_disp;
}	// Entity::getDisplayProperties
/*----------------------------------------------------------------------------*/
Property* Entity::setProperties (Property* prop)
{
    Property* old=0;
    if(prop==0){
        throw   TkUtil::Exception("Null property");
    }

    old = m_prop;
    m_prop = prop;
    return old;
}

/*----------------------------------------------------------------------------*/
std::string Entity::getName() const
{
#ifdef _DEBUG
    if (!m_prop)
        throw  TkUtil::Exception("Null property");
#endif
    return m_prop->getName();
}
/*----------------------------------------------------------------------------*/
std::string Entity::getUniqueName() const
{
    return getTypeName() + "_" + getName();
}
/*----------------------------------------------------------------------------*/
FilterEntity::objectType Entity::getFilteredType() const
{
	switch (getType ( ))
	{
		case GeomVertex		: return FilterEntity::GeomVertex;
		case GeomCurve		: return FilterEntity::GeomCurve;
		case GeomSurface	: return FilterEntity::GeomSurface;
		case GeomVolume		: return FilterEntity::GeomVolume;
		case TopoVertex		: return FilterEntity::TopoVertex;
		case TopoCoEdge		: return FilterEntity::TopoCoEdge;
		case TopoEdge		: return FilterEntity::TopoEdge;
		case TopoCoFace		: return FilterEntity::TopoCoFace;
		case TopoFace		: return FilterEntity::TopoFace;
		case TopoBlock		: return FilterEntity::TopoBlock;
		case MeshCloud		: return FilterEntity::MeshCloud;
		case MeshLine		: return FilterEntity::MeshLine;
		case MeshSurface	: return FilterEntity::MeshSurface;
		case MeshSubSurface	: return FilterEntity::MeshSubSurface;
		case MeshVolume		: return FilterEntity::MeshVolume;
		case MeshSubVolume	: return FilterEntity::MeshSubVolume;
		case Group0D		: return FilterEntity::Group0D;
		case Group1D		: return FilterEntity::Group1D;
		case Group2D		: return FilterEntity::Group2D;
		case Group3D		: return FilterEntity::Group3D;
		case StructuredMesh	: return FilterEntity::StructuredMesh;
		case SysCoord       	: return FilterEntity::SysCoord;
		case undefined		: return FilterEntity::NoneEntity;
	}	// switch (getType ( ))

	TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
	error << "Type non supporté : " << objectTypeToObjectTypeName (getType ( ))
	      << ".";
	INTERNAL_ERROR (exc, error, "Entity::getFilteredType")
	throw exc;
}
/*----------------------------------------------------------------------------*/
bool Entity::isGeomEntity ( ) const
{
	switch (getType ( ))
	{
		case GeomVertex		:
		case GeomCurve		:
		case GeomSurface	:
		case GeomVolume		: return true;
		default			: return false;
	}	// switch (getType ( ))
}
/*----------------------------------------------------------------------------*/
bool Entity::isTopoEntity ( ) const
{
	switch (getType ( ))
	{
		case TopoVertex		:
		case TopoCoEdge		:
		case TopoEdge		:
		case TopoCoFace		:
		case TopoFace		:
		case TopoBlock		: return true;
		default			: return false;
	}	// switch (getType ( ))
}
/*----------------------------------------------------------------------------*/
bool Entity::isMeshEntity ( ) const
{
	switch (getType ( ))
	{
		case MeshCloud		:
		case MeshLine		:
		case MeshSurface	:
		case MeshVolume		: return true;
		default				: return false;
	}	// switch (getType ( ))
}
/*----------------------------------------------------------------------------*/
bool Entity::isGroupEntity ( ) const
{
    switch (getType ( ))
    {
        case Group0D    :
        case Group1D    :
        case Group2D    :
        case Group3D    : return true;
        default         : return false;
    }   // switch (getType ( ))
}
/*----------------------------------------------------------------------------*/
bool Entity::isSysCoordEntity ( ) const
{
    switch (getType ( ))
    {
        case SysCoord    : return true;
        default         : return false;
    }   // switch (getType ( ))
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> Entity::getNodesValuesNames ( ) const
{
	return std::vector<std::string> ( );
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> Entity::getCellsValuesNames ( ) const
{
	return std::vector<std::string> ( );
}
/*----------------------------------------------------------------------------*/
std::map<int,double> Entity::getNodesValue (const std::string& name)
{
	return std::map<int,double> ( );
}
/*----------------------------------------------------------------------------*/
void Entity::setDestroyed(bool b)
{
    m_destroyed = b;
    // En cas de redo ...
    // [EB] seulement pour b==false pour éviter updateRepresentation sur une entité marquée comme détruite
	if (0 != getDisplayProperties ( ).getGraphicalRepresentation ( ) && !b)
	{
		getDisplayProperties ( ).getGraphicalRepresentation ( )->setHighlighted (false);
		getDisplayProperties ( ).getGraphicalRepresentation ( )->setSelected (false);
	}	// if (0 != getDisplayProperties ( ).getGraphicalRepresentation ( ) && !b)
#ifdef _DEBUG2
    std::cout<<"Entity::setDestroyed("<<(b?"vrai":"faux")<<") pour "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void Entity::log (const TkUtil::Log& l) const
{
    if (0 != getLogStream ( ))
        getLogStream ( )->log (l);
#ifdef _DEBUG
    if (0 == getLogStream ( ))
        std::cout<<"pas de LogStream pour l'entité\n";
#endif
}
/*----------------------------------------------------------------------------*/
bool Entity::isVisible() const
{
    return getDisplayProperties().isDisplayed();
}
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
