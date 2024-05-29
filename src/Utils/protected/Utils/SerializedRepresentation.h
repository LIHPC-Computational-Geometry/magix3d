/**
 * \file	SerializedRepresentation.h
 *
 * \author	Charles PIGNEROL, Franck Ledoux, Eric Brière de l'Isle
 *
 * \date	19/09/2013
 */

#ifndef SERIALIZED_REPRESENTATION_H
#define SERIALIZED_REPRESENTATION_H

#include <string>
#include <vector>


namespace Mgx3D
{

namespace Utils
{

	class Entity;

/*
 * <P>Classe permettant de représenter un objet par des chaînes de caractères et
 * une arborescence de rubriques.
 * </P>
 *
 * <P>Chaque niveau de rubrique à un nom, un ensemble de propriétés, et
 * des sous-niveaux.
 * </P>
 */
class SerializedRepresentation
{
	public :

	/**
 	 * Représente une propriété d'une classe d'obets.
 	 */
	class Property
	{
		friend class SerializedRepresentation;	// beurk


		public :

		/**
		 * Les différents types de propriétés.s
		 */
		enum TYPE { STRING, LONG, ULONG, DOUBLE, BOOL, DOUBLES, ENTITY };

		/**
		 * Constructeur par défaut.
		 * \warning		<B>NE PAS UTILISER, POUR STD::VECTOR UNIQUEMENT.</B>
		 */
		Property ( );

		/**
		 * \param		Nom de la propriété
		 * \param		Valeur de la propriété
		 */
		Property (const std::string& name, const std::string& value);
		Property (const std::string& name, long value);
		Property (const std::string& name, unsigned long value);
		Property (const std::string& name, double value);
		Property (const std::string& name, bool value);
		Property (const std::string& name, const std::vector<double>& value);
		Property (const std::string& name, const Mgx3D::Utils::Entity& entity);

		/**
 		 * Constructeur de copie. RAS.
 		 */
		Property (const Property&);

		/**
 		 * Opérateur =. RAS.
 		 */
		Property& operator = (const Property&);

		/**
		 * Destructeur. RAS.
		 */
		virtual ~Property ( );

		/**
		 * Opérateurs d'égalité : comparaison de tous les champs exception
		 * faite du parent.
		 */
		virtual bool operator == (
							const SerializedRepresentation::Property&) const;
		virtual bool operator != (
							const SerializedRepresentation::Property&) const;

		/**
		 * \return		Le nom de la propriété.
		 * \see			getPath
		 * \see			getType
		 */
		virtual std::string getName ( ) const;

		/**
		 * \return		Le type de la propriété.
		 * \see			getName
		 */
		virtual TYPE getType ( ) const;

		/**
		 * \return		Le nom de la propriété, incluant ses ascendants.
		 * \see			getName
		 */
		virtual std::string getPath ( ) const;

		/**
		 * \return		La valeur de la propriété.
		 * \exception	Une exception est levée si la propriété n'est pas
		 *				convertible dans le type demandé.
		 */
		virtual std::string getValue ( ) const;
		virtual long getValueAsLong ( ) const;
		virtual unsigned long getValueAsUnsignedLong ( ) const;
		virtual double getValueAsDouble ( ) const;
		virtual bool getValueAsBool ( ) const;
		virtual std::vector<double> getValueAsDoubleVector ( ) const;

		/**
		 * Annule la valeur (=> chaîne vide).
		 */
		virtual void resetValue ( );

		/**
		 * \return      L'unique id de l'entité.
		 * \see         _entity_uid
		 */
		virtual unsigned long getEntityUniqueId ( ) const;


		protected :

		/**
		 * \param		La valeur, sous forme de chaine de caractères.
		 * \warning		Enlève les éventuels espaces et tabulations en début et
		 *				fin de chaine.
		 */
		virtual void setValue (const std::string& value);


		private :

		/**
		 * \param		Parent de la propriété.
		 */
		void setParent (SerializedRepresentation* parent);

		/** Le parent de la propriété. */
		SerializedRepresentation*	_parent;

		/** Le nom de la propriété. */
		std::string					_name;

		/** Le type de la propriété. */
		TYPE						_type;

		/** La valeur de la propriété. */
		std::string					_value;

		/** L'unique id de l'entité. */
		unsigned long               _entity_uid;

	};	// class Property


	/**
	 * Constructeur par défaut.
	 * \warning		<B>NE PAS UTILISER, POUR VECTOR UNIQUEMENT.
	 */
	SerializedRepresentation ( );

	/**
	 * \param		Nom de la rubrique.
	 * \param		Eventuel résumé de la rubrique.
	 */
	SerializedRepresentation (
						const std::string& name, const std::string& summary);

