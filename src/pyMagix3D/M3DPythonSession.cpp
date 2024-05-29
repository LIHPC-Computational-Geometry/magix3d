#include "Python/M3DPythonSession.h"
#include <TkUtil/Exception.h>
#include <TkUtil/ThreadPool.h>	// Test CP
#include <patchlevel.h>	// PY_VERSION

#include <assert.h>


using namespace std;
using namespace TkUtil;

// La version 3.0.0.0 au format hexadécimal selon l'API Python (cf. PY_VERSION_HEX dans patchlevel.h) :
#define PY_3000_VERSION_HEX ((3 << 24) | (0 << 16) | (0 << 8) | (0xF << 4) | (0 << 0))

extern "C"
{	// Générés par les bindings swig de TkUtil et Magix 3D.
#if PY_VERSION_HEX >= PY_3000_VERSION_HEX
	void PyInit__TkUtilScripting ( );
	void PyInit__pyMagix3D ( );
#else	// PY_VERSION_HEX >= PY_3000_VERSION_HEX
	void init_TkUtilScripting ( );
	void init_pyMagix3D ( );
#endif	// PY_VERSION_HEX >= PY_3000_VERSION_HEX
}


//extern TkUtil::PythonSession* createMgx3DPythonSession ( )
TkUtil::PythonSession* createMgx3DPythonSession ( )
{
	return new Mgx3D::Python::M3DPythonSession ( );
}	// createMgx3DPythonSession



namespace Mgx3D {

namespace Python {


size_t			M3DPythonSession::_instanceCount	= 0;
const Version	M3DPythonSession::_version (std::string(M3D_PYTHON_API_VERSION).empty()?"0.0.0":M3D_PYTHON_API_VERSION);


M3DPythonSession::M3DPythonSession ( )
	: PythonSession ( )
{
	if (0 == _instanceCount)
	{
#if PY_VERSION_HEX >= PY_3000_VERSION_HEX
		PyInit__TkUtilScripting ( );
		PyInit__pyMagix3D ( );
#else	// PY_VERSION_HEX >= PY_3000_VERSION_HEX
		init_TkUtilScripting ( );
		init_pyMagix3D ( );
#endif	// PY_VERSION_HEX >= PY_3000_VERSION_HEX
//ThreadPool::initialize (0);	// Test CP pour exécution sous Lem
	}	// if (0 == _instanceCount)

	_instanceCount++;
}	// M3DPythonSession::M3DPythonSession


M3DPythonSession::M3DPythonSession (const M3DPythonSession&)
{
	assert (0 && "M3DPythonSession copy constructor is not allowed.");
}	// M3DPythonSession::M3DPythonSession (const M3DPythonSession&)


M3DPythonSession& M3DPythonSession::operator = (const M3DPythonSession&)
{
	assert (0 && "M3DPythonSession operator = is not allowed.");
	return *this;
}	// M3DPythonSession::operator =


M3DPythonSession::~M3DPythonSession ( )
{
	assert ((0 != _instanceCount) && "M3DPythonSession::~M3DPythonSession");
#ifndef SWIG_FORBIDS_FINALIZATION
	_instanceCount--;
#endif	// SWIG_FORBIDS_FINALIZATION
}	// M3DPythonSession::~M3DPythonSession


void M3DPythonSession::init (const vector<string>& sysPaths)
{
	static	bool	inited	= false;

	if (true == inited)
		return;

	for (vector<string>::const_iterator it = sysPaths.begin ( );
	     sysPaths.end ( ) != it; it++)
		PythonSession::addSysPath (*it);

	inited	= false;
}	// M3DPythonSession::init

}	// namespace Python

}	// namespace Mgx3D
