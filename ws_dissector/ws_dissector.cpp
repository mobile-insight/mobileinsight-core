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

int main(int argc, char** argv)
{
    if (argc > 1) {
        printf("This program works!\n");
        return 0;
    }

    epan_init(register_all_protocols, register_all_protocol_handoffs, 
                NULL, NULL);
 
    proto_register_aww();
    proto_reg_handoff_aww();
 
    epan_t *session = epan_new();
    char s[] = "uat:user_dlts:\"User 1 (DLT=148)\",\"aww\",\"0\",\"\",\"0\",\"\"";
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
        unsigned int type = ntohl(*(uint32_t *)buffer);
        size_t data_len = ntohl(*((uint32_t *)(buffer + 4)));
        fread(buffer + 2 * 4, 1, data_len, stdin);
        // fprintf(stderr, "type = %u, size = %u\n", type, (unsigned int) data_len);
        
        try_dissect(session, data_len + 2 * 4, buffer);
        printf("===___===\n");
    }

    epan_free(session);
    epan_cleanup();
    return 0;
}