/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: zdiag ind producer
 * This file should be changed only infrequently and with great care.
 */

#include "diag_ind_src.h"
#include "securec.h"
#include "diag_mem.h"
#include "diag_pkt_router.h"
#include "diag_debug.h"
#include "soc_diag_cmd_id.h"
#include "diag_adapt_layer.h"
#include "diag_dfx.h"
#include "diag_filter.h"
#include "dfx_adapt_layer.h"
#include "diag_pkt.h"
#include "errcode.h"
#include "log_common.h"
#include "log_file.h"
#include "diag_msg.h"
#ifdef CONFIG_DIAG_WITH_SECURE
#include "diag_secure.h"
#endif
#include "diag_oam_log.h"

#define PKT_DATA_CNT_2    2

static diag_msg_flow_control_hook g_flow_control_hook = NULL;

STATIC bool zdiag_msg_permit(uint32_t module_id, uint8_t level)
{
    if ((zdiag_is_enable() == false) && (uapi_zdiag_offline_log_is_enable() != true)) {
        return false;
    }
    return zdiag_log_enable(level, module_id);
}

#ifdef CONFIG_DIAG_WITH_SECURE
static errcode_t diag_check_secure_connect(void)
{
    diag_secure_ctx_t *secure_ctx = diag_get_secure_ctx();
    uint8_t key_check[AES128_KEY_LEN];
    memset_s(key_check, AES128_KEY_LEN, 0, AES128_KEY_LEN);
    if (memcmp(secure_ctx->srp_info.key, key_check, AES128_KEY_LEN) != 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static errcode_t uapi_diag_report_packet_secure(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet,
    uint16_t packet_size, bool sync)
{
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_UART, .src_addr = DIAG_FRAME_FID_MCU};
    if (option != NULL) {
        process_param.dst_addr = option->peer_addr;
    }
    uint16_t iv_tag_ext_size = 0;
    bool need_to_encrypt = diag_need_secure(cmd_id);
    if (need_to_encrypt) {
        if (diag_check_secure_connect() != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
        iv_tag_ext_size = AES_IV_LEN + AES_TAG_LEN;
    }
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + iv_tag_ext_size];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    uint16_t packet_encrypt_size = DIAG_IND_HEADER_SIZE + packet_size;
    uint8_t *packet_encrypt = dfx_malloc(0, packet_encrypt_size);
    if (packet_encrypt == NULL) {
        return ERRCODE_DIAG_NOT_ENOUGH_MEMORY;
    }
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(packet_encrypt);
    if (memcpy_s(packet_encrypt + DIAG_IND_HEADER_SIZE, packet_size, packet, packet_size) != EOK) {
        dfx_free(0, packet_encrypt);
        return ERRCODE_FAIL;
    }
    diag_mk_frame_header_1(frame, iv_tag_ext_size + DIAG_IND_HEADER_SIZE + packet_size);
    diag_mk_ind_header(ind, cmd_id);
    if (need_to_encrypt) {
        frame->ctrl |= 1 << DIAG_ROUTER_CTAL_SECURE_FLAG_BIT;
        uint8_t *iv = &buf[DIAG_ROUTER_HEADER_LEN];
        uint8_t *tag = &buf[DIAG_ROUTER_HEADER_LEN + AES_IV_LEN];
        if (diag_aes_gcm_encrypt_inplace(packet_encrypt, packet_encrypt_size, iv, tag) != ERRCODE_SUCC) {
            dfx_free(0, packet_encrypt);
            return ERRCODE_FAIL;
        }
    }
    diag_pkt_handle_init(&pkt, PKT_DATA_CNT_2);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)frame,
        DIAG_ROUTER_HEADER_LEN + iv_tag_ext_size, DIAG_PKT_STACK_DATA);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, packet_encrypt, packet_encrypt_size, DIAG_PKT_STACK_DATA);
    if (sync) {
        process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    } else {
        process_param.cur_proc = DIAG_PKT_PROC_ASYNC;
    }
    diag_pkt_router(&pkt, &process_param);
    dfx_free(0, packet_encrypt);
    return ERRCODE_SUCC;
}

