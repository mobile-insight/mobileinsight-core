/*
 * NR5G PDCP DL Data Pdu
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

static int _decode_nr_pdcp_dl_data_pdu (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;

    // return offset - start to return however many bytes we have left remaining
    return offset - start;
}




















