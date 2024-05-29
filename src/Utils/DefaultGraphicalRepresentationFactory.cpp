/*----------------------------------------------------------------------------*/
/*
 * \file DefaultGraphicalRepresentationFactory.cpp
 *
 *  \author Charles PIGNEROL
 *
 *  \date 23/07/2012
 */
/*----------------------------------------------------------------------------*/
#include "Utils/DefaultGraphicalRepresentationFactory.h"
#include "Utils/Common.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>

#include <iostream>
using namespace std;


namespace Mgx3D
{

namespace Utils
{


// =========================================================================
//                 LA CLASSE DefaultGraphicalRepresentationFactory
// =========================================================================


DefaultGraphicalRepresentationFactory::DefaultGraphicalRepresentationFactory ( )
	: GraphicalRepresentationFactoryIfc ( )
{
}	// DefaultGraphicalRepresentationFactory::DefaultGraphicalRepresentationFactory


DefaultGraphicalRepresentationFactory::DefaultGraphicalRepresentationFactory (
								const DefaultGraphicalRepresentationFactory&)
	: GraphicalRepresentationFactoryIfc ( )
{
	MGX_FORBIDDEN ("DefaultGraphicalRepresentationFactory copy constructor is not allowed.")
}	// DefaultGraphicalRepresentationFactory::DefaultGraphicalRepresentationFactory


DefaultGraphicalRepresentationFactory&
			DefaultGraphicalRepresentationFactory::operator = (
								const DefaultGraphicalRepresentationFactory&)
{
	MGX_FORBIDDEN ("DefaultGraphicalRepresentationFactory assignment operator is not allowed.")
	return *this;
}	// DefaultGraphicalRepresentationFactory::operator =


DefaultGraphicalRepresentationFactory::~DefaultGraphicalRepresentationFactory ( )
{
}	// DefaultGraphicalRepresentationFactory::~DefaultGraphicalRepresentationFactory


DisplayProperties::GraphicalRepresentation*
						DefaultGraphicalRepresentationFactory::create (Entity&)
{
	return new DisplayProperties::GraphicalRepresentation ( );
}	// DefaultGraphicalRepresentationFactory::create


} // end namespace Utils

} // end namespace Mgx3D
