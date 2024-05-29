#ifndef SURFMESHPARAM_H_
#define SURFMESHPARAM_H_

#include <vector>
#include <string>
#include <iostream>
#include <map>

/*----------------------------------------------------------------------------*/
namespace MeshGems{

class SurfMeshParam {

	friend class SurfMesh;

public:

	typedef struct esize_t_{
		double s;
		int np;
		double *t;
		double *xyz;
		esize_t_():s(0),np(0),t(0),xyz(0){}
	}esize_t;

	/*------------------------------------------------------------------------*/
	/** \brief Constructeur
	 */
	SurfMeshParam();
	/*------------------------------------------------------------------------*/
	/** \brief Destructeur
	 */
	~SurfMeshParam();
	/*------------------------------------------------------------------------*/
	/** \brief Mise à jour de la taille cible pour une face
	 *
	 * \param fid l'id de la face considérée
	 * \param size la taille souhaitée pour la face choisie
	 */
	int setFaceSize(int fid, double size);
	/*------------------------------------------------------------------------*/
	/** \brief Mise à jour de la taille cible pour une courbe
	 *
	 * \param eid l'id de la courbe considérée
	 * \param size la taille souhaitée pour la courbe choisie
	 */
	int setEdgeSize(int eid, double size);
	/*------------------------------------------------------------------------*/
	/** \brief Mise à jour de la taille cible pour une courbe en indiquant le
	 *         nombre de points que l'on veut sur cette courbe
	 *
	 * \param eid l'id de la courbe considérée
	 * \param np  nombre de points voulu
	 */

	int setEdgeRequiredNbPoint(int eid, int np);
	/*------------------------------------------------------------------------*/
	/** \brief définition pour une courbe des points la discrétisant
	 *
	 * \param eid l'id de la courbe considérée
	 * \param np  nombre de points la discrétisant
	 * \param t   tableau de paramètres d'abscisses curviligne des points en
	 * 			  question
	 * \param xyz position des points en question
	 */
	int setEdgeRequiredDiscretization(
			int eid, int np,
			double *t, double *xyz);

	/*------------------------------------------------------------------------*/
	/** \brief définition d'une gradation pour effectuer le maillage
	 *
	 * \param g la nouvelle gradation
	 */
	int setMeshGradation(double g);
	/*------------------------------------------------------------------------*/
	/** \brief Indique que l'on maillera la face d'id fid si toMesh=true
	 *
	 */
	void setFaceToMesh(int fid, bool toMesh);

private:

    std::map<int, bool> fmap_to_mesh;
	std::map<int, esize_t> emap;
	std::map<int, double> fmap;
	double gradation;
	int has_required_discretisation;
	int has_required_np;
	int has_local_size_on_face;
	int has_local_size_on_edge;
    int has_selected_faces;

};
/*----------------------------------------------------------------------------*/
} // namespace MeshGems
/*----------------------------------------------------------------------------*/
#endif /* SURFMESHPARAM_H_ */


