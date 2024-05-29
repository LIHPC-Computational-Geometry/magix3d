/**
 * \file	SerializedRepresentation.h
 *
 * \author	Charles PIGNEROL, Franck Ledoux, Eric Brière de l'Isle
 *
 * \date	19/09/2013
 */

#include "Utils/SerializedRepresentation.h"
#include "Utils/Entity.h"
#include "Utils/MgxNumeric.h"

#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/UTF8String.h>

#include <memory>

using namespace Mgx3D::Utils::Math;
using namespace TkUtil;
using namespace std;


namespace Mgx3D
{

namespace Utils
{


// =============================================================================
//                LA CLASSE SerializedRepresentation::Property
// =============================================================================

SerializedRepresentation::Property::Property ( )
	: _parent (0), _name ( ), _value ( ),
	  _type (SerializedRepresentation::Property::STRING),
	  _entity_uid(0)
{
}	// Property::Property


SerializedRepresentation::Property::Property (
									const string& name, const string& value)
	: _parent (0), _name (name), _value (value),
	  _type (SerializedRepresentation::Property::STRING),
      _entity_uid(0)
{
	if (string::npos != name.find (pathSeparator))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Création impossible de la propriété " << name << " : "
		        << "Son nom contient le caractère \'" << pathSeparator
		        << "\' de séparation des noms de sous-rubriques.";
		throw Exception (message);
	}	// if (string::npos != name.find (pathSeparator))
}	// Property::Property


SerializedRepresentation::Property::Property (const string& name, long value)
	: _parent (0), _name (name), _value ( ),
	  _type (SerializedRepresentation::Property::LONG),
      _entity_uid(0)
{
	if (string::npos != name.find (pathSeparator))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Création impossible de la propriété " << name << " : "
		        << "Son nom contient le caractère \'" << pathSeparator
		        << "\' de séparation des noms de sous-rubriques.";
		throw Exception (message);
	}	// if (string::npos != name.find (pathSeparator))
	setValue (NumericConversions::toStr (value));
}	// Property::Property


SerializedRepresentation::Property::Property (
										const string& name, unsigned long value)
	: _parent (0), _name (name), _value ( ),
	  _type (SerializedRepresentation::Property::ULONG),
      _entity_uid(0)
{
	if (string::npos != name.find (pathSeparator))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Création impossible de la propriété " << name << " : "
		        << "Son nom contient le caractère \'" << pathSeparator
		        << "\' de séparation des noms de sous-rubriques.";
		throw Exception (message);
	}	// if (string::npos != name.find (pathSeparator))
	setValue (NumericConversions::toStr (value));
}	// Property::Property


SerializedRepresentation::Property::Property (const string& name, double value)
	: _parent (0), _name (name), _value ( ),
	  _type (SerializedRepresentation::Property::DOUBLE),
      _entity_uid(0)
{
	if (string::npos != name.find (pathSeparator))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Création impossible de la propriété " << name << " : "
		        << "Son nom contient le caractère \'" << pathSeparator
		        << "\' de séparation des noms de sous-rubriques.";
		throw Exception (message);
	}	// if (string::npos != name.find (pathSeparator))
	setValue (MgxNumeric::userRepresentation (value));
}	// Property::Property


SerializedRepresentation::Property::Property (const string& name, bool value)
	: _parent (0), _name (name), _value ( ),
	  _type (SerializedRepresentation::Property::BOOL),
      _entity_uid(0)
{
	if (string::npos != name.find (pathSeparator))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Création impossible de la propriété " << name << " : "
		        << "Son nom contient le caractère \'" << pathSeparator
		        << "\' de séparation des noms de sous-rubriques.";
		throw Exception (message);
	}	// if (string::npos != name.find (pathSeparator))
	_value	= true == value ? "true" : "false";
}	// Property::Property


