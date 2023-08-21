/*----------------------------------------------------------------------------*/
/*
 * \file SurfacicSmoothing.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24/2/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHSURFACICSMOOTHING_H_
#define MESHSURFACICSMOOTHING_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshModificationItf.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/UTF8String.h>
#include <GMDS/Utils/CommonTypes.h>
/*----------------------------------------------------------------------------*/
namespace gmds{
class Node;
class Face;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class Surface;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/**
 * \class SurfacicSmoothing
 *
 * Objet qui va modifier un maillage suivant un lissage surfacique
 *
 */
class SurfacicSmoothing : public MeshModificationItf {
public:

	/// énuméré sur les différentes méthodes de lissage surfacique
	enum eSurfacicMethod { surfacicNormalSmoothing = 0, // optimisation suivant la normale
					 surfacicOrthogonalSmoothing, // optimisation suivant l'orthogonalité des bras des mailles
					 surfacicOrthogonalSmoothingElliptic};

	/// énuméré sur les différents solvers
	enum eSolver {gradientConjugue = 0, newton};

	/// Constructeur pour fonction de lissage surfacique avec les arguments par défaut
	SurfacicSmoothing();

	/// Constructeur avec paramètres différents du défaut
	SurfacicSmoothing(int nbIterations, eSurfacicMethod methodeLissage, eSolver solver);

	/// change le nombre d'itérations
	void setNbIterations(int nbIterations);

	/// change la méthode de lissage
	void setMethod(eSurfacicMethod methodeLissage);

	/// change le type de solver utilisé par Mesquite
	void setSolver(eSolver solver);

	/// accesseur sur le nombre d'itérations
	int getNbIterations() const {return m_nbIterations;}

	/// accesseur sur la méthode de lissage
	eSurfacicMethod getMethod() const {return m_methodeLissage;}

	/// accesseur sur le solver
	eSolver getSolver() const {return m_solver;}

#ifndef SWIG

	/// converti en chaine de caractères une méthode de lissage
	static std::string toString(eSurfacicMethod method);

	/// converti en chaine de caractères le nom d'un solver
	static std::string toString(eSolver solver);

	/** Création d'un clone, on copie toutes les informations */
    virtual SurfacicSmoothing* clone() const;

    /// Destructeur
	virtual ~SurfacicSmoothing();

	/** Fonction d'appel pour modifier un ensemble de noeuds du maillage
	 *  Ne sont modifiés que ceux dont la valeur est différente de la marque dans le filtre
	 */
	virtual void applyModification(std::vector<gmds::Node >& gmdsNodes,
			std::vector<gmds::Face>& gmdsPolygones,
			std::map<gmds::TCellID, uint>& filtre_nodes,
			std::map<gmds::TCellID, bool>& isPolyInverted,
			uint maskFixed,
			Geom::Surface* surface);

	/** \brief  Fournit une représentation textuelle de l'entité.
	 * \return	Description, à détruire par l'appelant.
	 */
	virtual void addToDescription (Mgx3D::Utils::SerializedRepresentation* description) const;

	 /// Script pour la commande de création Python
	virtual TkUtil::UTF8String getScriptCommand() const;

protected:
	SurfacicSmoothing(const SurfacicSmoothing&);

	SurfacicSmoothing& operator = (const SurfacicSmoothing&);

private:
	/// mémorise le fait que l'utilisateur ait changé un paramètre, pour getScriptCommand
	bool m_useDefaults;

	/// nombre d'itérations de l'algo Mesquite ou autre
	int m_nbIterations;

	/// la méthode de lissage de la surface
	eSurfacicMethod m_methodeLissage;

	/// le solver pour les algos de Mesquite
	eSolver m_solver;

#endif
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESHSURFACICSMOOTHING_H_ */
