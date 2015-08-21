#include "config.h"
#include <epan/packet.h>

#include <stdio.h>
#include "packet-aww.h"

static const int PROTO_MAX = 1000;

static int proto_aww = -1;

static dissector_table_t proto_table = NULL;

static int hf_data_len  = -1;
static int hf_proto     = -1;

static gint ett_aww     = -1;
static gint ett_payload = -1;

// Keep consistent with ws_dissector.py
static void init_proto_names (const char *protos []) {
    protos[100] = "rrc.ul.ccch";
    protos[101] = "rrc.ul.dcch";
    protos[102] = "rrc.dl.ccch";
    protos[103] = "rrc.dl.dcch";
    protos[104] = "rrc.bcch.bch";
    
    protos[150] = "rrc.si.mib";
    protos[151] = "rrc.si.sib1";
    protos[153] = "rrc.si.sib3";
    protos[155] = "rrc.si.sib5";
    protos[157] = "rrc.si.sib7";
    protos[162] = "rrc.si.sib12";
    protos[169] = "rrc.si.sib19";

    protos[190] = "gsm_a_dtap";

    protos[200] = "lte-rrc.pcch";
    protos[201] = "lte-rrc.dl.dcch";
    protos[202] = "lte-rrc.ul.dcch";
    protos[203] = "lte-rrc.bcch.dl.sch";
    protos[204] = "lte-rrc.dl.ccch";
    protos[205] = "lte-rrc.ul.ccch";

    protos[250] = "nas-eps_plain";
}

static void
dissect_aww(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "Automator Wireshark Wrapper");
    /* Clear out stuff in the info column */
    col_clear(pinfo->cinfo, COL_INFO);

    if (tree) { /* we are being asked for details */
        proto_item *ti = proto_tree_add_item(tree, proto_aww, tvb, 0, -1, ENC_NA);
        proto_tree *aww_tree = proto_item_add_subtree(ti, ett_aww);
        proto_tree_add_item(aww_tree, hf_proto, tvb, 0, 4, ENC_BIG_ENDIAN);
        proto_tree_add_item(aww_tree, hf_data_len, tvb, 4, 4, ENC_BIG_ENDIAN);

        guint32 proto_id = tvb_get_ntohl(tvb, 0);
        tvbuff_t* next_tvb = tvb_new_subset_remaining(tvb, 8);
        // fprintf(stderr, "len = %d\n", tvb_length(next_tvb));
        gboolean success = dissector_try_uint(proto_table, proto_id, next_tvb, pinfo, tree);
        if (!success) {
            fprintf(stderr, "Error: ws_dissector fails to decode protocol %d.\n", proto_id);
        }
    }
}


void
proto_register_aww(void)
{
    static hf_register_info hf[] = {
        { &hf_proto,
            { "Protocol", "aww.proto", FT_UINT32, BASE_DEC,
                NULL, 0x0, NULL, HFILL }
        },
        { &hf_data_len,
            { "Data length", "aww.data_len", FT_UINT32, BASE_DEC,
                NULL, 0x0, NULL, HFILL }
        }
    };

    /* Setup protocol subtree array */
    static gint *ett[] = {
        &ett_aww,
        &ett_payload
    };

    proto_aww = proto_register_protocol(
                    "Automator Wireshark Wrapper",  /* name       */
                    "AWW",                          /* short name */
                    "aww"                           /* abbrev     */
                    );

    proto_register_field_array(proto_aww, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));

    // subdissector table setup
    proto_table = register_dissector_table(
                    "aww.proto",
                    "AWW protocol",
                    FT_UINT16,
                    BASE_DEC
                    );
    
    register_dissector("aww", dissect_aww, proto_aww);
}


void
proto_reg_handoff_aww(void)
{
    const char *protos[PROTO_MAX + 1] = {};
    init_proto_names(protos);

    dissector_handle_t handle = NULL;
    for (int i = 0; i <= PROTO_MAX; i++) {
        if (protos[i] != NULL) {
            handle = find_dissector(protos[i]);
            dissector_add_uint("aww.proto", i, handle);
        }
    }
}

void print_proto_list () {
    const char *protos[PROTO_MAX + 1] = {};
    init_proto_names(protos);
    for (int i = 0; i <= PROTO_MAX; i++) {
        if (protos[i] != NULL) {
            printf("%d: %s\n", i, protos[i]);
        }
    }
}