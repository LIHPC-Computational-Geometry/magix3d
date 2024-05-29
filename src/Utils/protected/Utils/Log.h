/*
 * Eric B le 23/5/2019
 */

#ifndef MGX3D_UTILS_LOG_H
#define MGX3D_UTILS_LOG_H


namespace Mgx3D {
namespace Utils {

#ifndef SWIG
extern "C"
{
void maillage_log(int argc, char *argv[], const char* name);
}
#endif

} // end namespace Utils
} // end namespace Mgx3D

#endif

