/*----------------------------------------------------------------------------*/
/*
 * \file Common.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 nov. 2010
 */
/*----------------------------------------------------------------------------*/
#include <iostream>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/

extern "C" void
MgxNotYetImplemented(const  char* file,const  char* func,unsigned int line,const  char* text)
{
#ifdef _DEBUG
  std::cerr << file << ':' << func << ':' << line << std::endl;
  std::cerr << "Désolé, cette fonction n'est pas encore implémentée";
  if (text)
    std::cerr << ": " << text;
  std::cerr << std::endl;
#endif
}

extern "C" void
MgxObsolete(const  char* file,const  char* func,unsigned int line,const  char* text)
{
  std::cerr << file << ':' << func << ':' << line << std::endl;
  std::cerr << "L'usage de cette fonction est obsolète";
  if (text)
    std::cerr << ": " << text;
  std::cerr << std::endl;
}


extern "C" void
MgxForbidden(const  char* file,const  char* func,unsigned int line,const  char* text)
{
  std::cerr << file << ':' << func << ':' << line << std::endl;
  std::cerr << "L'usage de cette fonction est interdit";
  if (text)
    std::cerr << ": " << text;
  std::cerr << std::endl;
}

/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D