SerializedRepresentation::Property::Property (
							const string& name, const vector<double>& value)
	: _parent (0), _name (name), _value ( ),
	  _type (SerializedRepresentation::Property::DOUBLES),
      _entity_uid(0)
{
	bool			first	= true;
	UTF8String	str (Charset::UTF_8);
	str << "(";
	for (vector<double>::const_iterator it = value.begin ( );
	     value.end ( ) != it; it++)
	{
		if (false == first)
			str << ", ";
		else
			first	= false;
		str << MgxNumeric::userRepresentation (*it);
	}
	str << ")";
	setValue (str.iso ( ));
}	// Property::Property


SerializedRepresentation::Property::Property (
									const string& name, const Entity& entity)
	: _parent (0), _name (name), _value ( ),
	  _type (SerializedRepresentation::Property::ENTITY),
	  _entity_uid(entity.getUniqueId( ))
{
	//setValue (NumericConversions::toStr (entity.getUniqueId ( )));
    setValue (entity.getTypeName( ));
}	// Property::Property


SerializedRepresentation::Property::Property (
									const SerializedRepresentation::Property& p)
	: _parent (p._parent), _name (p._name), _value (p._value), _type (p._type), _entity_uid(p._entity_uid)
{
}	// Property::Property


SerializedRepresentation::Property&
				SerializedRepresentation::Property::operator = (
									const SerializedRepresentation::Property& p)
{
	if (&p != this)
	{
		_parent			= p._parent;
		_name			= p._name;
		_value			= p._value;
		_type			= p._type;
		_entity_uid     = p._entity_uid;
	}	// if (&p != this)

	return *this;
}	// Property::operator =


SerializedRepresentation::Property::~Property ( )
{
}	// Property::~Property


bool SerializedRepresentation::Property::operator == (
					const SerializedRepresentation::Property& property) const
{
	if ((_name == property._name) && (_type == property._type) &&
	    (_value == property._value) && (_entity_uid == property._entity_uid))
		return true;

	return false;
}	// SerializedRepresentation::Property::operator ==


bool SerializedRepresentation::Property::operator != (
					const SerializedRepresentation::Property& property) const
{
	return !operator == (property);
}	// SerializedRepresentation::Property::operator !=


string SerializedRepresentation::Property::getName ( ) const
{
	return _name;
}	// Property::getName


string SerializedRepresentation::Property::getPath ( ) const
{
	return 0 == _parent ?
	       getName ( ) : _parent->getPath ( ) + pathSeparator + getName ( );
}	// Property::getPath


string SerializedRepresentation::Property::getValue ( ) const
{
	return _value;
}	// Property::getValue


SerializedRepresentation::Property::TYPE
						SerializedRepresentation::Property::getType ( ) const
{
	return _type;
}	// Property::getType


long SerializedRepresentation::Property::getValueAsLong ( ) const
{
	try
	{
		return NumericConversions::strToLong (getValue ( ));
	}
	catch (const Exception& e)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Impossible de convertir la propriété " << getPath ( )
		        << " de valeur " << getValue ( ) << " en long : "
		        << e.getFullMessage ( ) << ".";
		throw Exception (message);
	}
	catch (...)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Impossible de convertir la propriété " << getPath ( )
		        << " de valeur " << getValue ( ) << " en long.";
		throw Exception (message);
	}
}	// Property::getValueAsLong


unsigned long
			SerializedRepresentation::Property::getValueAsUnsignedLong ( ) const
{
	try
	{
		return NumericConversions::strToULong (getValue ( ));
	}
	catch (const Exception& e)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Impossible de convertir la propriété " << getPath ( )
		        << " de valeur " << getValue ( ) << " en unsigned long : "
		        << e.getFullMessage ( ) << ".";
		throw Exception (message);
	}
	catch (...)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Impossible de convertir la propriété " << getPath ( )
		        << " de valeur " << getValue ( ) << " en unsigned long.";
		throw Exception (message);
	}
}	// Property::getValueAsUnsignedLong


