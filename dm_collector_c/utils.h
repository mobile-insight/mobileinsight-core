#ifndef __DM_COLLECTOR_C_UTILS_H__
#define __DM_COLLECTOR_C_UTILS_H__

#define ARRAY_SIZE(array, element_type) (sizeof(array) / sizeof(element_type))

#include <vector>

typedef std::vector<int> IdVector;

struct ValueName {
    int val;
    const char *name;
    bool b_public;	//Yuanjie: True if exposed to public, False otherwise
};

int find_ids (const ValueName id_to_name [], int n, const char *name, IdVector& out_vector);
const char* search_name (const ValueName id_to_name [], int n, int val);

#endif // __DM_COLLECTOR_C_UTILS_H__
