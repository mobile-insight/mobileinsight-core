#ifndef __DM_ENDEC_C_LOG_CONFIG_H__
#define __DM_ENDEC_C_LOG_CONFIG_H__

#include "consts.h"

#include <utility>
#include <vector>

enum LogConfigOp {
    DISABLE = 0,
    GET_RANGE = 1,
    SET_MASK = 3,
    GET_MASK = 4
};

typedef std::pair<char*, int> BinaryBuffer;
typedef std::vector<int> IdVector;

int get_equip_id (int type_id);
int get_item_id (int type_id);
BinaryBuffer encode_log_config (LogConfigOp op, const IdVector& type_ids);

#endif  // __DM_ENDEC_C_LOG_CONFIG_H__
