/***************************************************************************//**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Event Filter Interface
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SL_NCP_EVT_FILTER_COMMON_H
#define SL_NCP_EVT_FILTER_COMMON_H

/***********************************************************************************************//**
 * @addtogroup ncp_evt_filter
 * @{
 **************************************************************************************************/

// Manage event filter user command
#define SL_NCP_EVT_FILTER_CMD_ADD_ID    0
#define SL_NCP_EVT_FILTER_CMD_REMOVE_ID 1
#define SL_NCP_EVT_FILTER_CMD_RESET_ID  2

// Manage event filter command payload length
#define SL_NCP_EVT_FILTER_CMD_ADD_LEN    5
#define SL_NCP_EVT_FILTER_CMD_REMOVE_LEN 5
#define SL_NCP_EVT_FILTER_CMD_RESET_LEN  1

/** @} (end addtogroup ncp_evt_filter) */
#endif // SL_NCP_EVT_FILTER_COMMON_H
