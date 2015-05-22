/*
 * \file libBPMPlatform.h
 * \brief Plattform identifier macros for various platforms
 *
 *  Created on: 15.10.2013
 *      Author: kindt
 */

#ifndef LIBBPMCPLATFORM_H_
#define LIBBPMCPLATFORM_H_

#define LIBBPMC_PLATFORM_ARCHITECTURE_POSIX 1
#define LIBBPMC_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS 2

#ifndef LIBBPMC_PLATFORM_ARCHITECTURE
#define LIBBPMC_PLATFORM_ARCHITECTURE LIBBPMC_PLATFORM_ARCHITECTURE_POSIX
#endif
#endif /* LIBBPMCPLATFORM_H_ */
