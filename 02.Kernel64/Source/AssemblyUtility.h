/*
 * AssemblyUtility.h
 *
 *  Created on: 2017. 5. 24.
 *      Author: Administrator
 */

#ifndef __ASSEMBLYUTILITY_H_
#define __ASSEMBLYUTILITY_H_

#include "Types.h"

/*
 * Functions
 */

/*
 * 포트로부터 한바이트를 읽음
 */
BYTE  kInPortByte(WORD wPort);

/*
 * 포트에 한바이트를 씀
 */
BYTE  kOutPortByte(WORD wPort, BYTE bData);

#endif /* __ASSEMBLYUTILITY_H_ */