static errcode_t uapi_diag_report_packet_direct_secure(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet,
    uint16_t packet_size)
{
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_UART, .src_addr = DIAG_FRAME_FID_MCU};
    if (option != NULL) {
        process_param.dst_addr = option->peer_addr;
    }
    uint16_t iv_tag_ext_size = 0;
    bool need_to_encrypt = diag_need_secure(cmd_id);
    if (need_to_encrypt) {
        diag_secure_ctx_t *secure_ctx = diag_get_secure_ctx();
        uint8_t key_check[AES128_KEY_LEN];
        memset_s(key_check, AES128_KEY_LEN, 0, AES128_KEY_LEN);
        if (memcmp(secure_ctx->srp_info.key, key_check, AES128_KEY_LEN) != 0) {
            return ERRCODE_FAIL;
        }
        iv_tag_ext_size = AES_IV_LEN + AES_TAG_LEN;
    }
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + iv_tag_ext_size];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    uint16_t packet_encrypt_size = DIAG_IND_HEADER_SIZE + packet_size;
    uint8_t *packet_encrypt = dfx_malloc(0, packet_encrypt_size);
    if (packet_encrypt == NULL) {
        return ERRCODE_DIAG_NOT_ENOUGH_MEMORY;
    }
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(packet_encrypt);
    if (memcpy_s(packet_encrypt + DIAG_IND_HEADER_SIZE, packet_size, packet, packet_size) != EOK) {
        dfx_free(0, packet_encrypt);
        return ERRCODE_FAIL;
    }
    diag_mk_frame_header_1(frame, iv_tag_ext_size + DIAG_IND_HEADER_SIZE + packet_size);
    diag_mk_ind_header(ind, cmd_id);
    if (need_to_encrypt) {
        frame->ctrl |= 1 << DIAG_ROUTER_CTAL_SECURE_FLAG_BIT;
        uint8_t *iv = &buf[DIAG_ROUTER_HEADER_LEN];
        uint8_t *tag = &buf[DIAG_ROUTER_HEADER_LEN + AES_IV_LEN];
        if (diag_aes_gcm_encrypt_inplace(packet_encrypt, packet_encrypt_size, iv, tag) != ERRCODE_SUCC) {
            dfx_free(0, packet_encrypt);
            return ERRCODE_FAIL;
        }
    }
    diag_pkt_handle_init(&pkt, PKT_DATA_CNT_2);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)frame,
        DIAG_ROUTER_HEADER_LEN + iv_tag_ext_size, DIAG_PKT_STACK_DATA);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, packet_encrypt, packet_encrypt_size, DIAG_PKT_STACK_DATA);
    process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    diag_pkt_router(&pkt, &process_param);
    dfx_free(0, packet_encrypt);
    return ERRCODE_SUCC;
}
#endif

void uapi_diag_register_msg_flow_control_hook(diag_msg_flow_control_hook func)
{
    g_flow_control_hook = func;
}

errcode_t uapi_diag_report_packet(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet,
    uint16_t packet_size, bool sync)
{
    dfx_log_debug("uapi_diag_report_packet in. cmd_id = 0x%x packet_size = %d.\r\n", cmd_id, packet_size);
    if (zdiag_is_enable() == false) {
        dfx_log_info("diag_unconnect\r\n");
        return ERRCODE_FAIL;
    }
#ifndef CONFIG_DIAG_WITH_SECURE
    errcode_t ret = ERRCODE_SUCC;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_UART, .src_addr = DIAG_FRAME_FID_MCU};
    if (option != NULL) {
        process_param.dst_addr = option->peer_addr;
    }
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(buf + DIAG_ROUTER_HEADER_LEN);

    diag_pkt_handle_init(&pkt, PKT_DATA_CNT_2); /* data_cnt 为2 */
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)frame,
        DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE, DIAG_PKT_STACK_DATA);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, (uint8_t *)packet, packet_size, DIAG_PKT_STACK_DATA);

    diag_mk_ind_header(ind, cmd_id);

    diag_mk_frame_header_1(frame, DIAG_IND_HEADER_SIZE + packet_size);
    if (sync) {
        process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    } else {
        process_param.cur_proc = DIAG_PKT_PROC_ASYNC;
    }
    diag_pkt_router(&pkt, &process_param);
    return ret;
#else
    return uapi_diag_report_packet_secure(cmd_id, option, packet, packet_size, sync);
#endif
}

