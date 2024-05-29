/*----------------------------------------------------------------------------*/
/*
 * \file CommandMeshExplorer.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7 déc. 2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDMESHEXPLORER_H_
#define COMMANDMESHEXPLORER_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Mesh/SubVolume.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class Group3D;
}
namespace Topo{
class CoEdge;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class CommandMeshExplorer
 *  \brief Commande de création de sous-volumes pour observer l'intérieur de volumes
 */
/*----------------------------------------------------------------------------*/
class CommandMeshExplorer: public Internal::CommandInternal {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur d'un ensemble de sous-volumes
     *
     *  \param c le contexte
     *  \param oldExplo l'explorateur précédent s'il y en avait un
     *  \param inc le décalage en nombre de bras
     *  \param coedge l'arête par laquelle passe l'exploration
     */
    CommandMeshExplorer(Internal::Context& c, CommandMeshExplorer* oldExplo, int inc, Topo::CoEdge* arete);


    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour terminer l'exploration
     *
     *  \param c le contexte
     *  \param oldExplo l'explorateur précédent s'il y en avait un
     */
    CommandMeshExplorer(Internal::Context& c, CommandMeshExplorer* oldExplo);

	/*------------------------------------------------------------------------*/
	/** \brief	Destructeur. RAS.
	 */
	virtual ~CommandMeshExplorer();


    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /** \brief  rejoue la commande
     */
    void internalRedo();

protected:
    /// bloc, direction et position de la coupe
    struct BlockDirPos {
        Topo::Block* m_bloc;
        Topo::Block::eDirOnBlock m_dir;
        uint m_pos;
    };

    /// supprime les sous-volumes de la commande précédente
    void deleteOldSubVolumes();

    /// parcours les volumes, marque les blocs et les faces acceptés
    void selectCoFaceAndBlocks(std::map<Topo::CoFace*, uint>& filtre_coface,
            std::map<Topo::Block*, uint>& filtre_block);

    /// on ne prend qu'une fois une arête, on les marque suivant la position de la coupe
    void computePosCoEdge(std::map<Topo::CoFace*, uint>& filtre_coface,
            std::map<Topo::CoEdge*, uint> &filtre_coedge);

    void computePosBlock(std::map<Topo::Block*, uint>& filtre_block,
            std::map<Topo::CoEdge*, uint> &filtre_coedge,
            std::vector<BlockDirPos>& bloc_dirPos);

    /// création des sous-volumes suivant la position demandée dans les blocs
    void createSubVolume(std::vector<BlockDirPos>& bloc_dirPos);


private:
    /// Id pour l'explorateur
    unsigned long m_explorer_id;

    /// lien sur l'explorateur précédent s'il y a lieu
    CommandMeshExplorer* m_old_explorer;

    /// position de l'explorateur sur l'arête (en nombre de bras)
    int m_pos;

    /// l'arête par laquelle passe l'exploration
    Topo::CoEdge* m_edge;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDMESHEXPLORER_H_ */
/*----------------------------------------------------------------------------*/
