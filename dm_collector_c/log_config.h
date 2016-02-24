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
    DEBUG_LTE_ML1 = 5, //Yuanjie: enable debugging messages (currently for iCellular only)
    DEBUG_WCDMA_L1 = 6, //Yuanjie: enable debugging messages (currently for iCellular only)
};

// Encode a log config message.
// Note that all type IDs should have the same equip ID.
BinaryBuffer encode_log_config (LogConfigOp op, const IdVector& type_ids);

#endif  // __DM_COLLECTOR_C_LOG_CONFIG_H__
