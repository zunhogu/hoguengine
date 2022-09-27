#pragma once

#ifdef CREATEDLL_EXPORTS
#define MYDLL_DECLSPEC __declspec(dllexport)
#else
#define MYDLL_DECLSPEC __declspec(dllimport)
#endif

#define SINGLE(type) public:\
							static type* GetInst()\
							{\
								 static type mgr;\
								 return &mgr;\
							 }