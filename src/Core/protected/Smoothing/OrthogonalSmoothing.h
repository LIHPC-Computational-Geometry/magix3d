

#ifndef ORTHOGONALSMOOTHING_HPP_
#define ORTHOGONALSMOOTHING_HPP_

#include "Instruction.hpp"
namespace Mesquite2 {

class OrthogonalSmoothing : Instruction
{
        
        int m_nbiter;
public:
	OrthogonalSmoothing(int nb_iteration = 0);
        virtual double loop_over_mesh(MeshDomainAssoc* mesh_and_domain,
                                  const Settings *settings, // may be null
                                  MsqError &err) ;
       
        virtual std::string get_name() const {
                return "OrthogonalSmoothing";
        }
        virtual void initialize_queue( MeshDomainAssoc* mesh_and_domain,
                                         const Settings* settings,
                                         MsqError& err )
        {
          /* do nothing as default usage */
        }
        

	virtual ~OrthogonalSmoothing();
};

}
#endif /*ORTHOGONALSMOOTHING_HPP_*/
