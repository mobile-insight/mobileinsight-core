/* log_config.h
 * Author: Jiayao Li
 * Defines constants and functions related to log config messages.
 */

#ifndef __DM_COLLECTOR_C_LOG_CONFIG_H__
#define __DM_COLLECTOR_C_LOG_CONFIG_H__

#include "utils.h"
#include "consts.h"

#include <utility>
#include <vector>

typedef std::pair<char*, int> BinaryBuffer;

// Get the equip ID and item ID from a 2-byte log type ID.
int get_equip_id (int type_id);
int get_item_id (int type_id);

// Operations of log config messages
enum LogConfigOp {
    DISABLE = 0,
    GET_RANGE = 1,
    SET_MASK = 3,
    GET_MASK = 4,
    DEBUG_LTE_ML1 = 105, //Yuanjie: enable debugging messages (currently for iCellular only)
    DEBUG_WCDMA_L1 = 106, //Yuanjie: enable debugging messages (currently for iCellular only)
    DISABLE_DEBUG = 107,	// Disable all dbg msg
    DIAG_BEGIN = 108,	// Beginning of DIAG.CFG
    DIAG_END = 109,		// End of DIAG.CFG
    DIAG_BEGIN_1D = 110,
    DIAG_BEGIN_00 = 111,
    DIAG_BEGIN_7C = 112,
    DIAG_BEGIN_1C = 113,
    DIAG_BEGIN_0C = 114,
    DIAG_BEGIN_63 = 115,
    DIAG_BEGIN_4B0F0000 = 116,
    DIAG_BEGIN_4B090000 = 117,
    DIAG_BEGIN_4B080000 = 118,
    DIAG_BEGIN_4B080100 = 119,
    DIAG_BEGIN_4B040000 = 120,
    DIAG_BEGIN_4B040F00 = 121,
    DIAG_END_6000 = 122,
};

// Encode a log config message.
// Note that all type IDs should have the same equip ID.
BinaryBuffer encode_log_config (LogConfigOp op, const IdVector& type_ids);

#endif  // __DM_COLLECTOR_C_LOG_CONFIG_H__
