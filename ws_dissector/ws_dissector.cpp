#include "config.h"

#include <pcap.h>
#include <epan/epan.h>
#include <epan/prefs.h>
#include <epan/epan_dissect.h>
#include <epan/tap.h>
#include <epan/proto.h>
#include <epan/packet_info.h>
#include <epan/frame_data.h>
#include <epan/packet.h>
#include <epan/print.h>
#include <wiretap/wtap.h>
#include <wsutil/plugins.h>
#include <wsutil/privileges.h>
#include <epan/dissectors/packet-pdcp-lte.h>

#include "packet-aww.h"
#include <stdio.h>
#ifdef _WIN32
    #include <winsock2.h>   // for ntohl()
    #include <io.h>
    #define SET_BINARY_MODE(handle) setmode(handle, O_BINARY)
    // Define missing types
    typedef __int32 int32_t;
    typedef unsigned __int32 uint32_t;
#elif __GNUC__
    #include <arpa/inet.h>  // for ntohl()
#else
    #error Your compiler is not either MS Visual C compiler or GNU gcc.
#endif

#define WS_DISSECTOR_VERSION "3.2.7"

const int BUFFER_SIZE = 2000;
guchar buffer[BUFFER_SIZE] = {};

// void print_tree(const proto_tree* tree, int level)
// {
//     if(tree == NULL)
//         return;
// 
//     for(int i=0; i<level; ++i)
//         printf("    ");
// 
//     gchar field_str[ITEM_LABEL_LENGTH + 1] = {0};
//     if(tree->finfo->rep == NULL)
//         proto_item_fill_label(tree->finfo, field_str);
//     else
//         strcpy(field_str, tree->finfo->rep->representation);
// 
//     if(!PROTO_ITEM_IS_HIDDEN(tree))
//         printf("%s\n", field_str);
// 
//     print_tree(tree->first_child, level+1);
//     print_tree(tree->next, level);
// }

void try_dissect(epan_t *session, size_t data_len, const guchar* raw_data)
{
    wtap_rec phdr;
    frame_data fdata;

    memset(&phdr, 0, sizeof(wtap_rec));
    phdr.rec_header.packet_header.pkt_encap = WTAP_ENCAP_USER1;
    frame_data_init(&fdata, 0, &phdr, 0, 0);

    // fdata = (frame_data*)g_new(frame_data, 1);

    // memset(fdata, 0, sizeof(frame_data));
    // fdata->pfd  = NULL;
    // fdata->num = 1;    // fdata->cum_bytes = 0;
    // fdata->file_off = 0;
    // fdata->subnum = 0;
    // fdata->interface_id = 0;
    fdata.pkt_len  = data_len;
    fdata.cap_len  = data_len;
    // fdata->cum_bytes = 0;
    // fdata->file_off = 0;
    // fdata->subnum = 0;
    // fdata.lnk_t = WTAP_ENCAP_ETHERNET;
    // fdata.lnk_t = WTAP_ENCAP_USER1;
    fdata.encoding = PACKET_CHAR_ENC_CHAR_ASCII;
    // fdata->flags.visited = 0;
    // fdata->flags.marked = 0;
    // fdata->flags.ref_time = 0;
    // fdata->color_filter = NULL;
    // fdata->abs_ts.secs = 0;
    // fdata->abs_ts.nsecs = 0;
    // fdata->opt_comment = NULL;

    epan_dissect_t *edt = epan_dissect_new(session, TRUE, TRUE);
    epan_dissect_run(edt, 0, &phdr, tvb_new_real_data(raw_data, data_len, data_len), &fdata, NULL);
    // const proto_tree *payload_tree = edt->tree->first_child->next;
    // print_tree(payload_tree, 0);
    write_pdml_proto_tree(NULL, NULL, PF_NONE, edt, NULL, stdout, FALSE); 

    epan_dissect_free(edt);
    frame_data_destroy(&fdata);
}

