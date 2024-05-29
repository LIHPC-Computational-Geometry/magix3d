/*----------------------------------------------------------------------------*/
/*
 * \file CommandChangeLengthUnit.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9 juil. 2014
 */
/*----------------------------------------------------------------------------*/
#include "Internal/CommandChangeLengthUnit.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
CommandChangeLengthUnit::
CommandChangeLengthUnit(Internal::Context& context, const Utils::Unit::lengthUnit& lu)
: Internal::CommandInternal (context, "Changement d'unité de longueur")
, m_new_length_unit(lu)
, m_old_length_unit(context.m_length_unit)
{
	getInfoCommand ( ).addContextInfo (InfoCommand::LENGTHUNITMODIFIED);
#ifdef _DEBUG2
	std::cout<<"CommandChangeLengthUnit de "
			<<Utils::Unit::toString(m_old_length_unit)
	        <<" vers "<<Utils::Unit::toString(m_new_length_unit)
	        <<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
CommandChangeLengthUnit::
~CommandChangeLengthUnit()
{
}
/*----------------------------------------------------------------------------*/
void CommandChangeLengthUnit::
internalExecute()
{
#ifdef _DEBUG2
	std::cout<<"CommandChangeLengthUnit::internalExecute => "
	        <<Utils::Unit::toString(m_new_length_unit)
	        <<std::endl;
#endif
	getContext().m_length_unit = m_new_length_unit;
}
/*----------------------------------------------------------------------------*/
void CommandChangeLengthUnit::
internalUndo()
{
	getContext().m_length_unit = m_old_length_unit;
}
/*----------------------------------------------------------------------------*/
void CommandChangeLengthUnit::
internalRedo()
{
	getContext().m_length_unit = m_new_length_unit;
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