	/**
	 * Constructeur de copie. RAS.
	 */
	SerializedRepresentation (const SerializedRepresentation&);

	/**
	 * Opérateur =. RAS.
	 */
	SerializedRepresentation& operator = (const SerializedRepresentation&);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~SerializedRepresentation ( );


	/**
	 * \return		Le nom de la rubrique.
	 * \see			getPath
	 */
	virtual std::string getName ( ) const;

	/**
	 * \return		Le nom de la propriété, incluant ses ascendants.
	 * \see			getName
	 */
	virtual std::string getPath ( ) const;

	/**
	 * \return		Une chaine de caractères listant les caractéristiques de
	 *				l'instance.
	 * \see			getSummary
	 */
	virtual std::string toString ( ) const;

	/**
	 * \return		Une chaine de caractères synthétisant les caractéristiques 
	 *				de l'instance (1 ligne, quelques mots).
	 * \see			getString
	 * \see			setSummary
	 */
	virtual std::string getSummary ( ) const;

	/**
	 * \param		Une chaine de caractères synthétisant les caractéristiques
	 * 				de l'instance (1 ligne, quelques mots).
	 * \see			getSummary
	 */
	virtual void setSummary (const std::string& summary);

	/**
	 * \return		<I>true</I> si la rubrique n'a ni propriété ni
	 *				sous-rubrique, <I>false</I> dans le cas contraire.
	 */
	virtual bool isEmpty ( ) const;

	/**
	 * Le séparateur ('.') dans les noms complets (path) de
	 * rubriques/propriétés.
	 * \see		getPath
	 */
	static const char	pathSeparator;

	/**
	 * Les propriétés de la rubrique.
	 */
	//@{

	/**
	 * Ajoute la propriétée à la liste des propriétés de la rubrique.
	 */
	virtual void addProperty (const SerializedRepresentation::Property&);

	/**
	 * \return		La propriété dont le nom est transmis en argument.
	 * \exception	Une exception est levée si la propriété n'est pas trouvée.
	 */
	virtual SerializedRepresentation::Property getProperty (
												const std::string& path) const;

	/**
	 * \return		Les propriétés de la rubrique.
	 */
	const std::vector<SerializedRepresentation::Property>&
														getProperties ( ) const;

	//@}	//  Les propriétés de la rubrique.


	/**
	 * Les sous-rubriques.
	 */
	//@{

	/**
	 * Ajoute la sous-rubrique à la liste des sous-rubriques de la rubrique.
	 */
	virtual void addPropertiesSet (const SerializedRepresentation&);

	/**
	 * \return		La sous-rubrique dont le nom est transmis en argument.
	 * \exception	Une exception est levée si la sous-rubrique n'est pas
	 * 				trouvée.
	 */
	virtual const SerializedRepresentation& getPropertiesSet (
												const std::string& path) const;

	/**
	 * \return		Les sous-rubriques de la rubrique.
	 */
	const std::vector<SerializedRepresentation*>& getPropertiesSets ( ) const;

	//@}	// Les sous-rubriques.

	/**
	 * \param		Nom de la propriété racine
	 * \param		Liste des représentations dont on va retourner une
	 * 				représentation commune
	 * \param		<I>true</I> si les rubriques communes doivent être incluses
	 *				si elles sont vides, <I>false</I> dans le cas contraire.
	 * \return		Une représentation issue d'une liste de représentations ne
	 *				contenant que les propriétés et rubriques communes. Les
	 *				valeurs des propriétés sont renseignées lorsqu'elles sont
	 *				identiques, et non renseignées si au moins une diffère.
	 * 				Les propriétés dont les valeurs diffèrent reçoivent dans
	 * 				cette liste des valeurs vides.
	 */
	static SerializedRepresentation* merge (
					const std::string& name,
					const std::vector<const SerializedRepresentation*>& sets,
					bool withEmptySets = false);


	protected :

	/**
	 * Libère la mémoire utilisée.
	 */
	virtual void clear ( );


	private :

	/**
	 * \param		Parent de la rubrique.
	 */
	void setParent (SerializedRepresentation* parent);

	/** L'éventuel parent de la rubrique. */
	SerializedRepresentation*							_parent;

	/** Le nom de la rubrique. */
	std::string											_name;

	/** Un éventuel bref résumé de la rubrique. */
	std::string											_summary;

	/** Les propriétés. */
	std::vector<SerializedRepresentation::Property>		_properties;

	/** Les sous-rubriques. */
	std::vector<SerializedRepresentation*>				_propertiesSets;
};	// class SerializedRepresentation

}	// namespace Utils

}	// namespace Mgx3D

#endif	// SERIALIZED_REPRESENTATION_H