int main(int argc, char** argv)
{
     // set stdin to binary mode
#ifdef _WIN32
     // On Windows, 0x1a will cause EOF in text mode, so we must change stdin to
     // binary mode
     SET_BINARY_MODE(fileno(stdin));
#elif __GNUC__
     // This step may not be necessary on Linux/OS X, so I commented it out
     // freopen(NULL, "rb", stdin);
#endif

    if (argc > 1) {
        printf("Version " WS_DISSECTOR_VERSION "\n");
        printf("Supported protocols:\n");
        print_proto_list();
        return 0;
    }

    // to prevent "started_with_special_privs: assertion failed" error.
    init_process_policies();

    wtap_init(TRUE);
    epan_init(NULL, NULL, TRUE);

    proto_register_aww();
    proto_reg_handoff_aww();

	static const struct packet_provider_funcs funcs = {
		NULL,
		NULL,
		NULL,
		NULL
	};
    epan_t *session = epan_new(NULL, &funcs);
    char s[] = "uat:user_dlts:\"User 1 (DLT=148)\",\"aww\",\"0\",\"\",\"0\",\"\"";
    char *errmsg = NULL;
    switch (prefs_set_pref(s, &errmsg)) {
    case PREFS_SET_OK:
        break;

    case PREFS_SET_SYNTAX_ERR:
    case PREFS_SET_NO_SUCH_PREF:
    case PREFS_SET_OBSOLETE:
    default:
        fprintf(stderr, "Failed to set user_dlts.\n");
        return 1;
        break;
    }
    prefs_apply_all();

    while (!feof(stdin)) {  // stop dissect when the pipe is closed
        fflush(stdin);
        fflush(stdout);
        size_t ign = fread(buffer, sizeof(uint32_t), 2, stdin);
        if (ign < 2)
            break;
        unsigned int type = ntohl(*(uint32_t *)buffer);
        size_t data_len = ntohl(*((uint32_t *)(buffer + 4)));
        size_t offset = 8;
        size_t framingHeader_len = 0;
        if (type == 300 || type == 301) {
            /* If type is pdcp-lte signaling message, we need to add framing
             * header before read pdcp PDU. */
            /* Fixed start to each frame (allowing heuristic dissector to work
             * ) */
            memcpy(buffer + offset, PDCP_LTE_START_STRING,
                    strlen(PDCP_LTE_START_STRING));
            offset += strlen(PDCP_LTE_START_STRING);

            /* Now write out fixed fields (the mandatory elements of struct
             * pdcp_lte_info */

            /* gboolean no_header_pdu */
            buffer[offset++] = FALSE;

            /* enum pdcp_plane */
            buffer[offset++] = SIGNALING_PLANE;

            /* gboolean rohc_compression */
            buffer[offset++] = FALSE;

            /* Optional fields */
            /* Direction */
            buffer[offset++] = PDCP_LTE_DIRECTION_TAG;
            switch (type) {
                case 300:   // downlink
                    buffer[offset++] = DIRECTION_DOWNLINK;
                    break;
                case 301:   // uplink
                    buffer[offset++] = DIRECTION_UPLINK;
                    break;
            }

            /* Logical Channel Type */
            buffer[offset++] = PDCP_LTE_LOG_CHAN_TYPE_TAG;
            buffer[offset++] = Channel_DCCH;

            /* BCCH Transport Type */
            buffer[offset++] = PDCP_LTE_BCCH_TRANSPORT_TYPE_TAG;
            buffer[offset++] = 0;

            buffer[offset++] = PDCP_LTE_PAYLOAD_TAG;
            framingHeader_len = offset - 8;
        }
        fread(buffer + offset, 1, data_len, stdin);
        // fprintf(stderr, "type = %u, size = %u\n", type, (unsigned int) data_len);

        try_dissect(session, data_len + 2 * 4 + framingHeader_len, buffer);
        printf("===___===\n");  // this line CANNOT be deleted. used to seperate msgs
    }

    epan_free(session);
    epan_cleanup();
    return 0;
}
