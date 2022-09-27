#pragma once

// Singleton ∏≈≈©∑Œ
#define SINGLE(type) public:\
							static type* GetInst()\
							{\
								 static type mgr;\
								 return &mgr;\
							 }