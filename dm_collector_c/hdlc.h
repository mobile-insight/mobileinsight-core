#ifndef __DM_COLLECTOR_C_HDLC_H__
#define __DM_COLLECTOR_C_HDLC_H__

#include <string>

std::string encode_hdlc_frame (const char *payld, int length);
void feed_binary (const char *b, int length);
void reset_binary ();
bool get_next_frame (std::string& output_frame, bool& crc_correct);
void check_frame_format (std::string& output_frame);

#endif  // __DM_COLLECTOR_C_HDLC_H__
