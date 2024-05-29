/** \file	SwigCompletion.h
 *
 *  \author	Charles Pignerol
 *
 *  \date	02/10/2017
 *
 *  \brief	Nécessaire pour offrir un service de completion complet depuis la
 *  		console python.
 */


#ifndef SWIG_COMPLETION_H
#define SWIG_COMPLETION_H

#include <iostream>
#include <iomanip>

// Pour bénéficier d'une completion exacte dans la console python lorsque les
// méthodes ont une signature unique. Dans un tel cas, en plus déclaration
// de la méthode (ex : void foo();), ajouter la ligne :
// SET_SWIG_COMPLETABLE_METHOD(foo)
// La console détectera UnusedStructForSwigCompletion et ne proposera pas la
// méthode lors de la complétion.
//#ifndef UNUSED_SWIG_COMPLETION_STRUCT
//#define UNUSED_SWIG_COMPLETION_STRUCT
struct UnusedStructForSwigCompletion { };
//#endif	// UNUSED_SWIG_COMPLETION_STRUCT

#define SET_SWIG_COMPLETABLE_METHOD(method) \
    void method(UnusedStructForSwigCompletion){std::cerr<<"#method (UnusedStructForSwigCompletion) should not be called."<<std::endl; }
#define SET_SWIG_COMPLETABLE_METHOD_RET(rettype,method) \
    rettype method(UnusedStructForSwigCompletion){std::cerr<<"#method (UnusedStructForSwigCompletion) should not be called."<<std::endl; }
#define SET_SWIG_COMPLETABLE_METHOD_CMD(method) \
    Internal::M3DCommandResultIfc* method(UnusedStructForSwigCompletion){std::cerr<<"#method (UnusedStructForSwigCompletion) should not be called."<<std::endl; }


#endif	// SWIG_COMPLETION_H
