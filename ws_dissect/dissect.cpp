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

#include "packet-aww.h"
#include <stdio.h>
#include <arpa/inet.h>

const int BUFFER_SIZE = 2000;
guchar buffer[BUFFER_SIZE] = {};

// // 帧数据, 不包括PCAP文件头和帧头
// // 数据为ethernet - ipv4 - udp - DNS, 上网时随便捕获的.
// #define DATA_LEN 73
// const guchar data[DATA_LEN] = 
// {
// 0x7E, 0x6D, 0x20, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x45, 0x00,
// 0x00, 0x3B, 0x5F, 0x15, 0x00, 0x00, 0x40, 0x11, 0xF1, 0x51, 0x73, 0xAB, 0x4F, 0x08, 0xDB, 0x8D,
// 0x8C, 0x0A, 0x9B, 0x90, 0x00, 0x35, 0x00, 0x27, 0xEF, 0x4D, 0x43, 0x07, 0x01, 0x00, 0x00, 0x01,
// 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x74, 0x04, 0x73, 0x69, 0x6E, 0x61, 0x03, 0x63, 0x6F,
// 0x6D, 0x02, 0x63, 0x6E, 0x00, 0x00, 0x01, 0x00, 0x01
// };

// lte-rrc.pcch
// #define DATA_LEN (9)
// const guchar data[DATA_LEN] = {0x40, 0x01, 0xBF, 0x28, 0x1A, 0xEB, 0xA0, 0x00, 0x00};

// qcdm
// #define DATA_LEN (24)
// const guchar data[DATA_LEN] = 
// {
//     0x10, 0x00, 0x14, 0x00, 0x14, 0x00, 0x2F, 0x41, 0x00, 0x00, 
//     0xFE, 0x63, 0x76, 0xF2, 0xCD, 0x00, 0xFE, 0x28, 0x04, 0x00, 
//     0x07, 0x02, 0x00, 0x00
// };

void print_tree(const proto_tree* tree, int level)
{
    if(tree == NULL)
        return;

    for(int i=0; i<level; ++i)
        printf("    ");

    gchar field_str[ITEM_LABEL_LENGTH + 1] = {0};
    if(tree->finfo->rep == NULL)
        proto_item_fill_label(tree->finfo, field_str);
    else
        strcpy(field_str, tree->finfo->rep->representation);

    if(!PROTO_ITEM_IS_HIDDEN(tree))
        printf("%s\n", field_str);   

    print_tree(tree->first_child, level+1);
    print_tree(tree->next, level);
}

void try_dissect(epan_t *session, size_t data_len, const guchar* raw_data)
{
    wtap_pkthdr phdr;
    frame_data fdata;
    
    memset(&phdr, 0, sizeof(wtap_pkthdr));
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
    fdata.lnk_t = WTAP_ENCAP_USER1;
    fdata.flags.encoding = PACKET_CHAR_ENC_CHAR_ASCII;
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
    proto_tree_write_pdml(edt, stdout);

    epan_dissect_free(edt);
    frame_data_destroy(&fdata);
}

void my_init_plugins1 ()
{
    init_process_policies();

    /* Register all the plugin types we have. */
    epan_register_plugin_types(); /* Types known to libwireshark */
    // wtap_register_plugin_types(); /* Types known to libwiretap */

    /* Scan for plugins.  This does *not* call their registration routines;
       that's done later. */
    scan_plugins();

    /* Register all libwiretap plugin modules. */
    // register_all_wiretap_modules();
}

void my_init_plugins2 ()
{
    // register_all_plugin_tap_listeners();
    // plugins_dump_all();
}

int main(int argc, char** argv)
{
    // my_init_plugins1();

    epan_init(register_all_protocols, register_all_protocol_handoffs, 
                NULL, NULL);
    
    // my_init_plugins2();
 
    proto_register_aww();
    proto_reg_handoff_aww();
 
    epan_t *session = epan_new();
    char s[] = "uat:user_dlts:\"User 1 (DLT=148)\",\"aww\",\"0\",\"\",\"0\",\"\"";
    // char s[] = "uat:user_dlts:\"User 1 (DLT=148)\",\"qcdm\",\"0\",\"\",\"0\",\"\"";
    switch (prefs_set_pref(s)) {
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
        int ign = fread(buffer, sizeof(uint32_t), 2, stdin);
        if (ign < 2)
            break;
        size_t data_len = ntohl(*((uint32_t *)buffer));
        unsigned int type = ntohl(*(uint32_t *)(buffer + 4));
        fread(buffer + 2 * sizeof(uint32_t), sizeof(char), data_len, stdin);
        // fprintf(stderr, "type = %u, size = %u\n", type, (unsigned int) data_len);
        
        try_dissect(session, 2 * sizeof(uint32_t) + data_len, buffer);
        printf("===___===\n");
    }

    epan_free(session);
    epan_cleanup();
    return 0;
}