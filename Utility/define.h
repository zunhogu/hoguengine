#pragma once

#define SINGLE(type) public:\
							static type* GetInst()\
							{\
								 static type mgr;\
								 return &mgr;\
							 }