errcode_t uapi_diag_report_packet_direct(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet,
    uint16_t packet_size)
{
    unused(option);
#ifndef CONFIG_DIAG_WITH_SECURE
    errcode_t ret = ERRCODE_SUCC;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_UART, .src_addr = DIAG_FRAME_FID_MCU};
    if (option != NULL) {
        process_param.dst_addr = option->peer_addr;
    }
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(buf + DIAG_ROUTER_HEADER_LEN);

    diag_pkt_handle_init(&pkt, PKT_DATA_CNT_2); /* data_cnt 为2 */
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)frame,
        DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE, DIAG_PKT_STACK_DATA);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, (uint8_t *)packet, packet_size, DIAG_PKT_STACK_DATA);

    diag_mk_ind_header(ind, cmd_id);

    diag_mk_frame_header_1(frame, DIAG_IND_HEADER_SIZE + packet_size);
    process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    diag_pkt_router(&pkt, &process_param);
    return ret;
#else
    return uapi_diag_report_packet_direct_secure(cmd_id, option, packet, packet_size);
#endif
}

STATIC errcode_t zdiag_report_packets(uint16_t cmd_id, diag_option_t *option, diag_report_packet *report_packet,
    uint8_t critical)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t **packet = report_packet->packet;
    uint16_t *packet_size = report_packet->packet_size;
    uint8_t pkt_cnt = report_packet->pkt_cnt;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_UART, .src_addr = DIAG_FRAME_FID_MCU};
    if (option != NULL) {
        process_param.dst_addr = option->peer_addr;
    }
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(buf + DIAG_ROUTER_HEADER_LEN);

    uint16_t usr_data_size = 0;

    dfx_log_debug("zdiag_report_packets in. cmd_id = 0x%x\r\n", cmd_id);
    if (zdiag_is_enable() == false) {
        dfx_log_info("diag_unconnect\r\n");
        return ERRCODE_FAIL;
    }

    if (pkt_cnt > DIAG_PKT_DATA_ID_MAX - 1) {
        dfx_log_err("[ERROR] zdiag_report_packets pkt_cnt invalid.\r\n");
        return ERRCODE_FAIL;
    }

    diag_pkt_handle_init(&pkt, pkt_cnt + 1);
    if (critical != 0) {
        diag_pkt_set_critical(&pkt);
    }
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)frame, DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE,
        DIAG_PKT_STACK_DATA);
    for (uint8_t i = 0; i < pkt_cnt; i++) {
        diag_pkt_handle_set_data(&pkt, i + 1, (uint8_t *)packet[i], packet_size[i], DIAG_PKT_STACK_DATA);
        usr_data_size += packet_size[i];
    }

    diag_mk_ind_header(ind, cmd_id);

    diag_mk_frame_header_1(frame, DIAG_IND_HEADER_SIZE + usr_data_size);

    process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    diag_pkt_router(&pkt, &process_param);
    return ret;
}

errcode_t uapi_diag_report_packets_critical(uint16_t cmd_id, diag_option_t *option, uint8_t **packet,
    uint16_t *packet_size, uint8_t pkt_cnt)
{
    diag_report_packet pkt;
    pkt.packet = packet;
    pkt.packet_size = packet_size;
    pkt.pkt_cnt = pkt_cnt;
    return zdiag_report_packets(cmd_id, option, &pkt, 1);
}

errcode_t uapi_diag_report_packets_normal(uint16_t cmd_id, diag_option_t *option, uint8_t **packet,
    uint16_t *packet_size, uint8_t pkt_cnt)
{
    diag_report_packet pkt;
    pkt.packet = packet;
    pkt.packet_size = packet_size;
    pkt.pkt_cnt = pkt_cnt;
    return zdiag_report_packets(cmd_id, option, &pkt, 0);
}

