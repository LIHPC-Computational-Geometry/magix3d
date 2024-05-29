/*----------------------------------------------------------------------------*/
/*
 * \file SysCoord.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18 mai 2018
 */
/*----------------------------------------------------------------------------*/
#include <SysCoord/SysCoord.h>
#include <SysCoord/SysCoordDisplayRepresentation.h>
#include <SysCoord/CommandEditSysCoord.h>
#include "Internal/Context.h"
#include "Utils/Common.h"
#include "Internal/InfoCommand.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <string>
#include "TkUtil/MemoryError.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
const char* SysCoord::typeNameSysCoord = "SysCoord";
/*----------------------------------------------------------------------------*/
SysCoord::SysCoord(Internal::Context& ctx)
: Internal::InternalEntity (ctx,
		ctx.newProperty(Utils::Entity::SysCoord),
		ctx.newDisplayProperties(Utils::Entity::SysCoord))
, m_property(new SysCoordProperty())
, m_save_property(0)
{
	ctx.newGraphicalRepresentation (*this);
}
/*----------------------------------------------------------------------------*/
SysCoord::SysCoord(Internal::Context& ctx, const Utils::Math::Point& pt)
: Internal::InternalEntity (ctx,
		ctx.newProperty(Utils::Entity::SysCoord),
		ctx.newDisplayProperties(Utils::Entity::SysCoord))
, m_property(new SysCoordProperty())
, m_save_property(0)
{
	m_property->setCenter(pt);
	ctx.newGraphicalRepresentation (*this);
}
/*----------------------------------------------------------------------------*/
SysCoord::SysCoord(Internal::Context& ctx,
		const Utils::Math::Point& pCentre,
		const Utils::Math::Point& pX,
		const Utils::Math::Point& pY)
: Internal::InternalEntity (ctx,
		ctx.newProperty(Utils::Entity::SysCoord),
		ctx.newDisplayProperties(Utils::Entity::SysCoord))
, m_property(new SysCoordProperty())
, m_save_property(0)
{
	m_property->initProperties(pCentre, pX, pY);

	update();

	ctx.newGraphicalRepresentation (*this);
}
/*----------------------------------------------------------------------------*/
void SysCoord::getRepresentation(
		Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
    if (dr.getDisplayType()!= Utils::DisplayRepresentation::DISPLAY_SYSCOORD)
        throw TkUtil::Exception (TkUtil::UTF8String ("Invalid display type entity", TkUtil::Charset::UTF_8));

    if (isDestroyed() && (true == checkDestroyed)){
#ifdef _DEBUG_REPRESENTATION
    	std::cout <<"Repère "<<getName()<<" DETRUIT !!!"<<std::endl;
#endif
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Interdit d'afficher une entité détruite ("<<getName()<<")";
        throw TkUtil::Exception(messErr);
    }

    if (dr.hasRepresentation(Utils::DisplayRepresentation::SHOWTRIHEDRON)){
    	SysCoordDisplayRepresentation* scdr = dynamic_cast<SysCoordDisplayRepresentation*> (&dr);
    	CHECK_NULL_PTR_ERROR(scdr);
	scdr->getTransform ( ).setCenter (m_property->getCenter ( ));
	scdr->getTransform ( ).setTransf (m_property->getTransf ( ));
    }
}
/*----------------------------------------------------------------------------*/
Mgx3D::Utils::SerializedRepresentation* SysCoord::getDescription (
    		bool alsoComputed) const
{
	std::unique_ptr<Utils::SerializedRepresentation>	description	(
				InternalEntity::getDescription (alsoComputed));
	CHECK_NULL_PTR_ERROR (description.get ( ));

    Utils::SerializedRepresentation  repProprietes ("Propriétés du repère local", "");

	std::vector<double>	coords;
	Utils::Math::Point center = m_property->getCenter();
	coords.push_back (center.getX ( ));
	coords.push_back (center.getY ( ));
	coords.push_back (center.getZ ( ));
	Mgx3D::Utils::SerializedRepresentation::Property	centerProp (
			"Centre", coords);
	repProprietes.addProperty(centerProp);

	for (uint i=0; i<3; i++)
		for (uint j=0;j<3; j++){
			TkUtil::UTF8String nom (TkUtil::Charset::UTF_8);
			nom <<"transf["<<(short int)i<<","<<(short int)j<<"]";
			Mgx3D::Utils::SerializedRepresentation::Property	matrixProp (
					nom.utf8(), m_property->getTransfCoord(i,j));
			repProprietes.addProperty(matrixProp);
		}

    description->addPropertiesSet(repProprietes);

	return description.release ( );
}
/*----------------------------------------------------------------------------*/
std::string SysCoord::getSummary ( ) const
{
	std::vector<double>	coords;
	Utils::Math::Point center = m_property->getCenter();
	coords.push_back (center.getX ( ));
	coords.push_back (center.getY ( ));
	coords.push_back (center.getZ ( ));
	Mgx3D::Utils::SerializedRepresentation::Property	centerProp (
			"Centre", coords);
	return centerProp.getValue ( );
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point SysCoord::toLocal(const Utils::Math::Point& pt)
{
	return m_property->getTransf()*(pt-m_property->getCenter());
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point SysCoord::toGlobal(const Utils::Math::Point& pt)
{
	return m_property->getCenter() + m_inv*pt;
}
/*----------------------------------------------------------------------------*/
bool SysCoord::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*----------------------------------------------------------------------------*/
void SysCoord::update()
{
	m_inv = m_property->getTransf();
	m_inv.inverse();
}
/*----------------------------------------------------------------------------*/
void SysCoord::saveProperty(Internal::InfoCommand* icmd)
{
	if (icmd) {
		icmd->addSysCoordInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
		if (m_save_property == 0)
			m_save_property = m_property->clone();
	}
}
/*----------------------------------------------------------------------------*/
void SysCoord::saveInternals(CommandEditSysCoord* cest)
{
	if (m_save_property){
		cest->setInfoProperty(this, m_save_property);
		m_save_property = 0;
	}
}
/*----------------------------------------------------------------------------*/
SysCoordProperty* SysCoord::setProperty(SysCoordProperty* prop)
{
	SysCoordProperty* tmp = m_property;
	m_property = prop;
	update();
	return tmp;
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
