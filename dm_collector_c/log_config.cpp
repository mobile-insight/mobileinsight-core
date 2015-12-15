/* log_config.cpp
 * Author: Jiayao Li
 */

#include <Python.h>

#include "consts.h"
#include "log_config.h"

#include <algorithm>

int
get_equip_id (int type_id) {
    const int EQUIP_ID_MASK = 0x0000F000;
    return (type_id & EQUIP_ID_MASK) >> 12;
}

int
get_item_id (int type_id) {
    const int ITEM_ID_MASK  = 0x00000FFF;
    return type_id & ITEM_ID_MASK;
}

// Note that all type IDs should have the same equip ID.
BinaryBuffer
encode_log_config (LogConfigOp op, const std::vector<int>& type_ids) {
    BinaryBuffer buf;
    buf.first = NULL;
    buf.second = 0;
    switch (op) {
    case DISABLE:
        buf.second = sizeof(char) * 4 + sizeof(int);
        buf.first = new char[buf.second];
        buf.first[0] = 115;
        buf.first[1] = 0;
        buf.first[2] = 0;
        buf.first[3] = 0;
        *((int *)(buf.first + 4)) = (int) op;
        break;

    case SET_MASK:
        {
            int equip_id = -1;
            int highest = -1;
            for (size_t i = 0; i < type_ids.size(); i++) {
                int id = type_ids[i];
                int e = get_equip_id(id);
                if (equip_id == -1) {
                    equip_id = e;
                    highest = std::max(highest, get_item_id(id));
                } else if (equip_id == e) {
                    highest = std::max(highest, get_item_id(id));
                } else {
                    equip_id = -1;
                    break;
                }
            }
            if (equip_id == -1) {
                ;   // fail to extract an unique equid id
            } else {
                int mask_len = (highest / 8) + 1;
                buf.second = sizeof(char) * 4 + sizeof(int) * 3 + mask_len;
                buf.first = new char[buf.second];
                buf.first[0] = 115;
                buf.first[1] = 0;
                buf.first[2] = 0;
                buf.first[3] = 0;
                *((int *)(buf.first + 4)) = (int) op;
                *((int *)(buf.first + 8)) = equip_id;
                *((int *)(buf.first + 12)) = highest + 1;
                char *mask = buf.first + 16;
                for (int i = 0; i < mask_len; i++)
                    mask[i] = 0;
                for (size_t i = 0; i < type_ids.size(); i++) {
                    int id = type_ids[i];
                    int x = get_item_id(id);
                    mask[x / 8] |= 1 << (x % 8);
                }
            }
            break;
        }

    case GET_RANGE:
    case GET_MASK:
        // not implemented
        // fall through
    default:
        break;
    }
    return buf;
}
