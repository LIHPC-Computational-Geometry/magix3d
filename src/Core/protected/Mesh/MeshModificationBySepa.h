/*----------------------------------------------------------------------------*/
/*
 * \file MeshModificationBySepa.h
 *
 *  \author legoff
 *
 *  \date 8 jan. 2015
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHMODIFICATIONBYSEPA_H_
#define MESHMODIFICATIONBYSEPA_H_
/*----------------------------------------------------------------------------*/
#include <GMDS/Utils/CommonTypes.h>
#include <GMDS/IG/IG.h>
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Mesh/MeshModificationItf.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace gmds{
class Node;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/**
 * \class MeshModificationBySepa
 *
 * Objet qui va modifier
 *
 */
class MeshModificationBySepa : public MeshModificationItf {
public:

	MeshModificationBySepa();

	MeshModificationBySepa(Utils::Math::Point& ACenter, std::vector<std::string>& AFilenames);

	/** \brief Fixe le centre du repère de la séparatrice
	 */
	virtual void setCenter(const Utils::Math::Point ACenter);

	/** \brief Ajout d'un fichier de contrôle
	 */
	virtual void addControlFile(const std::string& AFilename);

	/** \brief Écrit un fichier associant noeuds avec delta
	 */
	//virtual void writeDeltaVTKFile(const std::string& AFilename);

	/** \brief Écrit un fichier de contrôle
	 */
	virtual void writeControlVTKFile(const std::string& AFilename);

#ifndef SWIG

	virtual ~MeshModificationBySepa();

	/** Création d'un clone, on copie toutes les informations */
	MeshModificationBySepa* clone() const;

	/** Fonction d'appel pour modifier un ensemble de noeuds du maillage
	 *  Ne sont modifiés que ceux dont la valeur est différente de la marque dans le filtre
	 */
	virtual void applyModification(std::vector<gmds::Node >& AGmdsNodes, std::vector<gmds::Face>& AGmdsFaces, Internal::Context& AContext);

	/** \brief  Fournit une représentation textuelle de l'entité.
	 * \return	Description, à détruire par l'appelant.
	 */
	virtual void addToDescription (Mgx3D::Utils::SerializedRepresentation* description) const;

	std::map<gmds::Node,double>& getDeltas();

	/** Script pour la commande de création Python */
	virtual TkUtil::UTF8String getScriptCommand() const;

protected:

	MeshModificationBySepa(const MeshModificationBySepa& ASepa);

//	MeshModificationBySepa(const MeshModificationBySepa&)
//    :MeshModificationItf()
//    {
//        MGX_FORBIDDEN("MeshModificationBySepa::MeshModificationBySepa is not allowed.");
//    }

	MeshModificationBySepa& operator = (const MeshModificationBySepa&)
    {
        MGX_FORBIDDEN("MeshModificationBySepa::operator = is not allowed.");
        return *this;
    }

private:

	// nom des fichiers de contrôle
	std::vector<std::string> m_sepafiles;

	// centre de la séparatrice
	Utils::Math::Point m_center;

	// deltas associated to nodes
	std::map<gmds::Node,double> m_deltas;

	// faces of the separatrice, used only for vtk outputs
	std::vector<gmds::Face> m_faces;

#endif // SWIG
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESHMODIFICATIONBYSEPA_H_ */