double SerializedRepresentation::Property::getValueAsDouble ( ) const
{
	try
	{
		return NumericConversions::strToDouble (getValue ( ));
	}
	catch (const Exception& e)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Impossible de convertir la propriété " << getPath ( )
		        << " de valeur " << getValue ( ) << " en double : "
		        << e.getFullMessage ( ) << ".";
		throw Exception (message);
	}
	catch (...)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Impossible de convertir la propriété " << getPath ( )
		        << " de valeur " << getValue ( ) << " en double.";
		throw Exception (message);
	}
}	// Property::getValueAsDouble


bool SerializedRepresentation::Property::getValueAsBool ( ) const
{
	try
	{
		if (getValue ( ) == "true")
			return true;
		else if (getValue ( ) == "false")
			return false;
		else
		{
			UTF8String	message (Charset::UTF_8);
			message << "\"" << getValue ( )
			        << "\" n'est pas une valeur bouléenne.";
			throw Exception (message);
		}
	}
	catch (const Exception& e)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Impossible de convertir la propriété " << getPath ( )
		        << " de valeur " << getValue ( ) << " en bool : "
		        << e.getFullMessage ( ) << ".";
		throw Exception (message);
	}
	catch (...)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Impossible de convertir la propriété " << getPath ( )
		        << " de valeur " << getValue ( ) << " en bool.";
		throw Exception (message);
	}
}	// Property::getValueAsBool


vector<double>
		SerializedRepresentation::Property::getValueAsDoubleVector ( ) const
{
	vector<double>		vect;

	// Elimination éventuels espaces/tabs en début et fin de chaine :
	const UTF8String	trimed	= UTF8String (getValue ( )).trim ( );
	if ((2 > trimed.length ( )) ||
	    ('(' != *trimed [0]) || (')' != *trimed [trimed.length ( ) - 1]))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Impossible de convertir la propriété " << getPath ( )
		        << " de valeur " << getValue ( ) << " en vecteur de double : "
		        << "syntaxe invalide.";
		throw Exception (message);
	}

	string	values	= trimed.substring (1, trimed.length ( ) - 2).iso ( );
	while (0 != values.length ( ))
	{
		string::size_type	pos	= values.find (',');
		if (string::npos != pos)
		{
			const string	value	= values.substr (0, pos);
			values					= values.substr (pos + 1);
			vect.push_back (NumericConversions::strToDouble (value));
		}	// if (string::npos != pos)
		else
		{
			vect.push_back (NumericConversions::strToDouble (values));
			values	= "";
		}
	}	// while (0 != values.length ( ))

	return vect;
}	// Property::getValueAsDoubleVector


void SerializedRepresentation::Property::resetValue ( )
{
	_value.clear ( );
}	// Property::resetValue


void SerializedRepresentation::Property::setValue (const string& value)
{
	_value	= UTF8String (value,  (Charset::UTF_8)).trim (true);
}	// Property::setValue


void SerializedRepresentation::Property::setParent (
											SerializedRepresentation* parent)
{
/*	if ((0 != _parent) && (parent != _parent))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Changement de parent de la propriété " << getPath ( )
		        << " impossible : opération interdite.";
		throw Exception (message);
	}	// if ((0 != _parent) && (parent != _parent))
*/
	_parent	= parent;
}	// Property::setParent


unsigned long SerializedRepresentation::Property::getEntityUniqueId ( ) const
{
    return _entity_uid;
}   // Property::getEntityUniqueId

// =============================================================================
//                      LA CLASSE SerializedRepresentation
// =============================================================================


const char	SerializedRepresentation::pathSeparator ('.');


SerializedRepresentation::SerializedRepresentation ( )
	: _parent (0), _name ( ), _summary ( ), _properties ( ), _propertiesSets ( )
{
}	// SerializedRepresentation::SerializedRepresentation


SerializedRepresentation::SerializedRepresentation (
									const string& name, const string& summary)
	: _parent (0), _name (name), _summary (summary),
	  _properties ( ), _propertiesSets ( )
{
	if (string::npos != name.find (pathSeparator))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Création impossible de la rubrique " << name << " : "
		        << "Son nom contient le caractère \'" << pathSeparator
		        << "\' de séparation des noms de sous-rubriques.";
		throw Exception (message);
	}	// if (string::npos != name.find (pathSeparator))
}	// SerializedRepresentation::SerializedRepresentation


SerializedRepresentation::SerializedRepresentation (
											const SerializedRepresentation& sr)
	: _parent (0), _name (sr._name), _summary (sr._summary),
	  _properties (sr._properties), _propertiesSets ( )
{
	for (vector<SerializedRepresentation*>::const_iterator it =
			sr._propertiesSets.begin( ); sr._propertiesSets.end( ) != it; it++)
	{
		SerializedRepresentation*	s	= new SerializedRepresentation (**it);
		s->setParent (this);
		_propertiesSets.push_back (s);
	}	// for (vector<SerializedRepresentation*>::const_iterator it = ...
}	// SerializedRepresentation::SerializedRepresentation


SerializedRepresentation& SerializedRepresentation::operator = (
											const SerializedRepresentation& sr)
{
	if (&sr != this)
	{
		clear ( );
		_parent			= 0;
		_name			= sr._name;
		_summary		= sr._summary;
		_properties		= sr._properties;
		for (vector<SerializedRepresentation*>::const_iterator it =
			sr._propertiesSets.begin( ); sr._propertiesSets.end( ) != it; it++)
		{
			SerializedRepresentation* s	= new SerializedRepresentation (**it);
			s->setParent (this);
			_propertiesSets.push_back (s);
		}	// for (vector<SerializedRepresentation*>::const_iterator it = ...
	}	// if (&sr != this)

	return *this;
}	// SerializedRepresentation::operator =


SerializedRepresentation::~SerializedRepresentation ( )
{
	clear ( );
}	// SerializedRepresentation::~SerializedRepresentation


string SerializedRepresentation::getName ( ) const
{
	return _name;
}	// SerializedRepresentation::getName


string SerializedRepresentation::getPath ( ) const
{
	return 0 == _parent ?
	       getName ( ) : _parent->getPath ( ) + pathSeparator + getName ( );
}	// SerializedRepresentation::getPath


string SerializedRepresentation::toString ( ) const
{
	UTF8String	description (Charset::UTF_8);
	description << "SerializedRepresentation " << getName ( )
	            << "\n"
	            << "Path                             : " << getPath ( )
	            << "\n"
	            << "Properties                       : ";
	const vector<SerializedRepresentation::Property>&	properties	=
															getProperties ( );
	for (vector<SerializedRepresentation::Property>::const_iterator itp =
			properties.begin ( ); properties.end ( ) != itp; itp++)
	{
		description << "\n"
				<< "\t" << (*itp).getName( ) <<"\t\t\t: "<< (*itp).getValue( );
	}
	description << "\n"
	            << "Properties sets                  : ";
	const vector<SerializedRepresentation*>&	propertiesSets	=
														getPropertiesSets ( );
	for (vector<SerializedRepresentation*>::const_iterator itps =
			propertiesSets.begin ( ); propertiesSets.end ( ) != itps; itps++)
	{
		description << "\n"
				<< "\t" << (*itps)->toString( );
	}

	return description.iso ( );
}	// SerializedRepresentation::toString


string SerializedRepresentation::getSummary ( ) const
{
	return _summary;
}	// SerializedRepresentation::getSummary


void SerializedRepresentation::setSummary (const string& summary)
{
	_summary	= summary;
}	// SerializedRepresentation::setSummary


bool SerializedRepresentation::isEmpty ( ) const
{
	return _properties.empty ( ) && _propertiesSets.empty ( );
}	// SerializedRepresentation::isEmpty


void SerializedRepresentation::addProperty (
							const SerializedRepresentation::Property& property)
{
// CP : une propriété peut être présente à de multiples fois, et peut être est
// il préférable de le laisser apparaître.
// Ex : quad dégénéré
/*	for (vector<SerializedRepresentation::Property>::const_iterator
			it = _properties.begin ( ); _properties.end ( ) != it; it++)
		if ((*it).getName ( ) == property.getName ( ))
		{
			UTF8String	message (Charset::UTF_8);
			message << "Ajout de la propriété " << property.getName ( )
			        << " à la rubrique " << getPath ( ) << " impossible : "
			        << "propriété déjà présente.";
			throw Exception (message);
		}	// if ((*it).getName ( ) == property.getName ( ))	*/

	Property	p (property);
	p.setParent (this);
	_properties.push_back (p);
}	// SerializedRepresentation::addProperty


SerializedRepresentation::Property SerializedRepresentation::getProperty (
													const string& path) const
{
	try
	{

	string::size_type	pos	= path.find (pathSeparator);
	if (string::npos == pos)
	{
		for (vector<SerializedRepresentation::Property>::const_iterator
			it = _properties.begin ( ); _properties.end ( ) != it; it++)
			if (path == (*it).getName ( ))
				return *it;

		throw Exception ("Path invalide.");
	}	// if (string::npos == pos)
	else	// if (string::npos == pos)
	{
		const string	parent	= path.substr (0, pos);
		const string	prop	= path.substr (pos + 1);
		for (vector<SerializedRepresentation*>::const_iterator
			it = _propertiesSets.begin ( ); _propertiesSets.end ( ) != it; it++)
			if (parent == (*it)->getName ( ))
				return (*it)->getProperty (prop);

		throw Exception ("Path invalide.");
	}	// else if (string::npos == pos)

	}
	catch (...)
	{
		UTF8String	message (Charset::UTF_8);
		message << "La rubrique " << getName ( ) << " (" << getPath ( )
		        << ") ne contient pas de propriété " << path << ".";
		throw Exception (message);
	}
}	// SerializedRepresentation::getProperty


const vector<SerializedRepresentation::Property>&
							SerializedRepresentation::getProperties ( ) const
{
	return _properties;
}	// SerializedRepresentation::getProperties


void SerializedRepresentation::addPropertiesSet (
							const SerializedRepresentation& propertySet)
{
	for (vector<SerializedRepresentation*>::const_iterator
			it = _propertiesSets.begin ( ); _propertiesSets.end ( ) != it; it++)
		if ((*it)->getName ( ) == propertySet.getName ( ))
		{
			UTF8String	message (Charset::UTF_8);
			message << "Ajout de la rubrique " << propertySet.getName ( )
			        << " à la rubrique " << getPath ( ) << " impossible : "
			        << "rubrique déjà présente.";
			throw Exception (message);
		}	// if ((*it)->getName ( ) == propertySets.getName ( ))

	SerializedRepresentation*	ps	= new SerializedRepresentation(propertySet);
	ps->setParent (this);
	_propertiesSets.push_back (ps);
}	// SerializedRepresentation::addPropertiesSet


const SerializedRepresentation& SerializedRepresentation::getPropertiesSet (
													const string& path) const
{
	try
	{

	string::size_type	pos	= path.find (pathSeparator);
	if (string::npos == pos)
	{
		for (vector<SerializedRepresentation*>::const_iterator
			it = _propertiesSets.begin ( ); _propertiesSets.end ( ) != it; it++)
			if (path == (*it)->getName ( ))
				return **it;

		throw Exception ("Path invalide.");
	}	// if (string::npos == pos)
	else	// if (string::npos == pos)
	{
		const string	parent	= path.substr (0, pos);
		const string	prop	= path.substr (pos + 1);
		for (vector<SerializedRepresentation*>::const_iterator
			it = _propertiesSets.begin ( ); _propertiesSets.end ( ) != it; it++)
			if (parent == (*it)->getName ( ))
				return (*it)->getPropertiesSet (prop);

		throw Exception ("Path invalide.");
	}	// else if (string::npos == pos)

	}
	catch (...)
	{
		UTF8String	message (Charset::UTF_8);
		message << "La rubrique " << getName ( ) << " (" << getPath ( )
		        << ") ne contient pas de sous-rubrique " << path << ".";
		throw Exception (message);
	}
}	// SerializedRepresentation::getPropertiesSet


const vector<SerializedRepresentation*>&
						SerializedRepresentation::getPropertiesSets ( ) const
{
	return _propertiesSets;
}	// SerializedRepresentation::getPropertiesSets