errcode_t uapi_diag_report_sys_msg_common(uint32_t module_id, uint32_t msg_id, const uint8_t *packet,
    uint16_t packet_size, diag_msg_attr_t *msg_attr)
{
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_UART, .src_addr = DIAG_FRAME_FID_MCU};
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(buf + DIAG_ROUTER_HEADER_LEN);
    uint8_t *log_header = (uint8_t *)(buf + DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE);
    uint8_t *pkt_header = NULL;
    uint16_t pkt_header_len;
    uint8_t level = msg_attr->level;

    if (zdiag_msg_permit(module_id, level) == false) {
        return ERRCODE_FAIL;
    }

    if (g_flow_control_hook != NULL && g_flow_control_hook((uint8_t)module_id, level) == false) {
        return ERRCODE_FAIL;
    }

    diag_pkt_handle_init(&pkt, 1); /* data_cnt 为1 */

    if (uapi_zdiag_offline_log_is_enable()) {
        pkt_header = log_header;
        pkt_header_len = (uint16_t)DIAG_LOG_HEADER_SIZE;
        diag_pkt_set_output_type(&pkt, 1);
    } else {
        pkt_header = (uint8_t *)frame;
        pkt_header_len = (uint16_t)(DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE);
    }

    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, pkt_header, pkt_header_len, DIAG_PKT_STACK_DATA);
    if (packet_size != 0) {
        diag_pkt_cnt_increase(&pkt, 1);
        diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, (uint8_t *)packet, packet_size, DIAG_PKT_STACK_DATA);
    }

    diag_mk_ind_header(ind, DIAG_CMD_MSG_RPT_SYS);

    diag_cmd_log_layer_t *log = (diag_cmd_log_layer_t *)log_header;
    diag_mk_log_pkt(log, module_id, msg_id, msg_attr->sn, msg_attr->sn_enable);

    diag_mk_frame_header_1(frame, DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE + packet_size);

    if (msg_attr->sync) {
        process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    } else {
        process_param.cur_proc = DIAG_PKT_PROC_ASYNC;
    }
    return diag_pkt_router(&pkt, &process_param);
}

errcode_t uapi_zdiag_report_sys_msg_instance(uint32_t module_id, uint32_t msg_id, const uint8_t *packet,
    uint16_t packet_size, uint8_t level)
{
    /* SN由内部设置，日志使用async模式，发送到DFX线程 */
    diag_msg_attr_t msg_attr = {0};
    msg_attr.level = level;
    msg_attr.sync = false;
    return uapi_diag_report_sys_msg_common(module_id, msg_id, packet, packet_size, &msg_attr);
}

errcode_t uapi_zdiag_report_sys_msg_instance_sn(uint32_t module_id, uint32_t msg_id,
    diag_report_sys_msg_packet *report_sys_msg_packet, uint8_t level, uint32_t sn)
{
    uint8_t *packet = report_sys_msg_packet->packet;
    uint16_t packet_size = report_sys_msg_packet->packet_size;

    /* SN由外部指定，日志直接输出到串口，不发送到DFX线程 */
    diag_msg_attr_t msg_attr = {0};
    msg_attr.level = level;
    msg_attr.sync = true;
    msg_attr.sn = sn;
    msg_attr.sn_enable = true;

    return uapi_diag_report_sys_msg_common(module_id, msg_id, packet, packet_size, &msg_attr);
}

/* diag日志输出到共享内存中 */
errcode_t uapi_diag_report_sys_msg_to_buffer(uint32_t module_id, uint32_t msg_id, const uint8_t *packet,
    uint16_t packet_size, uint8_t level)
{
    unused(level);
    uint8_t buf[DIAG_FRAME_HEADER_SIZE + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE +
                sizeof(uint32_t) * OML_LOG_ALTER_PARA_MAX_NUM + DIAG_ROUTER_EOF_LEN];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(buf + DIAG_FRAME_HEADER_SIZE);
    uint8_t *log_header = (uint8_t *)(buf + DIAG_FRAME_HEADER_SIZE + DIAG_IND_HEADER_SIZE);

    diag_mk_ind_header(ind, DIAG_CMD_MSG_RPT_SYS);

    diag_cmd_log_layer_t *log = (diag_cmd_log_layer_t *)log_header;
    diag_mk_log_pkt(log, module_id, msg_id, 0, false);

    diag_mk_frame_header_1(frame, DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE + packet_size + DIAG_ROUTER_EOF_LEN);
    /* 启用结束标志 */
    frame->ctrl = frame_ctrl_build(0, 0, 0, 1, 0, 0, 0);

    if (packet_size != 0) {
        if (memcpy_s(log->data, sizeof(uint32_t) * OML_LOG_ALTER_PARA_MAX_NUM, packet, packet_size) != EOK) {
            return ERRCODE_MEMCPY;
        }
    }
    log->data[packet_size] = DIAG_ROUTER_FRAME_END_FLAG;

    log_event((uint8_t *)buf, DIAG_FRAME_HEADER_SIZE + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE +
              packet_size + DIAG_ROUTER_EOF_LEN);
    return ERRCODE_SUCC;
}

