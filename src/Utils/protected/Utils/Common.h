/*----------------------------------------------------------------------------*/
/** \file Common.h
 *
 *  \author Eric BriÃ¨re de l'Isle
 *
 *  \date 25/10/2010
 *
 *
 *  NB: pour simplifier leur utilisation, les macros ne sont pas dans le namespace Utils
 *  mais seulement dans Mgx3D [EB]
 */
/*----------------------------------------------------------------------------*/
#ifndef UTILS_COMMON_H_
#define UTILS_COMMON_H_

#include <string>
#include <iostream>
#include <iomanip>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
extern "C"
{
  /*! Affichage d'un message indiquant que la fonction n'est pas encore implémenté. */
  void MgxNotYetImplemented(const char*,const char*,unsigned int,const char*);
  /*! Affichage d'un message indiquant que la fonction est obsolète. */
  void MgxObsolete(const char*,const char*,unsigned int,const char*);
  /*! Affichage d'un message indiquant que la fonction est interdite. */
  void MgxForbidden(const char*,const char*,unsigned int,const char*);
}
/*----------------------------------------------------------------------------*/

#define MGX_NOT_YET_IMPLEMENTED(a) \
{ Mgx3D::MgxNotYetImplemented(__FILE__,__PRETTY_FUNCTION__,__LINE__,(a)); }
#define MGX_OBSOLETE(a) \
{ Mgx3D::MgxObsolete(__FILE__,__PRETTY_FUNCTION__,__LINE__,(a)); }
#define MGX_FORBIDDEN(a) \
{ Mgx3D::MgxForbidden(__FILE__,__PRETTY_FUNCTION__,__LINE__,(a)); }

/*----------------------------------------------------------------------------*/
/// Alias pour pythoneries
#define getContextAlias() "ctx"
#define getMgx3DAlias()   "Mgx3D"


/**
 * Macros d'instanciations de logs de type Trace.
 * @param		name est le nom de l'instance
 * @param		text est le message
 * @param		type est le niveau de trace souhaité
 */
#define MGX_TRACE_LOG(name, text, type)                                          \
TkUtil::TraceLog	name (TkUtil::UTF8String (text, type, TkUtil::UTF8String (__FILE__, __LINE__);

#define MGX_TRACE_LOG_1(name, text)                                              \
TkUtil::TraceLog	name (TkUtil::UTF8String (text, TkUtil::Charset::UTF_8), Log::TRACE_1, TkUtil::UTF8String (__FILE__, TkUtil::Charset::UTF_8), __LINE__);

#define MGX_TRACE_LOG_2(name, text)                                              \
TkUtil::TraceLog	name (TkUtil::UTF8String (text, TkUtil::Charset::UTF_8), Log::TRACE_2, TkUtil::UTF8String (__FILE__, TkUtil::Charset::UTF_8), __LINE__);

#define MGX_TRACE_LOG_3(name, text)                                              \
TkUtil::TraceLog	name (TkUtil::UTF8String (text, TkUtil::Charset::UTF_8), Log::TRACE_3, TkUtil::UTF8String (__FILE__, TkUtil::Charset::UTF_8), __LINE__);

#define MGX_TRACE_LOG_4(name, text)                                              \
TkUtil::TraceLog	name (TkUtil::UTF8String (text, TkUtil::Charset::UTF_8), Log::TRACE_4, TkUtil::UTF8String (__FILE__, TkUtil::Charset::UTF_8), __LINE__);

#define MGX_TRACE_LOG_5(name, text)                                              \
TkUtil::TraceLog	name (TkUtil::UTF8String (text, TkUtil::Charset::UTF_8), Log::TRACE_5, TkUtil::UTF8String (__FILE__, TkUtil::Charset::UTF_8), __LINE__);


/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D

/*----------------------------------------------------------------------------*/

#endif /* UTILS_COMMON_H_ */
/*----------------------------------------------------------------------------*/