void SerializedRepresentation::clear ( )
{
	for (vector<SerializedRepresentation*>::iterator itps =
			_propertiesSets.begin ( ); _propertiesSets.end ( ) != itps; itps++)
		delete *itps;
	_propertiesSets.clear ( );
	_properties.clear ( );
	_summary.clear ( );
	_name.clear ( );
	_parent	= 0;
}	// SerializedRepresentation::clear


void SerializedRepresentation::setParent (SerializedRepresentation* parent)
{
	if ((0 != _parent) && (parent != _parent))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Changement de parent de la rubrique " << getPath ( )
		        << " impossible : opération interdite.";
		throw Exception (message);
	}	// if ((0 != _parent) && (parent != _parent))

	_parent	= parent;
}	// SerializedRepresentation::setParent



SerializedRepresentation* SerializedRepresentation::merge (
				const string& name,
				const vector<const SerializedRepresentation*>& representations,
				bool withEmptySets)
{
	SerializedRepresentation*	commonRepresentation	=
										new SerializedRepresentation (name, "");
	if (0 == representations.size ( ))
		return commonRepresentation;

	vector<const SerializedRepresentation*>::const_iterator	it	=
													representations.begin ( );
	const SerializedRepresentation*	reference	= *it;
	CHECK_NULL_PTR_ERROR (*it)
	++it;
	vector<const SerializedRepresentation*>::const_iterator	it1	= it, it2 = it;
	const vector<SerializedRepresentation::Property>&	properties		=
											reference->getProperties ( );
	const vector<SerializedRepresentation*>&			propertiesSets	=
											reference->getPropertiesSets ( );
	for (vector<SerializedRepresentation*>::const_iterator itps =
			propertiesSets.begin ( ); propertiesSets.end ( ) != itps; itps++)
	{
		bool									havePropsSet	= true;
		vector<const SerializedRepresentation*>	propsSets;
		propsSets.push_back (*itps);
		for (it2 = it; it2 != representations.end ( ); it2++)
		{
			const SerializedRepresentation*	ps	= 0;
			try
			{
				ps	= &(*it2)->getPropertiesSet ((*itps)->getName ( ));
			}
			catch (...)
			{
				havePropsSet	= false;
				break;
			}

			propsSets.push_back (ps);
		}	// for (it2 = it; it2 != representations.end ( ); it2++)

		if (true == havePropsSet)	// Partagé => on l'ajoute
		{
			unique_ptr<SerializedRepresentation>	ps (
					SerializedRepresentation::merge (
							(*itps)->getName ( ), propsSets, withEmptySets));
			if ((false == ps->isEmpty ( )) || (true == withEmptySets))
				commonRepresentation->addPropertiesSet (*ps.get ( ));
		}	// if (true == havePropsSet)
	}	// for (vector<SerializedRepresentation*>::const_iterator itps ...

	for (vector<SerializedRepresentation::Property>::const_iterator itp =
			properties.begin ( ); properties.end ( ) != itp; itp++)
	{
		bool	haveProp	= true;
		bool	sameValue	= true;
		for (it1 = it; it1 != representations.end ( ); it1++)
		{
			SerializedRepresentation::Property	p;
			try
			{
				p	= (*it1)->getProperty ((*itp).getName ( ));
			}
			catch (...)
			{
				haveProp	= false;
				break;
			}

			if ((true == sameValue) && (p != (*itp)))
				sameValue	= false;
		}	// for (it1 = it; it1 != representations.end ( ); it1++)

		if (true == haveProp)	// Partagée => on l'ajoute
		{
			SerializedRepresentation::Property	prop	 (*itp);
			if (false == sameValue)
				prop.resetValue ( );
			commonRepresentation->addProperty (prop);
		}	// if (true == haveProp)
	}	// for (vector<SerializedRepresentation::Property>::const_iterator itp 

	return commonRepresentation;
}	// SerializedRepresentation::merge


}	// namespace Utils

}	// namespace Mgx3D
