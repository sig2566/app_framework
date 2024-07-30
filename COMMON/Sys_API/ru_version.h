/*
 *   Version name
 *   Automaticaly updated by the build mechanism
 */

#ifndef RU_SYS_API_PHY_VERSION_H_
#define RU_SYS_API_PHY_VERSION_H_

#define VER_LABEL "5G_PHY_RU"
#define VER_MAJOR "0"
#define VER_MINOR "0"
#define VER_RELEASE "2"
#define VER_BUILD "1"
//#define VER_PATCH "0"

#define VER_NAME_SHORT VER_LABEL "." VER_MAJOR "." VER_MINOR "." VER_RELEASE "." VER_BUILD

#ifdef VER_PATCH
#define VER_NAME VER_NAME_SHORT "." VER_PATCH
#else
#define VER_NAME VER_NAME_SHORT
#endif






#endif /* RU_SYS_API_PHY_VERSION_H_ */
