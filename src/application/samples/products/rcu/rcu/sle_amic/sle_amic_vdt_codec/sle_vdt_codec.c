/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE RCU VDT Codec Source. \n
 *
 * History: \n
 * 2023-12-26, Create file. \n
 */
#include "common_def.h"
#include "osal_debug.h"
#include "audio_enc_codec.h"
#include "audio_dec_codec.h"
#include "audio_profile_calc.h"
#include "sle_vdt_codec.h"

typedef struct sle_vdt_codec_inst {
    uint32_t codec_id;
    uint32_t sample_rate;
    uint32_t bit_depth;
    uint32_t channels;
} sle_vdt_codec_inst_t;

static sle_vdt_codec_inst_t g_sle_vdt_codec_inst = {
    .codec_id = 0,
    .sample_rate = 16000,
    .bit_depth = (uint32_t)0x10,
    .channels = (uint32_t)1
};

#if defined (CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static const uint32_t g_encoder_indata_size[2][4] = {
    {ENC_FREAM_16K_SBC_SIZE, ENC_FREAM_16K_MSBC_SIZE, ENC_FREAM_16K_OPUS_SIZE, ENC_FREAM_16K_L2HC_SIZE},
    {ENC_FREAM_48K_SBC_SIZE, ENC_FREAM_48K_MSBC_SIZE, ENC_FREAM_48K_OPUS_SIZE, ENC_FREAM_48K_L2HC_SIZE} /* msbc不支持 */
}; /* 与codec_id_iput一一对应 */
#else
static const uint32_t g_decoder_indata_size[2][4] = {
    {DEC_FREAM_16K_SBC_SIZE, DEC_FREAM_16K_MSBC_SIZE, DEC_FREAM_16K_OPUS_SIZE, DEC_FREAM_16K_L2HC_SIZE},
    {DEC_FREAM_48K_SBC_SIZE, DEC_FREAM_48K_MSBC_SIZE, DEC_FREAM_48K_OPUS_SIZE, DEC_FREAM_48K_L2HC_SIZE} /* msbc不支持 */
}; /* 与codec_id_iput一一对应 */
#endif

#if defined (CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static void sle_vdt_codec_set_enc_data_size(sle_vdt_codec_inst_t *codec_inst)
{
    uint32_t base_size;
    uint32_t data_size;
    base_size = g_encoder_indata_size[0][codec_inst->codec_id];
    data_size = (base_size * codec_inst->channels);
#ifdef CODEC_5MS
    if (codec_inst->codec_id == HA_CODEC_ID_OPUS) {
        data_size = (base_size * codec_inst->channels) >> 1;
    }
#endif
    audio_set_encoder_consume_data_size(data_size);
}
#else
static void sle_vdt_codec_set_dec_data_size(sle_vdt_codec_inst_t *codec_inst)
{
    uint32_t base_size;
    uint32_t data_size;
    base_size = g_decoder_indata_size[0][codec_inst->codec_id];
    data_size = (base_size * codec_inst->channels);
    audio_set_decoder_consume_data_size(data_size);
}
#endif

void sle_vdt_codec_init(void)
{
    int32_t ret;
#if defined (CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
    sle_vdt_codec_set_enc_data_size(&g_sle_vdt_codec_inst);
    ha_codec_enc_param enc_open_param;
    enc_open_param.channels = g_sle_vdt_codec_inst.channels;
    enc_open_param.bit_depth = g_sle_vdt_codec_inst.bit_depth;
    enc_open_param.sample_rate = g_sle_vdt_codec_inst.sample_rate;
    enc_open_param.private_data = NULL;
    enc_open_param.private_data_size = 0;
    ret = aenc_open_codec(g_sle_vdt_codec_inst.codec_id, &enc_open_param);
    if (ret != AUDIO_SUCCESS) {
        osal_printk("open encoder codec fail %#x\n", ret);
        aenc_close_codec();
        return;
    }
    osal_printk("Open encoder codec success.\r\n");
#else
    sle_vdt_codec_set_dec_data_size(&g_sle_vdt_codec_inst);
    ha_codec_dec_param dec_open_param;
    dec_open_param.channels = g_sle_vdt_codec_inst.channels;
    dec_open_param.bit_depth = g_sle_vdt_codec_inst.bit_depth;
    dec_open_param.sample_rate = g_sle_vdt_codec_inst.sample_rate;
    dec_open_param.private_data = NULL;
    dec_open_param.private_data_size = 0;
    ret = adec_open_codec(g_sle_vdt_codec_inst.codec_id, &dec_open_param);
    if (ret != AUDIO_SUCCESS) {
        osal_printk("open decoder codec fail %#x\n", ret);
        adec_close_codec();
        return;
    }
    osal_printk("Open decoder codec success.\r\n");
#endif
}

void sle_vdt_codec_deinit(void)
{
#if defined (CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
    aenc_close_codec();
#else
    adec_close_codec();
#endif
}

uint32_t sle_vdt_codec_encode(uint8_t *pcm_data, uint8_t **enc_data)
{
    int32_t ret;
    ring_buffer_t *enc_in_fifo = audio_get_in_fifo(HA_CODEC_ENDODER);
    ring_buffer_t *enc_out_fifo = audio_get_out_fifo(HA_CODEC_ENDODER);

    /* audio codec begin */
    ret = write_data_into_fifo(pcm_data, enc_in_fifo, audio_get_encoder_consume_data_size());
    if (ret != AUDIO_SUCCESS) {
        osal_printk("write data err %d\n", ret);
        return 0;
    }

    ret = aenc_encode_frame();
    if (ret != 0) {
        osal_printk("aenc_encode_frame %d\n", ret);
        return 0;
    }

    uint32_t enc_product_data_size = audio_get_encoder_product_data_size();
    ret = read_data_from_fifo(enc_data, enc_out_fifo, enc_product_data_size);
    if (ret != AUDIO_SUCCESS) {
        return 0;
    }

    read_data_from_fifo_finish(enc_out_fifo, enc_product_data_size);

    return enc_product_data_size;
}

uint32_t sle_vdt_codec_decode(uint8_t *enc_data, uint8_t **pcm_data)
{
    int32_t ret;
    ring_buffer_t *dec_in_fifo = audio_get_in_fifo(HA_CODEC_DECODER);
    ring_buffer_t *dec_out_fifo = audio_get_out_fifo(HA_CODEC_DECODER);

    ret = write_data_into_fifo(enc_data, dec_in_fifo, audio_get_decoder_consume_data_size());
    if (ret != AUDIO_SUCCESS) {
        osal_printk("write data err %d\n", ret);
        return 0;
    }

    ret = adec_decode_frame(0);
    if (ret != 0) {
        osal_printk("adec_decode_frame %d\n", ret);
        return 0;
    }

    uint32_t product_data_size = audio_get_decoder_product_data_size();
    ret = read_data_from_fifo(pcm_data, dec_out_fifo, product_data_size);
    if (ret != AUDIO_SUCCESS) {
        return 0;
    }
    read_data_from_fifo_finish(dec_out_fifo, product_data_size);

    return product_data_size;
}