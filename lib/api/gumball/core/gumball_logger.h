#ifndef GUM_LOGGER_H
#define GUM_LOGGER_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__logger_8h.html

/*!  \file
 *   \brief      Gumball logger
 *   \ingroup    core
 *
 *   Simple wrappers around GBL_LOG_* for convenience
 *   All logs are prefixed with "gumball" as their domain.
 *
 *   \author     2025 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include <gimbal/core/gimbal_logger.h>

#define GUM_LOG_VERBOSE(...)         GBL_LOG_VERBOSE("gumball", __VA_ARGS__)         //!< Logs with GBL_LOG_VERBOSE
#define GUM_LOG_DEBUG(...)           GBL_LOG_DEBUG("gumball", __VA_ARGS__)           //!< Logs with GBL_LOG_DEBUG
#define GUM_LOG_INFO(...)            GBL_LOG_INFO("gumball", __VA_ARGS__)            //!< Logs with GBL_LOG_INFO
#define GUM_LOG_WARN(...)            GBL_LOG_WARN("gumball", __VA_ARGS__)            //!< Logs with GBL_LOG_WARN
#define GUM_LOG_ERROR(...)           GBL_LOG_ERROR("gumball", __VA_ARGS__)           //!< Logs with GBL_LOG_ERROR
#define GUM_LOG_PUSH()               GBL_LOG_PUSH()
#define GUM_LOG_POP(n)               GBL_LOG_POP(n)

#define GUM_LOG_VERBOSE_PUSH(...)    GBL_LOG_VERBOSE_PUSH("gumball", __VA_ARGS__)    //!< Pushes a level to the log stack, then logs with GBL_LOG_VERBOSE
#define GUM_LOG_DEBUG_PUSH(...)      GBL_LOG_DEBUG_PUSH("gumball", __VA_ARGS__)      //!< Pushes a level to the log stack, then logs with GBL_LOG_DEBUG
#define GUM_LOG_INFO_PUSH(...)       GBL_LOG_INFO_PUSH("gumball", __VA_ARGS__)       //!< Pushes a level to the log stack, then logs with GBL_LOG_INFO
#define GUM_LOG_WARN_PUSH(...)       GBL_LOG_WARN_PUSH("gumball", __VA_ARGS__)       //!< Pushes a level to the log stack, then logs with GBL_LOG_WARN
#define GUM_LOG_ERROR_PUSH(...)      GBL_LOG_ERROR_PUSH("gumball", __VA_ARGS__)      //!< Pushes a level to the log stack, then logs with GBL_LOG_ERROR


#define GUM_LOG_VERBOSE_SCOPE(...)   GBL_LOG_VERBOSE_SCOPE("gumball", __VA_ARGS__)   //!< Logs with GBL_LOG_VERBOSE, then pushes a level to the log stack. Pops level when scope ends
#define GUM_LOG_DEBUG_SCOPE(...)     GBL_LOG_DEBUG_SCOPE("gumball", __VA_ARGS__)     //!< Logs with GBL_LOG_DEBUG,   then pushes a level to the log stack. Pops level when scope ends
#define GUM_LOG_INFO_SCOPE(...)      GBL_LOG_INFO_SCOPE("gumball", __VA_ARGS__)      //!< Logs with GBL_LOG_INFO,    then pushes a level to the log stack. Pops level when scope ends
#define GUM_LOG_WARN_SCOPE(...)      GBL_LOG_WARN_SCOPE("gumball", __VA_ARGS__)      //!< Logs with GBL_LOG_WARN,    then pushes a level to the log stack. Pops level when scope ends
#define GUM_LOG_ERROR_SCOPE(...)     GBL_LOG_ERROR_SCOPE("gumball", __VA_ARGS__)     //!< Logs with GBL_LOG_ERROR,   then pushes a level to the log stack. Pops level when scope ends

#endif // GUM_LOGGER_H
