/*
 * LTE PHY PUSCH Tx Report
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePhyPuschTxReport_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePhyPuschTxReport_Payload_v23 [] = {
    {UINT, "Serving Cell ID", 2},    // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPuschTxReport_Record_v23 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "Coding Rate Data", 2},  // x/1024.0
    {UINT, "ACK", 4},   // 1 bit
    {PLACEHOLDER, "CQI", 0},    // 1 bit
    {PLACEHOLDER, "RI", 0}, // 1 bit
    {PLACEHOLDER, "Frequency Hopping", 0},  // 2 bits
    {PLACEHOLDER, "Redund Ver", 0}, // 2 bits
    {PLACEHOLDER, "Mirror Hopping", 0}, // 2 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 0},  // 4 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},  // 4 bits
    {PLACEHOLDER, "DMRS Root Slot 0", 0},   // 11 bits
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {UINT, "DMRS Root Slot 1", 4},  // 11 bits
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "Num of RB", 0},  // 7 bits
    {UINT, "PUSCH TB Size", 2},

    {UINT, "Num ACK Bits", 2},
    {PLACEHOLDER, "ACK Payload", 0},   // 20 bits, bytestream

    {UINT, "Rate Matched ACK Bits", 4},
    {PLACEHOLDER, "Num RI Bits NRI (bits)", 0}, // right shift 1 bit, 3 bits
    {PLACEHOLDER, "RI Payload", 0},    // 4 bits
    {PLACEHOLDER, "Rate Matched RI Bits", 0},   // 11 bits
    {PLACEHOLDER, "PUSCH Mod Order", 0},    // 2 bits
    {PLACEHOLDER, "RI Payload2", 0},    // 4 bits


    {UINT, "PUSCH Digital Gain (dB)", 1},    // 8 bits

    {UINT, "SRS Occasion", 1},   // 1 bit
    {PLACEHOLDER, "Re-tx Index", 0},    // 3 bits
    {SKIP, NULL, 2},

    {UINT, "PUSCH Tx Power (dBm)", 4},    // 10 bits
    {PLACEHOLDER, "Num CQI Bits", 0},   // 8 bits
    {PLACEHOLDER, "Rate Matched CQI Bits", 0},  // 14 bits

    {BYTE_STREAM, "CQI Payload", 16},
    {UINT, "Tx Resampler", 4},
};

const Fmt LtePhyPuschTxReport_Payload_v24 [] = {
    {UINT, "Serving Cell ID", 2},    // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPuschTxReport_Record_v24 [] = {
//modify byte length and remove some fields Num repetition/RB NB Start Index
    {UINT, "Current SFN SF", 2},
    {UINT, "Coding Rate Data", 2},  // x/1024.0
    {UINT, "ACK", 4},   // 1 bit
    {PLACEHOLDER, "CQI", 0},    // 1 bit
    {PLACEHOLDER, "RI", 0}, // 1 bit
    {PLACEHOLDER, "Frequency Hopping", 0},  // 2 bits
    {PLACEHOLDER, "Redund Ver", 0}, // 2 bits
    {PLACEHOLDER, "Mirror Hopping", 0}, // 2 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 0},  // 4 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},  // 4 bits
    {PLACEHOLDER, "DMRS Root Slot 0", 0},   // 11 bits
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {UINT, "DMRS Root Slot 1", 4},  // 11 bits
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "Num of RB", 0},  // 7 bits
    {UINT, "PUSCH TB Size", 2},

    {UINT, "Num ACK Bits", 2},
    {PLACEHOLDER, "ACK Payload", 0},   // 20 bits, bytestream

    {UINT, "Rate Matched ACK Bits", 4},
    {PLACEHOLDER, "Num RI Bits NRI (bits)", 0}, // right shift 1 bit, 3 bits
    {PLACEHOLDER, "RI Payload", 0},    // 4 bits
    {PLACEHOLDER, "Rate Matched RI Bits", 0},   // 11 bits
    {PLACEHOLDER, "PUSCH Mod Order", 0},    // 2 bits
    {PLACEHOLDER, "RI Payload2", 0},    // 4 bits


    {UINT, "PUSCH Digital Gain (dB)", 1},    // 8 bits

    {UINT, "SRS Occasion", 1},   // 1 bit
    {PLACEHOLDER, "Re-tx Index", 0},    // 3 bits
    {SKIP, NULL, 2},

    {UINT, "PUSCH Tx Power (dBm)", 4},    // 10 bits
    {PLACEHOLDER, "Num CQI Bits", 0},   // 8 bits
    {PLACEHOLDER, "Rate Matched CQI Bits", 0},  // 14 bits

    {BYTE_STREAM, "CQI Payload", 16},
    {UINT, "Tx Resampler", 4},
};

const Fmt LtePhyPuschTxReport_Payload_v26 [] = {
    {UINT, "Serving Cell ID", 2},    // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPuschTxReport_Record_v26 [] = {
//modify byte length and remove some fields Num repetition/RB NB Start Index
    {UINT, "Current SFN SF", 2},
    {UINT, "Coding Rate Data", 2},  // x/1024.0
    {UINT, "ACK", 4},   // 1 bit
    {PLACEHOLDER, "CQI", 0},    // 1 bit
    {PLACEHOLDER, "RI", 0}, // 1 bit
    {PLACEHOLDER, "Frequency Hopping", 0},  // 2 bits
    {PLACEHOLDER, "Redund Ver", 0}, // 2 bits
    {PLACEHOLDER, "Mirror Hopping", 0}, // 2 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 0},  // 4 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},  // 4 bits
    {PLACEHOLDER, "DMRS Root Slot 0", 0},   // 11 bits
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {UINT, "DMRS Root Slot 1", 4},  // 11 bits
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "Num of RB", 0},  // 7 bits
    {UINT, "PUSCH TB Size", 2},

    {UINT, "Num ACK Bits", 2},
    {PLACEHOLDER, "ACK Payload", 0},   // 20 bits, bytestream

    {UINT, "Rate Matched ACK Bits", 4},
    {PLACEHOLDER, "Num RI Bits NRI (bits)", 0}, // right shift 1 bit, 3 bits
    {PLACEHOLDER, "RI Payload", 0},    // 4 bits
    {PLACEHOLDER, "Rate Matched RI Bits", 0},   // 11 bits
    {PLACEHOLDER, "PUSCH Mod Order", 0},    // 2 bits
    {PLACEHOLDER, "RI Payload2", 0},    // 4 bits


    {UINT, "PUSCH Digital Gain (dB)", 1},    // 8 bits

    {UINT, "SRS Occasion", 1},   // 1 bit
    {PLACEHOLDER, "Re-tx Index", 0},    // 3 bits
    {SKIP, NULL, 2},

    {UINT, "PUSCH Tx Power (dBm)", 4},    // 10 bits
    {PLACEHOLDER, "Num CQI Bits", 0},   // 8 bits
    {PLACEHOLDER, "Rate Matched CQI Bits", 0},  // 14 bits

    {BYTE_STREAM, "CQI Payload", 16},
    {UINT, "Tx Resampler", 4},
    
    {UINT, "Num Repetition", 4},// 12 bit
    {PLACEHOLDER,"RB NB Start Index",0},//8 bit
};

const Fmt LtePhyPuschTxReport_Payload_v43 [] = {
    {UINT, "Serving Cell ID", 2},    // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPuschTxReport_Record_v43 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "Coding Rate Data", 2},  // x/1024.0
    {UINT, "ACK", 4},   // 1 bit
    {PLACEHOLDER, "CQI", 0},    // 1 bit
    {PLACEHOLDER, "RI", 0}, // 1 bit
    {PLACEHOLDER, "Frequency Hopping", 0},  // 2 bits
    {PLACEHOLDER, "Redund Ver", 0}, // 2 bits
    {PLACEHOLDER, "Mirror Hopping", 0}, // 2 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 0},  // 4 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},  // 4 bits
    {PLACEHOLDER, "DMRS Root Slot 0", 0},   // 11 bits
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {UINT, "DMRS Root Slot 1", 4},  // 11 bits
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "Num of RB", 0},  // 7 bits
    {UINT, "PUSCH TB Size", 2},
    {UINT, "Rate Matched ACK Bits", 2},
    {UINT, "ACK Payload", 4},   // 20 bits, bytestream
    {PLACEHOLDER, "ACK/NAK Inp Length 0", 0},   // 4 bits
    {PLACEHOLDER, "ACK/NAK Inp Length 1", 0},   // 4 bits
    {PLACEHOLDER, "Num RI Bits NRI (bits)", 0}, // right shift 1 bit, 3 bits
    {UINT, "RI Payload", 4},    // 4 bits
    {PLACEHOLDER, "Rate Matched RI Bits", 0},   // 11 bits
    {PLACEHOLDER, "PUSCH Mod Order", 0},    // 2 bits
    {PLACEHOLDER, "PUSCH Digital Gain (dB)", 0},    // 8 bits
    {PLACEHOLDER, "SRS Occasion", 0},   // 1 bit
    {PLACEHOLDER, "Re-tx Index", 0},    // 3 bits
    {UINT, "PUSCH Tx Power (dBm)", 4},    // 7 bits
    {PLACEHOLDER, "Num CQI Bits", 0},   // 8 bits
    {PLACEHOLDER, "Rate Matched CQI Bits", 0},  // 16 bits
    {SKIP, NULL, 4},    // Num DL carriers, Ack Nak Index, Ack Nak Late, Csf Late, Dropped Pusch
    {BYTE_STREAM, "CQI Payload", 28},
    {UINT, "Tx Resampler", 4},
};

const Fmt LtePhyPuschTxReport_Payload_v102 [] = {
    {UINT, "Serving Cell ID", 2},    // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPuschTxReport_Record_v102 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "Carrier Index", 2}, // 2 bits
    {PLACEHOLDER, "ACK", 0},    // 1 bit
    {PLACEHOLDER, "CQI", 0},    // 1 bit
    {PLACEHOLDER, "RI", 0}, // 1 bit
    {PLACEHOLDER, "Frequency Hopping", 0},  // 2 bits
    {PLACEHOLDER, "Re-tx Index", 0},  // 5 bits
    {PLACEHOLDER, "Redund Ver", 0}, // 2 bits
    {PLACEHOLDER, "Mirror Hopping", 0}, // 2 bits
    {UINT, "Resource Allocation Type", 4},  // 1 bit
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "Num of RB", 0},  // 7 bits
    {UINT, "PUSCH TB Size", 2},
    {UINT, "Coding Rate", 2},  // x/1024.0

    {UINT, "Rate Matched ACK Bits", 4}, // 14 bits
    {PLACEHOLDER, "RI Payload", 4},    // 4 bits
    {PLACEHOLDER, "Rate Matched RI Bits", 0},   // 11 bits
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {PLACEHOLDER, "SRS Occasion", 0},   // 1 bit

    {UINT, "ACK Payload", 4},   // 20 bits, bytestream
    {PLACEHOLDER, "ACK/NAK Inp Length 0", 0},   // 4 bits
    {PLACEHOLDER, "ACK/NAK Inp Length 1", 0},   // 4 bits
    {PLACEHOLDER, "Num RI Bits NRI (bits)", 0}, // 3 bits

    {UINT, "PUSCH Mod Order", 4},    // 2 bits
    {PLACEHOLDER, "PUSCH Digital Gain (dB)", 0},    // 8 bits
    {PLACEHOLDER, "Start RB Cluster1", 0},  // 8 bits
    {PLACEHOLDER, "Num RB Cluster1", 0},    // 6 bits

    {UINT, "PUSCH Tx Power (dBm)", 4},    // 7 bits (x - 128)
    {PLACEHOLDER, "Num CQI Bits", 0},   // 8 bits
    {PLACEHOLDER, "Rate Matched CQI Bits", 0},  // 16 bits

    {UINT, "Num DL Carriers", 4},   // 2 bits
    {PLACEHOLDER, "Ack Nack Index", 0}, // 12 bits
    {PLACEHOLDER, "Ack Nack Late", 0}, // 1 bit
    {PLACEHOLDER, "CSF Late", 0}, // 1 bit
    {PLACEHOLDER, "Drop PUSCH", 0}, // 1 bit

    {BYTE_STREAM, "CQI Payload", 28},

    {UINT, "Tx Resampler", 4},

    {UINT, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 4},  // 4 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},  // 4 bits
    {PLACEHOLDER, "DMRS Root Slot 0", 0},   // 11 bits
    {PLACEHOLDER, "DMRS Root Slot 1", 0},  // 11 bits
};

const Fmt LtePhyPuschTxReport_Record_v122 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "UL Carrier Index", 2}, // 2 bits
    {PLACEHOLDER, "ACK", 0},    // 1 bit
    {PLACEHOLDER, "CQI", 0},    // 1 bit
    {PLACEHOLDER, "RI", 0}, // 1 bit
    {PLACEHOLDER, "Frequency Hopping", 0},  // 2 bits
    {PLACEHOLDER, "Re-tx Index", 0},  // 5 bits
    {PLACEHOLDER, "Redund Ver", 0}, // 2 bits
    {PLACEHOLDER, "Mirror Hopping", 0}, // 2 bits

    {UINT, "Start RB Slot 0", 4},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "Num of RB", 0},  // 7 bits
    {PLACEHOLDER, "DL Carrier Index",0},//2 bits

    {UINT, "PUSCH TB Size", 2},
    {UINT, "Coding Rate", 2},  // x/1024.0

    {UINT, "Rate Matched ACK Bits", 4}, // 14 bits
    {PLACEHOLDER, "RI Payload", 0},    // 4 bits
    {PLACEHOLDER, "Rate Matched RI Bits", 0},   // 11 bits
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {PLACEHOLDER, "SRS Occasion", 0},   // 1 bit

    {UINT, "ACK Payload", 4},   // 20 bits, bytestream
    {PLACEHOLDER, "ACK/NAK Inp Length 0", 0},   // 4 bits
    {PLACEHOLDER, "ACK/NAK Inp Length 1", 0},   // 4 bits
    {PLACEHOLDER, "Num RI Bits NRI (bits)", 0}, // 3 bits

    {UINT, "PUSCH Mod Order", 4},    // 2 bits
    {PLACEHOLDER, "PUSCH Digital Gain (dB)", 0},    // 8 bits

    {UINT, "PUSCH Tx Power (dBm)", 4},    // 7 bits (x - 128)
    {PLACEHOLDER, "Num CQI Bits", 0},   // 8 bits
    {PLACEHOLDER, "Rate Matched CQI Bits", 0},  // 16 bits

    {UINT, "Num DL Carriers", 4},   // 2 bits
    {PLACEHOLDER, "Ack Nack Index", 0}, // 12 bits
    {PLACEHOLDER, "Ack Nack Late", 0}, // 1 bit
    {PLACEHOLDER, "CSF Late", 0}, // 1 bit
    {PLACEHOLDER, "Drop PUSCH", 0}, // 1 bit

    {BYTE_STREAM, "CQI Payload", 28},

    {UINT, "Tx Resampler", 4},

    {UINT, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 4},  // 4 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},  // 4 bits
    {PLACEHOLDER, "DMRS Root Slot 0", 0},   // 11 bits
    {PLACEHOLDER, "DMRS Root Slot 1", 0},  // 11 bits
};

const Fmt LtePhyPuschTxReport_Record_v124 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "UL Carrier Index", 2}, // 2 bits
    {PLACEHOLDER, "ACK", 0},    // 1 bit
    {PLACEHOLDER, "CQI", 0},    // 1 bit
    {PLACEHOLDER, "RI", 0}, // 1 bit
    {PLACEHOLDER, "Frequency Hopping", 0},  // 2 bits
    {PLACEHOLDER, "Re-tx Index", 0},  // 5 bits
    {PLACEHOLDER, "Redund Ver", 0}, // 2 bits
    {PLACEHOLDER, "Mirror Hopping", 0}, // 2 bits
    {UINT, "Resource Allocation Type", 4},  // 1 bit
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "Num of RB", 0},  // 7 bits
    {PLACEHOLDER, "DL Carrier Index",0},//2 bits
    {UINT, "PUSCH TB Size", 2},
    {UINT, "Coding Rate", 2},  // x/1024.0

    {UINT, "Rate Matched ACK Bits", 4}, // 14 bits
    {PLACEHOLDER, "RI Payload", 0},    // 4 bits
    {PLACEHOLDER, "Rate Matched RI Bits", 0},   // 11 bits
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {PLACEHOLDER, "SRS Occasion", 0},   // 1 bit

    {UINT, "ACK Payload", 4},   // 20 bits, bytestream
    {PLACEHOLDER, "ACK/NAK Inp Length 0", 0},   // 4 bits
    {PLACEHOLDER, "ACK/NAK Inp Length 1", 0},   // 4 bits
    {PLACEHOLDER, "Num RI Bits NRI (bits)", 0}, // 3 bits

    {UINT, "PUSCH Mod Order", 4},    // 2 bits
    {PLACEHOLDER, "PUSCH Digital Gain (dB)", 0},    // 8 bits
    {PLACEHOLDER, "Start RB Cluster1", 0},  // 8 bits
    {PLACEHOLDER, "Num RB Cluster1", 0},    // 6 bits

    {UINT, "PUSCH Tx Power (dBm)", 4},    // 7 bits (x - 128)
    {PLACEHOLDER, "Num CQI Bits", 0},   // 8 bits
    {PLACEHOLDER, "Rate Matched CQI Bits", 0},  // 16 bits

    {UINT, "Num DL Carriers", 4},   // 2 bits
    {PLACEHOLDER, "Ack Nack Index", 0}, // 12 bits
    {PLACEHOLDER, "Ack Nack Late", 0}, // 1 bit
    {PLACEHOLDER, "CSF Late", 0}, // 1 bit
    {PLACEHOLDER, "Drop PUSCH", 0}, // 1 bit

    {BYTE_STREAM, "CQI Payload", 44},

    {UINT, "Tx Resampler", 4},

    {UINT, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 4},  // 4 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},  // 4 bits
    {PLACEHOLDER, "DMRS Root Slot 0", 0},   // 11 bits
    {PLACEHOLDER, "DMRS Root Slot 1", 0},  // 11 bits
};

const Fmt LtePhyPuschTxReport_Payload_v144 [] = {
    {UINT, "Serving Cell ID", 2},    // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};


const Fmt LtePhyPuschTxReport_Record_v144 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "UL Carrier Index", 2}, // 2 bits
    {PLACEHOLDER, "ACK", 0},    // 1 bit
    {PLACEHOLDER, "CQI", 0},    // 1 bit
    {PLACEHOLDER, "RI", 0}, // 1 bit
    {PLACEHOLDER, "Frequency Hopping", 0},  // 2 bits
    {PLACEHOLDER, "Re-tx Index", 0},  // 5 bits
    {PLACEHOLDER, "Redund Ver", 0}, // 2 bits
    {PLACEHOLDER, "Mirror Hopping", 0}, // 2 bits

    {UINT, "Resource Allocation Type", 4},  // 1 bit
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "Num of RB", 0},  // 7 bits

    {PLACEHOLDER, "DL Carrier Index",0},//3 bits
    {PLACEHOLDER,"Enable UL DMRS OCC",0}, 	// shift 25 bits,total 1 bits

    {UINT, "PUSCH TB Size", 2},
    {UINT, "Coding Rate", 2},  // x/1024.0

    {UINT, "Rate Matched ACK Bits", 4}, // 14 bits

    {PLACEHOLDER, "RI Payload", 0},    // shift 14 then 14 bits
    {PLACEHOLDER, "Num RI Bits NRI", 0},   // shirt 14 then 4 bits

    //{PLACEHOLDER, "UE SRS", 0}, // 1 bit
    //{PLACEHOLDER, "SRS Occasion", 0},   // 1 bit

    {UINT, "ACK Payload", 16},

    {UINT,"ACK/NAK Inp Length 0",4}, 	// shift 0 bits,total 7 bits
    {PLACEHOLDER,"ACK/NAK Inp Length 1",0}, 	// shift 7 bits,total 7 bits
    {PLACEHOLDER,"Rate Matched RI Bits",0}, 	// shift 14 bits,total 11 bits
    {PLACEHOLDER,"Reserved",0}, 	// shift 25 bits,total 7 bits

    {UINT,"UE SRS",4}, 	// shift 0 bits,total 1 bits
    {PLACEHOLDER,"SRS Occasion",0}, 	// shift 1 bits,total 1 bits
    {PLACEHOLDER,"PUSCH Mod Order",0}, 	// shift 2 bits,total 3 bits
    {PLACEHOLDER,"PUSCH Digital Gain",0}, 	// shift 5 bits,total 8 bits
    {PLACEHOLDER,"Start RB Cluster1",0}, 	// shift 13 bits,total 7 bits
    {PLACEHOLDER,"Num RB Cluster1",0}, 	// shift 20 bits,total 7 bits

    {UINT, "PUSCH Tx Power (dBm)", 4},    // 7 bits (x - 128)
    {PLACEHOLDER, "Num CQI Bits", 0},   // 8 bits
    {PLACEHOLDER, "Rate Matched CQI Bits", 0},  // 16 bits

    {UINT, "Num DL Carriers", 4},   // 2 bits
    {PLACEHOLDER, "Ack Nack Index", 0}, // 12 bits
    {PLACEHOLDER, "Ack Nack Late", 0}, // 1 bit
    {PLACEHOLDER, "CSF Late", 0}, // 1 bit
    {PLACEHOLDER, "Drop PUSCH", 0}, // 1 bit

    {BYTE_STREAM, "CQI Payload", 44},

    {UINT, "Tx Resampler", 4},

    {UINT, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 4},  // 4 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},  // 4 bits
};


const Fmt LtePhyPuschTxReport_Payload_v145 [] = {
    {UINT, "Serving Cell ID", 2},    // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPuschTxReport_Record_v145 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "UL Carrier Index", 2}, // 2 bits
    {PLACEHOLDER, "ACK", 0},    // 1 bit
    {PLACEHOLDER, "CQI", 0},    // 1 bit
    {PLACEHOLDER, "RI", 0}, // 1 bit
    {PLACEHOLDER, "Frequency Hopping", 0},  // 2 bits
    {PLACEHOLDER, "Re-tx Index", 0},  // 5 bits
    {PLACEHOLDER, "Redund Ver", 0}, // 2 bits
    {PLACEHOLDER, "Mirror Hopping", 0}, // 2 bits

    {UINT, "Resource Allocation Type", 4},  // 1 bit
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "Num of RB", 0},  // 7 bits

    {PLACEHOLDER, "DL Carrier Index",0},//3 bits
    {PLACEHOLDER,"Enable UL DMRS OCC",0},   // shift 25 bits,total 1 bits

    {UINT, "PUSCH TB Size", 2},
    {UINT, "Coding Rate", 2},  // x/1024.0

    {UINT, "Rate Matched ACK Bits", 4}, // 14 bits

    {PLACEHOLDER, "RI Payload", 0},    // shift 14 then 14 bits
    {PLACEHOLDER, "Num RI Bits NRI", 0},   // shirt 14 then 4 bits

    //{PLACEHOLDER, "UE SRS", 0}, // 1 bit
    //{PLACEHOLDER, "SRS Occasion", 0},   // 1 bit

    {UINT, "ACK Payload", 16},

    {UINT,"ACK/NAK Inp Length 0",4},    // shift 0 bits,total 7 bits
    {PLACEHOLDER,"ACK/NAK Inp Length 1",0},     // shift 7 bits,total 7 bits
    {PLACEHOLDER,"Rate Matched RI Bits",0},     // shift 14 bits,total 11 bits
    {PLACEHOLDER,"Reserved",0},     // shift 25 bits,total 7 bits

    {UINT,"UE SRS",4},  // shift 0 bits,total 1 bits
    {PLACEHOLDER,"SRS Occasion",0},     // shift 1 bits,total 1 bits
    {PLACEHOLDER,"PUSCH Mod Order",0},  // shift 2 bits,total 3 bits
    {PLACEHOLDER,"PUSCH Digital Gain",0},   // shift 5 bits,total 8 bits
    {PLACEHOLDER,"Start RB Cluster1",0},    // shift 13 bits,total 7 bits
    {PLACEHOLDER,"Num RB Cluster1",0},  // shift 20 bits,total 7 bits

    {UINT, "PUSCH Tx Power (dBm)", 4},    // 7 bits (x - 128)
    {PLACEHOLDER, "Num CQI Bits", 0},   // 8 bits
    {PLACEHOLDER, "Rate Matched CQI Bits", 0},  // 16 bits

    {UINT, "Num DL Carriers", 4},   // 2 bits
    {PLACEHOLDER, "Ack Nack Index", 0}, // 12 bits
    {PLACEHOLDER, "Ack Nack Late", 0}, // 1 bit
    {PLACEHOLDER, "CSF Late", 0}, // 1 bit
    {PLACEHOLDER, "Drop PUSCH", 0}, // 1 bit

    {BYTE_STREAM, "CQI Payload", 44},

    {UINT, "Tx Resampler", 4},

    {UINT, "Cyclic Shift of DMRS Symbols Slot 0 (Samples)", 4},  // 4 bits
    {PLACEHOLDER, "Cyclic Shift of DMRS Symbols Slot 1 (Samples)", 0},  // 4 bits
};


static int _decode_lte_phy_pusch_tx_report_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    int temp = 0;
    unsigned int u_temp = 0;
    PyObject *old_object;
    PyObject *pyfloat;
    PyObject *pystr;

    switch (pkt_ver) {
    case 23:
        {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v23,
                    ARRAY_SIZE(LtePhyPuschTxReport_Payload_v23, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v23,
                        ARRAY_SIZE(LtePhyPuschTxReport_Record_v23, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "Coding Rate Data");
                float fCodingRateData = temp / 1024.0;
                pyfloat = Py_BuildValue("f", fCodingRateData);
                old_object = _replace_result(result_record_item,
                        "Coding Rate Data", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);

                u_temp = _search_result_uint(result_record_item, "ACK");
                int iAck = u_temp & 1;    // 1 bit
                int iCQI = (u_temp >> 1) & 1;   // 1 bit
                int iRI = (u_temp >> 2) & 1;    // 1 bit
                int iFrequencyHopping = (u_temp >> 3) & 3;  // 2 bits
                int iRedundVer = (u_temp >> 5) & 3; // 2 bits
                int iMirrorHopping = (u_temp >> 7) & 3; // 2 bits
                int iCSDSS0 = (u_temp >> 9) & 15;   // 4 bits
                int iCSDSS1 = (u_temp >> 13) & 15;  // 4 bits
                int iDMRSRootSlot0 = (u_temp >> 17) & 2047; // 11 bits
                int iUESRS = (u_temp >> 28) & 1;    // 1 bit
                old_object = _replace_result_int(result_record_item, "ACK",
                        iAck);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "ACK", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "CQI",
                        iCQI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "CQI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "RI",
                        iRI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "RI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Frequency Hopping", iFrequencyHopping);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Frequency Hopping", ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Redund Ver", iRedundVer);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Mirror Hopping", iMirrorHopping);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)",
                        iCSDSS0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 1 (Samples)",
                        iCSDSS1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 0", iDMRSRootSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");

                u_temp = _search_result_uint(result_record_item, "DMRS Root Slot 1");
                int iDMRSRootSlot1 = u_temp & 2047; // 11 bits
                int iStartRBSlot0 = (u_temp >> 11) & 127;   // 7 bits
                int iStartRBSlot1 = (u_temp >> 18) & 127;   // 7 bits
                int iNumOfRB = (u_temp >> 25) & 127;    // 7 bits
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 1", iDMRSRootSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num of RB", iNumOfRB);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "Num ACK Bits");
                int iNumACKBits = u_temp & 0x7;// 3 bit
                int iACKPayload = (u_temp>>3) & 0x15;// 4 bit
                old_object = _replace_result_int(result_record_item,
                        "Num ACK Bits", iNumACKBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "ACK Payload", iACKPayload);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "Rate Matched ACK Bits");
                int iRateMatchedACKBits = u_temp & 0x7ff;//11 bit
                int iNumRIBit = (u_temp>>11) & 0x3;//2 bit
                int iRIPayload = (u_temp>>13) & 0x3;//2 bit
                int iRateMatchedRIBits=(u_temp >> 15) & 0x7ff;//11 bit
                int iPUSCHModOrder = (u_temp >> 26) & 0x3;//2 bit
                int iRIPayload2 =(u_temp >>28) & 0x15;//4 bit

                old_object = _replace_result_int(result_record_item,
                        "Rate Matched ACK Bits", iRateMatchedACKBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num RI Bits NRI (bits)", iNumRIBit);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "RI Payload", iRIPayload);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched RI Bits", iRateMatchedRIBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Mod Order", iPUSCHModOrder);
                Py_DECREF(old_object);

                (void)_map_result_field_to_name(result_record_item,
                        "PUSCH Mod Order",
                        ValueNameModulation,
                        ARRAY_SIZE(ValueNameModulation, ValueName),
                        "(MI)Unknown");

                old_object = _replace_result_int(result_record_item,
                        "RI Payload2", iRIPayload2);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "SRS Occasion");
                int iSRSOccasion = u_temp & 0x1;// 1 bit
                int iRetxIndex = (u_temp >>1) & 0x1f;//5 bit
                old_object = _replace_result_int(result_record_item,
                        "SRS Occasion", iSRSOccasion);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Re-tx Index", iRetxIndex);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "PUSCH Tx Power (dBm)");
                int iPUSCHTxPower = u_temp & 0x3ff;// 10 bit
                int iNumCqiBits = (u_temp >>10) & 0xff;//8 bit
                int iRateMatchedCqiBits =(u_temp>>18) & 0x3fff;//14 bit

                old_object = _replace_result_int(result_record_item,
                        "PUSCH Tx Power (dBm)", iPUSCHTxPower);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num CQI Bits", iNumCqiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched CQI Bits", iRateMatchedCqiBits);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    case 24:
        {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v24,
                    ARRAY_SIZE(LtePhyPuschTxReport_Payload_v24, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v24,
                        ARRAY_SIZE(LtePhyPuschTxReport_Record_v24, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "Coding Rate Data");
                float fCodingRateData = temp / 1024.0;
                pyfloat = Py_BuildValue("f", fCodingRateData);
                old_object = _replace_result(result_record_item,
                        "Coding Rate Data", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);

                u_temp = _search_result_uint(result_record_item, "ACK");
                int iAck = u_temp & 1;    // 1 bit
                int iCQI = (u_temp >> 1) & 1;   // 1 bit
                int iRI = (u_temp >> 2) & 1;    // 1 bit
                int iFrequencyHopping = (u_temp >> 3) & 3;  // 2 bits
                int iRedundVer = (u_temp >> 5) & 3; // 2 bits
                int iMirrorHopping = (u_temp >> 7) & 3; // 2 bits
                int iCSDSS0 = (u_temp >> 9) & 15;   // 4 bits
                int iCSDSS1 = (u_temp >> 13) & 15;  // 4 bits
                int iDMRSRootSlot0 = (u_temp >> 17) & 2047; // 11 bits
                int iUESRS = (u_temp >> 28) & 1;    // 1 bit
                old_object = _replace_result_int(result_record_item, "ACK",
                        iAck);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "ACK", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "CQI",
                        iCQI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "CQI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "RI",
                        iRI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "RI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Frequency Hopping", iFrequencyHopping);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Frequency Hopping", ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Redund Ver", iRedundVer);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Mirror Hopping", iMirrorHopping);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)",
                        iCSDSS0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 1 (Samples)",
                        iCSDSS1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 0", iDMRSRootSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");

                u_temp = _search_result_uint(result_record_item, "DMRS Root Slot 1");
                int iDMRSRootSlot1 = u_temp & 2047; // 11 bits
                int iStartRBSlot0 = (u_temp >> 11) & 127;   // 7 bits
                int iStartRBSlot1 = (u_temp >> 18) & 127;   // 7 bits
                int iNumOfRB = (u_temp >> 25) & 127;    // 7 bits
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 1", iDMRSRootSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num of RB", iNumOfRB);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "Num ACK Bits");
                int iNumACKBits = u_temp & 0x7;// 3 bit
                int iACKPayload = (u_temp>>3) & 0x15;// 4 bit
                old_object = _replace_result_int(result_record_item,
                        "Num ACK Bits", iNumACKBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "ACK Payload", iACKPayload);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "Rate Matched ACK Bits");
                int iRateMatchedACKBits = u_temp & 0x7ff;//11 bit
                int iNumRIBit = (u_temp>>11) & 0x3;//2 bit
                int iRIPayload = (u_temp>>13) & 0x3;//2 bit
                int iRateMatchedRIBits=(u_temp >> 15) & 0x7ff;//11 bit
                int iPUSCHModOrder = (u_temp >> 26) & 0x3;//2 bit
                int iRIPayload2 =(u_temp >>28) & 0x15;//4 bit

                old_object = _replace_result_int(result_record_item,
                        "Rate Matched ACK Bits", iRateMatchedACKBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num RI Bits NRI (bits)", iNumRIBit);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "RI Payload", iRIPayload);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched RI Bits", iRateMatchedRIBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Mod Order", iPUSCHModOrder);
                Py_DECREF(old_object);

                (void)_map_result_field_to_name(result_record_item,
                        "PUSCH Mod Order",
                        ValueNameModulation,
                        ARRAY_SIZE(ValueNameModulation, ValueName),
                        "(MI)Unknown");

                old_object = _replace_result_int(result_record_item,
                        "RI Payload2", iRIPayload2);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "SRS Occasion");
                int iSRSOccasion = u_temp & 0x1;// 1 bit
                int iRetxIndex = (u_temp >>1) & 0x1f;//5 bit
                old_object = _replace_result_int(result_record_item,
                        "SRS Occasion", iSRSOccasion);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Re-tx Index", iRetxIndex);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "PUSCH Tx Power (dBm)");
                int iPUSCHTxPower = u_temp & 0x3ff;// 10 bit
                int iNumCqiBits = (u_temp >>10) & 0xff;//8 bit
                int iRateMatchedCqiBits =(u_temp>>18) & 0x3fff;//14 bit

                old_object = _replace_result_int(result_record_item,
                        "PUSCH Tx Power (dBm)", iPUSCHTxPower);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num CQI Bits", iNumCqiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched CQI Bits", iRateMatchedCqiBits);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    case 26:
        {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v26,
                    ARRAY_SIZE(LtePhyPuschTxReport_Payload_v26, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v26,
                        ARRAY_SIZE(LtePhyPuschTxReport_Record_v26, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "Coding Rate Data");
                float fCodingRateData = temp / 1024.0;
                pyfloat = Py_BuildValue("f", fCodingRateData);
                old_object = _replace_result(result_record_item,
                        "Coding Rate Data", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);

                u_temp = _search_result_uint(result_record_item, "ACK");
                int iAck = u_temp & 1;    // 1 bit
                int iCQI = (u_temp >> 1) & 1;   // 1 bit
                int iRI = (u_temp >> 2) & 1;    // 1 bit
                int iFrequencyHopping = (u_temp >> 3) & 3;  // 2 bits
                int iRedundVer = (u_temp >> 5) & 3; // 2 bits
                int iMirrorHopping = (u_temp >> 7) & 3; // 2 bits
                int iCSDSS0 = (u_temp >> 9) & 15;   // 4 bits
                int iCSDSS1 = (u_temp >> 13) & 15;  // 4 bits
                int iDMRSRootSlot0 = (u_temp >> 17) & 2047; // 11 bits
                int iUESRS = (u_temp >> 28) & 1;    // 1 bit
                old_object = _replace_result_int(result_record_item, "ACK",
                        iAck);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "ACK", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "CQI",
                        iCQI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "CQI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "RI",
                        iRI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "RI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Frequency Hopping", iFrequencyHopping);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Frequency Hopping", ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Redund Ver", iRedundVer);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Mirror Hopping", iMirrorHopping);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)",
                        iCSDSS0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 1 (Samples)",
                        iCSDSS1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 0", iDMRSRootSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");

                u_temp = _search_result_uint(result_record_item, "DMRS Root Slot 1");
                int iDMRSRootSlot1 = u_temp & 2047; // 11 bits
                int iStartRBSlot0 = (u_temp >> 11) & 127;   // 7 bits
                int iStartRBSlot1 = (u_temp >> 18) & 127;   // 7 bits
                int iNumOfRB = (u_temp >> 25) & 127;    // 7 bits
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 1", iDMRSRootSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num of RB", iNumOfRB);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "Num ACK Bits");
                int iNumACKBits = u_temp & 0x7;// 3 bit
                int iACKPayload = (u_temp>>3) & 0x15;// 4 bit
                old_object = _replace_result_int(result_record_item,
                        "Num ACK Bits", iNumACKBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "ACK Payload", iACKPayload);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "Rate Matched ACK Bits");
                int iRateMatchedACKBits = u_temp & 0x7ff;//11 bit
                int iNumRIBit = (u_temp>>11) & 0x3;//2 bit
                int iRIPayload = (u_temp>>13) & 0x3;//2 bit
                int iRateMatchedRIBits=(u_temp >> 15) & 0x7ff;//11 bit
                int iPUSCHModOrder = (u_temp >> 26) & 0x3;//2 bit
                int iRIPayload2 =(u_temp >>28) & 0x15;//4 bit

                old_object = _replace_result_int(result_record_item,
                        "Rate Matched ACK Bits", iRateMatchedACKBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num RI Bits NRI (bits)", iNumRIBit);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "RI Payload", iRIPayload);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched RI Bits", iRateMatchedRIBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Mod Order", iPUSCHModOrder);
                Py_DECREF(old_object);

                (void)_map_result_field_to_name(result_record_item,
                        "PUSCH Mod Order",
                        ValueNameModulation,
                        ARRAY_SIZE(ValueNameModulation, ValueName),
                        "(MI)Unknown");

                old_object = _replace_result_int(result_record_item,
                        "RI Payload2", iRIPayload2);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "SRS Occasion");
                int iSRSOccasion = u_temp & 0x1;// 1 bit
                int iRetxIndex = (u_temp >>1) & 0x1f;//5 bit
                old_object = _replace_result_int(result_record_item,
                        "SRS Occasion", iSRSOccasion);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Re-tx Index", iRetxIndex);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "PUSCH Tx Power (dBm)");
                int iPUSCHTxPower = u_temp & 0x3ff;// 10 bit
                int iNumCqiBits = (u_temp >>10) & 0xff;//8 bit
                int iRateMatchedCqiBits =(u_temp>>18) & 0x3fff;//14 bit

                old_object = _replace_result_int(result_record_item,
                        "PUSCH Tx Power (dBm)", iPUSCHTxPower);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num CQI Bits", iNumCqiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched CQI Bits", iRateMatchedCqiBits);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Num Repetition");
                int iNumRepetition= u_temp & 0xfff;//12 bit
                int iRBNBStartIndex = (u_temp>>12) & 0xff ;//8 bit
                old_object = _replace_result_int(result_record_item,
                        "Num Repetition", iNumRepetition);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "RB NB Start Index", iRBNBStartIndex);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    case 43:
        {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v43,
                    ARRAY_SIZE(LtePhyPuschTxReport_Payload_v43, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v43,
                        ARRAY_SIZE(LtePhyPuschTxReport_Record_v43, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "Coding Rate Data");
                float fCodingRateData = temp / 1024.0;
                pyfloat = Py_BuildValue("f", fCodingRateData);
                old_object = _replace_result(result_record_item,
                        "Coding Rate Data", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);

                u_temp = _search_result_uint(result_record_item, "ACK");
                int iAck = u_temp & 1;    // 1 bit
                int iCQI = (u_temp >> 1) & 1;   // 1 bit
                int iRI = (u_temp >> 2) & 1;    // 1 bit
                int iFrequencyHopping = (u_temp >> 3) & 3;  // 2 bits
                int iRedundVer = (u_temp >> 5) & 3; // 2 bits
                int iMirrorHopping = (u_temp >> 7) & 3; // 2 bits
                int iCSDSS0 = (u_temp >> 9) & 15;   // 4 bits
                int iCSDSS1 = (u_temp >> 13) & 15;  // 4 bits
                int iDMRSRootSlot0 = (u_temp >> 17) & 2047; // 11 bits
                int iUESRS = (u_temp >> 28) & 1;    // 1 bit
                old_object = _replace_result_int(result_record_item, "ACK",
                        iAck);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "ACK", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "CQI",
                        iCQI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "CQI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "RI",
                        iRI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "RI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Frequency Hopping", iFrequencyHopping);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Frequency Hopping", ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Redund Ver", iRedundVer);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Mirror Hopping", iMirrorHopping);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)",
                        iCSDSS0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 1 (Samples)",
                        iCSDSS1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 0", iDMRSRootSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");

                u_temp = _search_result_uint(result_record_item, "DMRS Root Slot 1");
                int iDMRSRootSlot1 = u_temp & 2047; // 11 bits
                int iStartRBSlot0 = (u_temp >> 11) & 127;   // 7 bits
                int iStartRBSlot1 = (u_temp >> 18) & 127;   // 7 bits
                int iNumOfRB = (u_temp >> 25) & 127;    // 7 bits
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 1", iDMRSRootSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num of RB", iNumOfRB);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "ACK Payload");
                char hex[10] = {};
                std::string strAckPayload = "0x";
                sprintf(hex, "%02x", u_temp & 0xFF);
                strAckPayload += hex;
                sprintf(hex, "%02x", (u_temp >> 8) & 0xFF);
                strAckPayload += hex;
                sprintf(hex, "%02x", (u_temp >> 16) & 0xF);
                strAckPayload += hex;
                pystr = Py_BuildValue("s", strAckPayload.c_str());
                old_object = _replace_result(result_record_item, "ACK Payload", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                int iAckNakInpLength0 = (u_temp >> 20) & 15;
                int iAckNakInpLength1 = (u_temp >> 24) & 15;
                int iNumRiBitsNri = (u_temp >> 29) & 7;
                old_object = _replace_result_int(result_record_item,
                        "ACK/NAK Inp Length 0", iAckNakInpLength0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "ACK/NAK Inp Length 1", iAckNakInpLength1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num RI Bits NRI (bits)", iNumRiBitsNri);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "RI Payload");
                // int iRiPayload = u_temp & 15;   // 4 bits
                int iRateMatchedRiBits = (u_temp >> 4) & 2047;  // 11 bits
                int iPuschModOrder = (u_temp >> 15) & 3;    // 2 bits
                int iPuschDigitalGain = (u_temp >> 17) & 255;   // 8 bits
                int iSrsOccasion = (u_temp >> 25) & 1;  // 1 bit
                int iRetxIndex = (u_temp >> 26) & 7;    // 3 bits
                std::string strRiPayload = "0x";
                sprintf(hex, "%02x", u_temp & 0xF);
                strRiPayload += hex;
                pystr = Py_BuildValue("s", strRiPayload.c_str());
                old_object = _replace_result(result_record_item, "RI Payload", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched RI Bits", iRateMatchedRiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Mod Order", iPuschModOrder);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "PUSCH Mod Order",
                        ValueNameModulation,
                        ARRAY_SIZE(ValueNameModulation, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Digital Gain (dB)", iPuschDigitalGain);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "SRS Occasion", iSrsOccasion);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "SRS Occasion",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Re-tx Index", iRetxIndex);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "Re-tx Index",
                        ValueNameNumber,
                        ARRAY_SIZE(ValueNameNumber, ValueName),
                        "(MI)Unknown");

                u_temp = _search_result_uint(result_record_item,
                        "PUSCH Tx Power (dBm)");
                int iPuschTxPower = u_temp & 127;   // 7 bits
                int iNumCqiBits = (u_temp >> 7) & 255;  // 8 bits
                int iRateMatchedCqiBits = (u_temp >> 15) & 65535;   // 16 bits
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Tx Power (dBm)", iPuschTxPower);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num CQI Bits", iNumCqiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched CQI Bits", iRateMatchedCqiBits);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    case 102:
        {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v102,
                    ARRAY_SIZE(LtePhyPuschTxReport_Payload_v102, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v102,
                        ARRAY_SIZE(LtePhyPuschTxReport_Record_v102, Fmt),
                        b, offset, length, result_record_item);
                u_temp = _search_result_uint(result_record_item, "Carrier Index");
                int iCarrierIndex = u_temp & 3;  // 2 bits
                int iAck = (u_temp >> 2) & 1;    // 1 bit
                int iCQI = (u_temp >> 3) & 1;   // 1 bit
                int iRI = (u_temp >> 4) & 1;    // 1 bit
                int iFrequencyHopping = (u_temp >> 5) & 3;  // 2 bits
                int iRetxIndex = (u_temp >> 7) & 31;    // 5 bits
                int iRedundVer = (u_temp >> 12) & 3; // 2 bits
                int iMirrorHopping = (u_temp >> 14) & 3; // 2 bits
                old_object = _replace_result_int(result_record_item,
                        "Carrier Index", iCarrierIndex);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Carrier Index", ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "ACK",
                        iAck);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "ACK", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "CQI",
                        iCQI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "CQI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "RI",
                        iRI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "RI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Frequency Hopping", iFrequencyHopping);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Frequency Hopping", ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Re-tx Index", iRetxIndex);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "Re-tx Index",
                        ValueNameNumber,
                        ARRAY_SIZE(ValueNameNumber, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Redund Ver", iRedundVer);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Mirror Hopping", iMirrorHopping);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Resource Allocation Type");
                int iResourceAllocationType = u_temp & 1;   // 1 bit
                int iStartRBSlot0 = (u_temp >> 1) & 127;   // 7 bits
                int iStartRBSlot1 = (u_temp >> 8) & 127;   // 7 bits
                int iNumOfRB = (u_temp >> 15) & 127;    // 7 bits
                old_object = _replace_result_int(result_record_item,
                        "Resource Allocation Type", iResourceAllocationType);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num of RB", iNumOfRB);
                Py_DECREF(old_object);

                temp = _search_result_int(result_record_item, "Coding Rate");
                float fCodingRate = temp / 1024.0;
                pyfloat = Py_BuildValue("f", fCodingRate);
                old_object = _replace_result(result_record_item,
                        "Coding Rate", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);

                u_temp = _search_result_uint(result_record_item,
                        "Rate Matched ACK Bits");
                int iRateMatchedAckBits = u_temp & 16383;   // 14 bits
                int iRiPayload = (u_temp >> 14) & 15;   // 4 bits
                int iRateMatchedRiBits = (u_temp >> 18) & 2047;  // 11 bits
                int iUESRS = (u_temp >> 29) & 1;    // 1 bit
                int iSrsOccasion = (u_temp >> 30) & 1;  // 1 bit
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched ACK Bits", iRateMatchedAckBits);
                Py_DECREF(old_object);
                char hex[10] = {};
                std::string strRiPayload = "0x";
                sprintf(hex, "%02x", iRiPayload);
                strRiPayload += hex;
                pystr = Py_BuildValue("s", strRiPayload.c_str());
                old_object = _replace_result(result_record_item, "RI Payload", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched RI Bits", iRateMatchedRiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "SRS Occasion", iSrsOccasion);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "SRS Occasion",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");

                u_temp = _search_result_uint(result_record_item, "ACK Payload");
                std::string strAckPayload = "0x";
                sprintf(hex, "%02x", u_temp & 0xFF);
                strAckPayload += hex;
                sprintf(hex, "%02x", (u_temp >> 8) & 0xFF);
                strAckPayload += hex;
                sprintf(hex, "%02x", (u_temp >> 16) & 0xF);
                strAckPayload += hex;
                pystr = Py_BuildValue("s", strAckPayload.c_str());
                old_object = _replace_result(result_record_item, "ACK Payload", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                int iAckNakInpLength0 = (u_temp >> 20) & 15;
                int iAckNakInpLength1 = (u_temp >> 24) & 15;
                int iNumRiBitsNri = (u_temp >> 28) & 7;
                old_object = _replace_result_int(result_record_item,
                        "ACK/NAK Inp Length 0", iAckNakInpLength0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "ACK/NAK Inp Length 1", iAckNakInpLength1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num RI Bits NRI (bits)", iNumRiBitsNri);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "PUSCH Mod Order");
                int iPuschModOrder = u_temp & 3;    // 2 bits
                int iPuschDigitalGain = (u_temp >> 2) & 255;    // 8 bits
                int iStartRBCluster1 = (u_temp >> 10) & 255;    // 8 bits
                int iNumRBCluster1 = (u_temp >> 18) * 63;   // 6 bits
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Mod Order", iPuschModOrder);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "PUSCH Mod Order",
                        ValueNameModulation,
                        ARRAY_SIZE(ValueNameModulation, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Digital Gain (dB)", iPuschDigitalGain);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Cluster1", iStartRBCluster1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num RB Cluster1", iNumRBCluster1);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "PUSCH Tx Power (dBm)");
                int iPuschTxPower = (u_temp & 127) - 128;   // 7 bits
                int iNumCqiBits = (u_temp >> 7) & 255;  // 8 bits
                int iRateMatchedCqiBits = (u_temp >> 15) & 65535;   // 16 bits
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Tx Power (dBm)", iPuschTxPower);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num CQI Bits", iNumCqiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched CQI Bits", iRateMatchedCqiBits);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Num DL Carriers");
                int iNumDLCarriers = u_temp & 3;    // 2 bits
                int iAckNackIndex = (u_temp >> 2) & 4096;   // 12 bits
                int iAckNackLate = (u_temp >> 14) & 1;  // 1 bit
                int iCSFLate = (u_temp >> 15) & 1;  // 1 bit
                int iDropPusch = (u_temp >> 16) & 1;  // 1 bit
                old_object = _replace_result_int(result_record_item,
                        "Num DL Carriers", iNumDLCarriers);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nack Index", iAckNackIndex);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nack Late", iAckNackLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "CSF Late", iCSFLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Drop PUSCH", iDropPusch);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)");
                int iCSDSS0 = u_temp & 15;   // 4 bits
                int iCSDSS1 = (u_temp >> 4) & 15;  // 4 bits
                int iDMRSRootSlot0 = (u_temp >> 8) & 2047; // 11 bits
                int iDMRSRootSlot1 = (u_temp >> 19) & 2047; // 11 bits
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)",
                        iCSDSS0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 1 (Samples)",
                        iCSDSS1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 0", iDMRSRootSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 1", iDMRSRootSlot1);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
   case 122:
        {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v102,
                    ARRAY_SIZE(LtePhyPuschTxReport_Payload_v102, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v122,
                        ARRAY_SIZE(LtePhyPuschTxReport_Record_v122, Fmt),
                        b, offset, length, result_record_item);
                u_temp = _search_result_uint(result_record_item, "UL Carrier Index");
                int iCarrierIndex = u_temp & 3;  // 2 bits
                int iAck = (u_temp >> 2) & 1;    // 1 bit
                int iCQI = (u_temp >> 3) & 1;   // 1 bit
                int iRI = (u_temp >> 4) & 1;    // 1 bit
                int iFrequencyHopping = (u_temp >> 5) & 3;  // 2 bits
                int iRetxIndex = (u_temp >> 7) & 31;    // 5 bits
                int iRedundVer = (u_temp >> 12) & 3; // 2 bits
                int iMirrorHopping = (u_temp >> 14) & 3; // 2 bits
                old_object = _replace_result_int(result_record_item,
                        "UL Carrier Index", iCarrierIndex);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "UL Carrier Index", ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "ACK",
                        iAck);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "ACK", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "CQI",
                        iCQI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "CQI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "RI",
                        iRI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "RI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Frequency Hopping", iFrequencyHopping);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Frequency Hopping", ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Re-tx Index", iRetxIndex);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "Re-tx Index",
                        ValueNameNumber,
                        ARRAY_SIZE(ValueNameNumber, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Redund Ver", iRedundVer);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Mirror Hopping", iMirrorHopping);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Start RB Slot 0");
                int iStartRBSlot0 = (u_temp) & 127;   // 7 bits
                int iStartRBSlot1 = (u_temp >> 7) & 127;   // 7 bits
                int iNumOfRB = (u_temp >> 14) & 127;    // 7 bits
                int iDlCarrierIndex=(u_temp >>21) & 7;//3 bits

                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num of RB", iNumOfRB);
                Py_DECREF(old_object);

                old_object = _replace_result_int(result_record_item,
                        "DL Carrier Index", iDlCarrierIndex);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "DL Carrier Index", ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");

                temp = _search_result_int(result_record_item, "Coding Rate");
                float fCodingRate = temp / 1024.0;
                pyfloat = Py_BuildValue("f", fCodingRate);
                old_object = _replace_result(result_record_item,
                        "Coding Rate", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);

                u_temp = _search_result_uint(result_record_item,
                        "Rate Matched ACK Bits");
                int iRateMatchedAckBits = u_temp & 16383;   // 14 bits
                int iRiPayload = (u_temp >> 14) & 15;   // 4 bits
                int iRateMatchedRiBits = (u_temp >> 19) & 2047;  // 11 bits
                int iUESRS = (u_temp >> 29) & 1;    // 1 bit
                int iSrsOccasion = (u_temp >> 30) & 1;  // 1 bit
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched ACK Bits", iRateMatchedAckBits);
                Py_DECREF(old_object);
                char hex[10] = {};
                std::string strRiPayload = "0x";
                sprintf(hex, "%02x", iRiPayload);
                strRiPayload += hex;
                pystr = Py_BuildValue("s", strRiPayload.c_str());
                old_object = _replace_result(result_record_item, "RI Payload", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched RI Bits", iRateMatchedRiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "SRS Occasion", iSrsOccasion);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "SRS Occasion",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");

                u_temp = _search_result_uint(result_record_item, "ACK Payload");
                std::string strAckPayload = "0x";
                sprintf(hex, "%02x", u_temp & 0xFF);
                strAckPayload += hex;
                sprintf(hex, "%02x", (u_temp >> 8) & 0xFF);
                strAckPayload += hex;
                sprintf(hex, "%02x", (u_temp >> 16) & 0xF);
                strAckPayload += hex;
                pystr = Py_BuildValue("s", strAckPayload.c_str());
                old_object = _replace_result(result_record_item, "ACK Payload", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                int iAckNakInpLength0 = (u_temp >> 20) & 15;
                int iAckNakInpLength1 = (u_temp >> 24) & 15;
                int iNumRiBitsNri = (u_temp >> 28) & 7;
                old_object = _replace_result_int(result_record_item,
                        "ACK/NAK Inp Length 0", iAckNakInpLength0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "ACK/NAK Inp Length 1", iAckNakInpLength1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num RI Bits NRI (bits)", iNumRiBitsNri);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "PUSCH Mod Order");
                int iPuschModOrder = u_temp & 3;    // 2 bits
                int iPuschDigitalGain = (u_temp >> 2) & 255;    // 8 bits
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Mod Order", iPuschModOrder);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "PUSCH Mod Order",
                        ValueNameModulation,
                        ARRAY_SIZE(ValueNameModulation, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Digital Gain (dB)", iPuschDigitalGain);
                Py_DECREF(old_object);


                u_temp = _search_result_uint(result_record_item,
                        "PUSCH Tx Power (dBm)");
                int iPuschTxPower = u_temp & 127;   // 7 bits
                int iNumCqiBits = (u_temp >> 7) & 255;  // 8 bits
                int iRateMatchedCqiBits = (u_temp >> 18) & 16383;   // 16 bits
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Tx Power (dBm)", iPuschTxPower);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num CQI Bits", iNumCqiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched CQI Bits", iRateMatchedCqiBits);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Num DL Carriers");
                int iNumDLCarriers = u_temp & 3;    // 2 bits
                int iAckNackIndex = (u_temp >> 2) & 4096;   // 12 bits
                int iAckNackLate = (u_temp >> 14) & 1;  // 1 bit
                int iCSFLate = (u_temp >> 15) & 1;  // 1 bit
                int iDropPusch = (u_temp >> 16) & 1;  // 1 bit
                old_object = _replace_result_int(result_record_item,
                        "Num DL Carriers", iNumDLCarriers);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nack Index", iAckNackIndex);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nack Late", iAckNackLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "CSF Late", iCSFLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Drop PUSCH", iDropPusch);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)");
                int iCSDSS0 = u_temp & 15;   // 4 bits
                int iCSDSS1 = (u_temp >> 4) & 15;  // 4 bits
                int iDMRSRootSlot0 = (u_temp >> 8) & 2047; // 11 bits
                int iDMRSRootSlot1 = (u_temp >> 19) & 2047; // 11 bits
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)",
                        iCSDSS0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 1 (Samples)",
                        iCSDSS1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 0", iDMRSRootSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 1", iDMRSRootSlot1);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
   case 124:
        {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v102,
                    ARRAY_SIZE(LtePhyPuschTxReport_Payload_v102, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v124,
                        ARRAY_SIZE(LtePhyPuschTxReport_Record_v124, Fmt),
                        b, offset, length, result_record_item);
                u_temp = _search_result_uint(result_record_item, "UL Carrier Index");
                int iCarrierIndex = u_temp & 3;  // 2 bits
                int iAck = (u_temp >> 2) & 1;    // 1 bit
                int iCQI = (u_temp >> 3) & 1;   // 1 bit
                int iRI = (u_temp >> 4) & 1;    // 1 bit
                int iFrequencyHopping = (u_temp >> 5) & 3;  // 2 bits
                int iRetxIndex = (u_temp >> 7) & 31;    // 5 bits
                int iRedundVer = (u_temp >> 12) & 3; // 2 bits
                int iMirrorHopping = (u_temp >> 14) & 3; // 2 bits
                old_object = _replace_result_int(result_record_item,
                        "UL Carrier Index", iCarrierIndex);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "UL Carrier Index", ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "ACK",
                        iAck);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "ACK", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "CQI",
                        iCQI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "CQI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "RI",
                        iRI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "RI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Frequency Hopping", iFrequencyHopping);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Frequency Hopping", ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Re-tx Index", iRetxIndex);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "Re-tx Index",
                        ValueNameNumber,
                        ARRAY_SIZE(ValueNameNumber, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Redund Ver", iRedundVer);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Mirror Hopping", iMirrorHopping);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Resource Allocation Type");
                int iResourceAllocationType = u_temp & 1;   // 1 bit
                int iStartRBSlot0 = (u_temp >> 1) & 127;   // 7 bits
                int iStartRBSlot1 = (u_temp >> 8) & 127;   // 7 bits
                int iNumOfRB = (u_temp >> 15) & 127;    // 7 bits
                int iDlCarrierIndex=(u_temp >>25) &3;//2 bits

                old_object = _replace_result_int(result_record_item,
                        "Resource Allocation Type", iResourceAllocationType);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num of RB", iNumOfRB);
                Py_DECREF(old_object);

                old_object = _replace_result_int(result_record_item,
                        "DL Carrier Index", iDlCarrierIndex);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "DL Carrier Index", ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");

                temp = _search_result_int(result_record_item, "Coding Rate");
                float fCodingRate = temp / 1024.0;
                pyfloat = Py_BuildValue("f", fCodingRate);
                old_object = _replace_result(result_record_item,
                        "Coding Rate", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);

                u_temp = _search_result_uint(result_record_item,
                        "Rate Matched ACK Bits");
                int iRateMatchedAckBits = u_temp & 16383;   // 14 bits
                int iRiPayload = (u_temp >> 14) & 15;   // 4 bits
                int iRateMatchedRiBits = (u_temp >> 19) & 2047;  // 11 bits
                int iUESRS = (u_temp >> 29) & 1;    // 1 bit
                int iSrsOccasion = (u_temp >> 30) & 1;  // 1 bit
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched ACK Bits", iRateMatchedAckBits);
                Py_DECREF(old_object);
                char hex[10] = {};
                std::string strRiPayload = "0x";
                sprintf(hex, "%02x", iRiPayload);
                strRiPayload += hex;
                pystr = Py_BuildValue("s", strRiPayload.c_str());
                old_object = _replace_result(result_record_item, "RI Payload", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched RI Bits", iRateMatchedRiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "SRS Occasion", iSrsOccasion);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "SRS Occasion",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");

                u_temp = _search_result_uint(result_record_item, "ACK Payload");
                std::string strAckPayload = "0x";
                sprintf(hex, "%02x", u_temp & 0xFF);
                strAckPayload += hex;
                sprintf(hex, "%02x", (u_temp >> 8) & 0xFF);
                strAckPayload += hex;
                sprintf(hex, "%02x", (u_temp >> 16) & 0xF);
                strAckPayload += hex;
                pystr = Py_BuildValue("s", strAckPayload.c_str());
                old_object = _replace_result(result_record_item, "ACK Payload", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                int iAckNakInpLength0 = (u_temp >> 20) & 15;
                int iAckNakInpLength1 = (u_temp >> 24) & 15;
                int iNumRiBitsNri = (u_temp >> 28) & 7;
                old_object = _replace_result_int(result_record_item,
                        "ACK/NAK Inp Length 0", iAckNakInpLength0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "ACK/NAK Inp Length 1", iAckNakInpLength1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num RI Bits NRI (bits)", iNumRiBitsNri);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "PUSCH Mod Order");
                int iPuschModOrder = u_temp & 3;    // 2 bits
                int iPuschDigitalGain = (u_temp >> 2) & 255;    // 8 bits
                int iStartRBCluster1 = (u_temp >> 10) & 255;    // 8 bits
                int iNumRBCluster1 = (u_temp >> 18) * 63;   // 6 bits
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Mod Order", iPuschModOrder);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "PUSCH Mod Order",
                        ValueNameModulation,
                        ARRAY_SIZE(ValueNameModulation, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Digital Gain (dB)", iPuschDigitalGain);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Cluster1", iStartRBCluster1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num RB Cluster1", iNumRBCluster1);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "PUSCH Tx Power (dBm)");
                int iPuschTxPower = u_temp & 127;   // 7 bits
                int iNumCqiBits = (u_temp >> 7) & 255;  // 8 bits
                int iRateMatchedCqiBits = (u_temp >> 18) & 16383;   // 16 bits
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Tx Power (dBm)", iPuschTxPower);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num CQI Bits", iNumCqiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched CQI Bits", iRateMatchedCqiBits);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Num DL Carriers");
                int iNumDLCarriers = u_temp & 3;    // 2 bits
                int iAckNackIndex = (u_temp >> 2) & 4096;   // 12 bits
                int iAckNackLate = (u_temp >> 14) & 1;  // 1 bit
                int iCSFLate = (u_temp >> 15) & 1;  // 1 bit
                int iDropPusch = (u_temp >> 16) & 1;  // 1 bit
                old_object = _replace_result_int(result_record_item,
                        "Num DL Carriers", iNumDLCarriers);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nack Index", iAckNackIndex);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nack Late", iAckNackLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "CSF Late", iCSFLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Drop PUSCH", iDropPusch);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)");
                int iCSDSS0 = u_temp & 15;   // 4 bits
                int iCSDSS1 = (u_temp >> 4) & 15;  // 4 bits
                int iDMRSRootSlot0 = (u_temp >> 8) & 2047; // 11 bits
                int iDMRSRootSlot1 = (u_temp >> 19) & 2047; // 11 bits
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)",
                        iCSDSS0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 1 (Samples)",
                        iCSDSS1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 0", iDMRSRootSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Root Slot 1", iDMRSRootSlot1);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
   case 144:
   case 145:
        {
            offset += _decode_by_fmt(LtePhyPuschTxReport_Payload_v144,
                    ARRAY_SIZE(LtePhyPuschTxReport_Payload_v144, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPuschTxReport_Record_v144,
                        ARRAY_SIZE(LtePhyPuschTxReport_Record_v144, Fmt),
                        b, offset, length, result_record_item);
                u_temp = _search_result_uint(result_record_item, "UL Carrier Index");
                int iCarrierIndex = u_temp & 3;  // 2 bits
                int iAck = (u_temp >> 2) & 1;    // 1 bit
                int iCQI = (u_temp >> 3) & 1;   // 1 bit
                int iRI = (u_temp >> 4) & 1;    // 1 bit
                int iFrequencyHopping = (u_temp >> 5) & 3;  // 2 bits
                int iRetxIndex = (u_temp >> 7) & 31;    // 5 bits
                int iRedundVer = (u_temp >> 12) & 3; // 2 bits
                int iMirrorHopping = (u_temp >> 14) & 3; // 2 bits
                old_object = _replace_result_int(result_record_item,
                        "UL Carrier Index", iCarrierIndex);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "UL Carrier Index", ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "ACK",
                        iAck);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "ACK", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "CQI",
                        iCQI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "CQI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "RI",
                        iRI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "RI", ValueNameExistsOrNone,
                        ARRAY_SIZE(ValueNameExistsOrNone, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Frequency Hopping", iFrequencyHopping);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Frequency Hopping", ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Re-tx Index", iRetxIndex);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "Re-tx Index",
                        ValueNameNumber,
                        ARRAY_SIZE(ValueNameNumber, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Redund Ver", iRedundVer);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Mirror Hopping", iMirrorHopping);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Resource Allocation Type");
                int iResourceAllocationType = u_temp & 1;   // 1 bit
                int iStartRBSlot0 = (u_temp >> 1) & 127;   // 7 bits
                int iStartRBSlot1 = (u_temp >> 8) & 127;   // 7 bits
                int iNumOfRB = (u_temp >> 15) & 127;    // 7 bits
                int iDlCarrierIndex=(u_temp >>22) &7;//3 bits
                int iEnableULDMRSOCC = (u_temp>>25) &0x1;

                old_object = _replace_result_int(result_record_item,
                        "Resource Allocation Type", iResourceAllocationType);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num of RB", iNumOfRB);
                Py_DECREF(old_object);

                old_object = _replace_result_int(result_record_item,
                        "DL Carrier Index", iDlCarrierIndex);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "DL Carrier Index", ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Enable UL DMRS OCC", iEnableULDMRSOCC);
                Py_DECREF(old_object);

                temp = _search_result_int(result_record_item, "Coding Rate");
                float fCodingRate = temp / 1024.0;
                pyfloat = Py_BuildValue("f", fCodingRate);
                old_object = _replace_result(result_record_item,
                        "Coding Rate", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);

                u_temp = _search_result_uint(result_record_item,
                        "Rate Matched ACK Bits");
                int iRateMatchedAckBits = u_temp & 16383;   // 14 bits
                int iRiPayload = (u_temp >> 14) & 16383;   // 14 bits
                int iNumRIBitsNRI = (u_temp >> 28) & 0xf;  // 4 bits

                old_object = _replace_result_int(result_record_item,
                        "Rate Matched ACK Bits", iRateMatchedAckBits);
                Py_DECREF(old_object);

                char hex[10] = {};
                std::string strRiPayload = "0x";
                sprintf(hex, "%02x", iRiPayload);
                strRiPayload += hex;
                pystr = Py_BuildValue("s", strRiPayload.c_str());
                old_object = _replace_result(result_record_item, "RI Payload", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);

                old_object = _replace_result_int(result_record_item,
                        "Num RI Bits NRI", iNumRIBitsNRI);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "ACK/NAK Inp Length 0");
                int iAckNakInpLength0 = (u_temp >> 0) & 0x7f;
                int iAckNakInpLength1 = (u_temp >> 7) & 0x7f;
                int iRateMatchedRIBits = (u_temp >> 14) & 0x7ff;

                old_object = _replace_result_int(result_record_item,
                        "ACK/NAK Inp Length 0", iAckNakInpLength0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "ACK/NAK Inp Length 1", iAckNakInpLength1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched RI Bits", iRateMatchedRIBits);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "UE SRS");
                int iUESRS = (u_temp >>0) & 0x1;
                int iSRSOccasion = (u_temp >>1) & 0x1;
                int iPuschModOrder = (u_temp>>2) & 0x7;    // 3 bits
                int iPuschDigitalGain = (u_temp >> 5) & 255;    // 8 bits
                int iStartRBCluster1 = (u_temp >> 13) & 0x7f;    // 7 bits
                int iNumRBCluster1 = (u_temp >> 20) * 0x7f;   // 7 bits

                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "SRS Occasion", iSRSOccasion);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "SRS Occasion",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");

                old_object = _replace_result_int(result_record_item,
                        "PUSCH Mod Order", iPuschModOrder);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "PUSCH Mod Order",
                        ValueNameModulation,
                        ARRAY_SIZE(ValueNameModulation, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Digital Gain", iPuschDigitalGain);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Cluster1", iStartRBCluster1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num RB Cluster1", iNumRBCluster1);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "PUSCH Tx Power (dBm)");
                int iPuschTxPower = u_temp & 127;   // 7 bits
                int iNumCqiBits = (u_temp >> 7) & 255;  // 8 bits
                int iRateMatchedCqiBits = (u_temp >> 18) & 16383;   // 16 bits
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Tx Power (dBm)", iPuschTxPower);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num CQI Bits", iNumCqiBits);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Rate Matched CQI Bits", iRateMatchedCqiBits);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Num DL Carriers");
                int iNumDLCarriers = u_temp & 3;    // 2 bits
                int iAckNackIndex = (u_temp >> 2) & 4096;   // 12 bits
                int iAckNackLate = (u_temp >> 14) & 1;  // 1 bit
                int iCSFLate = (u_temp >> 15) & 1;  // 1 bit
                int iDropPusch = (u_temp >> 16) & 1;  // 1 bit
                old_object = _replace_result_int(result_record_item,
                        "Num DL Carriers", iNumDLCarriers);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nack Index", iAckNackIndex);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nack Late", iAckNackLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "CSF Late", iCSFLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Drop PUSCH", iDropPusch);
                Py_DECREF(old_object);


                u_temp = _search_result_uint(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)");
                int iCSDSS0 = u_temp & 15;   // 4 bits
                int iCSDSS1 = (u_temp >> 4) & 15;  // 4 bits
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 0 (Samples)",
                        iCSDSS0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Cyclic Shift of DMRS Symbols Slot 1 (Samples)",
                        iCSDSS1);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PHY PUSCH Tx Report version: 0x%x\n", pkt_ver);
        return 0;
    }

}
