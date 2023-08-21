/*----------------------------------------------------------------------------*/
/*
 * \file VolumicSmoothing.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 4/3/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHVOLUMICSMOOTHING_H_
#define MESHVOLUMICSMOOTHING_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshModificationItf.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/UTF8String.h>
#include <GMDS/Utils/CommonTypes.h>
/*----------------------------------------------------------------------------*/
namespace gmds{
class Node;
class Region;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class Volume;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/**
 * \class VolumicSmoothing
 *
 * Objet qui va modifier un maillage suivant un lissage volumique
 *
 */
class VolumicSmoothing : public MeshModificationItf {
public:

	/// énuméré sur les différentes méthodes de lissage volumique
	enum eVolumicMethod {
		laplace = 0,
		tipton,
		jun,
		conditionNumber,
		inverseMeanRatio};

	/// Constructeur pour fonction de lissage volumique avec les arguments par défaut
	VolumicSmoothing();

	/// Constructeur avec paramètres différents du défaut
	VolumicSmoothing(int nbIterations, eVolumicMethod methodeLissage);

	/// change le nombre d'itérations
	void setNbIterations(int nbIterations);

	/// change la méthode de lissage
	void setMethod(eVolumicMethod methodeLissage);

	/// accesseur sur le nombre d'itérations
	int getNbIterations() const {return m_nbIterations;}

	/// accesseur sur la méthode de lissage
	eVolumicMethod getMethod() const {return m_methodeLissage;}

#ifndef SWIG

	/// converti en chaine de caractères une méthode de lissage
	static std::string toString(eVolumicMethod method);

    /** Création d'un clone, on copie toutes les informations */
    virtual VolumicSmoothing* clone() const;

    /// Destructeur
	virtual ~VolumicSmoothing();

	/** Fonction d'appel pour modifier un ensemble de noeuds du maillage
	 *  Ne sont modifiés que ceux dont la valeur est différente de la marque dans le filtre
	 */
	virtual void applyModification(std::vector<gmds::Node >& gmdsNodes,
			std::vector<gmds::Region>& gmdsPolyedres,
			std::map<gmds::TCellID, uint>& filtre_nodes,
			uint maskFixed,
			Geom::Volume* volume);

	/** \brief  Fournit une représentation textuelle de l'entité.
	 * \return	Description, à détruire par l'appelant.
	 */
	virtual void addToDescription (Mgx3D::Utils::SerializedRepresentation* description) const;

	 /// Script pour la commande de création Python
	virtual TkUtil::UTF8String getScriptCommand() const;

protected:
	VolumicSmoothing(const VolumicSmoothing&);

	VolumicSmoothing& operator = (const VolumicSmoothing&);

private:
	/// mémorise le fait que l'utilisateur ait changé un paramètre, pour getScriptCommand
	bool m_useDefaults;

	/// nombre d'itérations de l'algo
	int m_nbIterations;

	/// la méthode de lissage du volume
	eVolumicMethod m_methodeLissage;

#endif
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESHVOLUMICSMOOTHING_H_ */
