/*----------------------------------------------------------------------------*/
/** \file CommandNewTopo.h
 *
 *  \author Simon Calderan
 *
 *  \date 03/01/2025
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWTOPO_H_
#define COMMANDNEWTOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandCreateTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
    namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandNewTopoOnGeometry
 *  \brief Commande permettant de créer une topologie pour une géométrie quelconque
 *  avec réutilisation de la topologie existante
 *  Fonctionne également sans géométrie, mais avec nom de groupe
 */
/*----------------------------------------------------------------------------*/
        class CommandNewTopo: public CommandEditTopo {

        public:

            /** type de topologie demandée, non structuré, structuré avec projection,
             * un bloc sans projection, ou un bloc pour l'insertion
             */
            enum eTopoType { UNSTRUCTURED_BLOCK, STRUCTURED_BLOCK, CONFORM };

            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur
             *
             *  \param c le contexte
             *  \param entity un Volume ou une Surface
             *  \param topoType un énuméré sur le type de topologie souhaitée
             */
            CommandNewTopo(Internal::Context& c, std::vector<Topo::Vertex*> sommets, eTopoType topoType,
                           std::string groupName);
            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur
             *
             *  \param c le contexte
             *  \param entity un Volume ou une Surface
             *  \param topoType un énuméré sur le type de topologie souhaitée
             */
            CommandNewTopo(Internal::Context& c, const Utils::Math::Point point,
                           std::string groupName);

            /*------------------------------------------------------------------------*/
            /** \brief  Destructeur
             */
            virtual ~CommandNewTopo();

            /*------------------------------------------------------------------------*/
            /** \brief  exécute la commande
             */
            virtual void internalExecute();

            /*------------------------------------------------------------------------*/
            /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
             *  une représentation des entités créées
             *
             *  La commande doit ensuite être détruite
             */
            virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

        private:

            /** Création d'un sommet topologique */
            virtual void createVertex();

            /** Création d'une arête topologique */
            virtual CoEdge* createCoEdge(Vertex* v0,Vertex* v1);

            /** Création d'une face topologique structurée */
            virtual void createFace();

            virtual Topo::CoEdge* getCommonEdge(const Vertex* v0, const Vertex* v1);

        private:

            /// dimension de l'entité topo libre
            short m_dim;

            /// nom du groupe pour cas d'un bloc seul (sans projection)
            std::string m_groupName;

            Utils::Math::Point m_point;

            std::vector<Topo::Vertex*> m_vertices;

            /// nombre de bras pour la première direction dans le cas structuré
            uint m_ni;
            /// nombre de bras pour la deuxième direction dans le cas structuré
            uint m_nj;
            /// nombre de bras pour la troisième direction dans le cas structuré
            uint m_nk;

        };
/*----------------------------------------------------------------------------*/
    } // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWTOPO_H_ */
/*----------------------------------------------------------------------------*/


