#ifndef MGX3D_INTERNAL_ODOBJECT_H_
#define MGX3D_INTERNAL_ODOBJECT_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Common.h"
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
/**
 * \class ODObject
 * \brief Sommet de la géométrie
 */
class ODObject {

protected:
  
  /*------------------------------------------------------------------------*/
  /** \brief  Constructeur
  */
  ODObject(){}

public:

  static Internal::ODObject* nameToEntity(const Internal::ContextIfc& context, const std::string& name)
  {
    Utils::Entity& entity	= context.nameToEntity (name);
    ODObject*	o	= dynamic_cast<ODObject*>(&entity);

    if (0 != o)
      return o;

    throw TkUtil::Exception (TkUtil::UTF8String ("Absence de point de ce nom.", TkUtil::Charset::UTF_8));
  }

  virtual Utils::Math::Point getPoint () const = 0;

  /// Destructeur
  virtual ~ODObject() {}

};
/*----------------------------------------------------------------------------*/

} // end namespace Geom

} // end namespace Mgx3D

#endif /* MGX3D_GEOM_ODOBJECT_H_ */