errcode_t uapi_diag_report_sys_msg_from_buffer(uint8_t *buf, uint16_t buf_size, bool proc_async)
{
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_UART, .src_addr = DIAG_FRAME_FID_MCU};
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_router_inner_frame_t *inner = (diag_router_inner_frame_t *)frame->inner;
    uint8_t *log_header = (uint8_t *)(buf + DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE);
    uint8_t *pkt_header = NULL;
    uint16_t pkt_header_len;

    diag_cmd_log_layer_t *log = (diag_cmd_log_layer_t *)log_header;
    /* 目前只有日志帧使用 */
    uint32_t module_id = ((log->id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
#if (CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG == DFX_YES)
    module_id |= (log->sub_mod << SUB_MOD_ID_OFFSET);
#endif
    uint8_t level = log->id & OAM_LOG_LEVEL_MASK;

    if (zdiag_msg_permit(module_id, level) == false) {
        return ERRCODE_FAIL;
    }

    if (g_flow_control_hook != NULL && g_flow_control_hook((uint8_t)module_id, level) == false) {
        return ERRCODE_FAIL;
    }

    diag_pkt_handle_init(&pkt, 1); /* data_cnt 为1 */

    if (uapi_zdiag_offline_log_is_enable()) {
        pkt_header = log_header;
        pkt_header_len = (uint16_t)(get_frame_len(inner->len_msb, inner->len_lsb) - DIAG_IND_HEADER_SIZE);
        if (get_frame_ctrl_eof_en(frame->ctrl) != 0) {
            pkt_header_len -= DIAG_ROUTER_EOF_LEN;
        }
        diag_pkt_set_output_type(&pkt, 1);
    } else {
        pkt_header = buf;
        pkt_header_len = (uint16_t)buf_size;
    }

    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, pkt_header, pkt_header_len, DIAG_PKT_STACK_DATA);

    process_param.cur_proc = proc_async ? DIAG_PKT_PROC_ASYNC : DIAG_PKT_PROC_SYNC;
    return diag_pkt_router(&pkt, &process_param);
}

#if (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
errcode_t uapi_zdiag_report_sys_msg_custom(uint32_t msg_id, uint32_t src_mod, uint32_t dst_mod,
    diag_report_sys_msg_packet *report_sys_msg_packet, uint8_t level)
{
    unused(level);
    uint8_t *packet = report_sys_msg_packet->packet;
    uint16_t packet_size = report_sys_msg_packet->packet_size;
    diag_pkt_handle_t pkt_handle;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_UART, .src_addr = DIAG_FRAME_FID_MCU};
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(buf + DIAG_ROUTER_HEADER_LEN);
    uint8_t *log_header = (uint8_t *)(buf + DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE);
    uint8_t *pkt_header = NULL;
    uint16_t pkt_header_len;

    if ((zdiag_is_enable() == false)) {
        return ERRCODE_FAIL;
    }

    diag_pkt_handle_init(&pkt_handle, 1); /* data_cnt 为1 */

    if (uapi_zdiag_offline_log_is_enable()) {
        pkt_header = log_header;
        pkt_header_len = (uint16_t)DIAG_LOG_HEADER_SIZE;
        diag_pkt_set_output_type(&pkt_handle, 1);
    } else {
        pkt_header = (uint8_t *)frame;
        pkt_header_len = (uint16_t)(DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE);
    }

    diag_pkt_handle_set_data(&pkt_handle, DIAG_PKT_DATA_ID_0, pkt_header, pkt_header_len, DIAG_PKT_STACK_DATA);
    if (packet_size != 0) {
        diag_pkt_cnt_increase(&pkt_handle, 1);
        diag_pkt_handle_set_data(&pkt_handle, DIAG_PKT_DATA_ID_1, (uint8_t *)packet, packet_size, DIAG_PKT_STACK_DATA);
    }

    diag_mk_ind_header(ind, DIAG_CMD_MSG_RPT_SYS);

    diag_cmd_log_layer_stru_t *log = (diag_cmd_log_layer_stru_t *)log_header;
    diag_mk_log_pkt_custom(log, src_mod, dst_mod, msg_id);

    diag_mk_frame_header_1(frame, DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE + packet_size);

    process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    return diag_pkt_router(&pkt_handle, &process_param);
}
#endif
