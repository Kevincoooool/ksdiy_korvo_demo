// Copyright 2021-2023 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "esp_err.h"
#include "usb/usb_types_stack.h"
#include "libuvc_def.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UVC_XFER_ISOC = 0,  /*!< Isochronous Transfer Mode */
    UVC_XFER_BULK,      /*!< Bulk Transfer Mode */
} uvc_xfer_t;

typedef enum {
    STREAM_UVC = 0,     /*!< usb video stream */
    STREAM_UAC_SPK,     /*!< usb audio speaker stream */
    STREAM_UAC_MIC,     /*!< usb audio microphone stream */
    STREAM_MAX,
} usb_stream_t;

typedef enum {
    CTRL_NONE = 0,
    CTRL_SUSPEND,      /*!< streaming suspend control. ctrl_data NULL */
    CTRL_RESUME,       /*!< streaming resume control. ctrl_data NULL */
    CTRL_UAC_MUTE,     /*!< mute control. ctrl_data (false/true) */
    CTRL_UAC_VOLUME,   /*!< volume control. ctrl_data (0~100) */
    CTRL_MAX,
} stream_ctrl_t;

/**
 * @brief users need to get params from camera descriptors,
 * run the example first to printf the descriptor
 */
typedef struct uvc_config {
    uvc_xfer_t xfer_type;           /*!< UVC stream transfer type, UVC_XFER_ISOC or UVC_XFER_BULK */
    uint8_t format_index;           /*!< Format index of MJPEG */
    uint8_t frame_index;            /*!< Frame index, to choose resolution */
    uint16_t frame_width;           /*!< Picture width of selected frame_index */
    uint16_t frame_height;          /*!< Picture height of selected frame_index */
    uint32_t frame_interval;        /*!< Frame interval in 100-ns units, 666666 ~ 15 Fps*/
    uint16_t interface;             /*!< UVC stream interface number */
    uint16_t interface_alt;         /*!< UVC stream alternate interface, to choose MPS (Max Packet Size), bulk fix to 0*/
    uint8_t ep_addr;                /*!< endpoint address of selected alternate interface*/
    uint32_t ep_mps;                /*!< MPS of selected interface_alt */
    uint32_t xfer_buffer_size;      /*!< Transfer buffer size, using double buffer here, must larger than one frame size */
    uint8_t *xfer_buffer_a;         /*!< Buffer a for usb payload */
    uint8_t *xfer_buffer_b;         /*!< Buffer b for usb payload */
    uint32_t frame_buffer_size;     /*!< Frame buffer size, must larger than one frame size */
    uint8_t *frame_buffer;          /*!< Buffer for one frame */
    uvc_frame_callback_t *frame_cb; /*!< callback function to handle incoming frame */
    void *frame_cb_arg;             /*!< callback function arg */
} uvc_config_t;

/* mic callback type **/ 
typedef struct {
    void *data;                 /*!< mic data */
    uint32_t data_bytes;        /*!< mic data size */
    uint16_t bit_resolution;    /*!< bit resolution in buffer */
    uint32_t samples_frequence; /*!< mic sample frequency */
} mic_frame_t;

typedef void(mic_callback_t)(mic_frame_t *frame, void *user_ptr);

/**
 * @brief uac interface params, config before usb_streaming_start
 */
typedef struct {
    uint16_t mic_interface;              /*!< microphone stream interface number, set 0 if not use */
    uint16_t mic_bit_resolution;         /*!< microphone resolution, bits */
    uint32_t mic_samples_frequence;      /*!< microphone frequence, Hz */
    uint8_t  mic_ep_addr;                /*!< microphone interface endpoint address */
    uint32_t mic_ep_mps;                 /*!< microphone interface endpoint mps */
    uint16_t spk_interface;              /*!< speaker stream interface number, set 0 if not use */
    uint16_t spk_bit_resolution;         /*!< speaker resolution, bits */
    uint32_t spk_samples_frequence;      /*!< speaker frequence, Hz */
    uint8_t  spk_ep_addr;                /*!< speaker interface endpoint address */
    uint32_t spk_ep_mps;                 /*!< speaker interface endpoint mps */
    uint32_t spk_buf_size;               /*!< size of speaker send buffer, should be a multiple of spk_ep_mps */
    uint32_t mic_buf_size;               /*!< mic receive buffer size, 0 if not use, else should be a multiple of mic_min_bytes */
    uint32_t mic_min_bytes;              /*!< min bytes to trigger mic callback, 0 if not using callback, else must be multiple (1~32) of mic_ep_mps */
    mic_callback_t *mic_cb;              /*!< mic callback, can not block in here! */
    void *mic_cb_arg;                    /*!< mic callback args */
    uint16_t ac_interface;               /*!< (optional) audio control interface number, set 0 if not use */
    uint8_t  mic_fu_id;                  /*!< (optional) microphone feature unit id, set 0 if not use */
    uint8_t  spk_fu_id;                  /*!< (optional) speaker feature unit id, set 0 if not use */
} uac_config_t;

/**
 * @brief Pre-config UVC driver with params from known USB Camera Descriptor
 * 
 * @param config config struct described in uvc_config_t
 * @return esp_err_t 
 *         ESP_ERR_INVALID_ARG Args not supported
 *         ESP_OK Config driver succeed
 */
esp_err_t uvc_streaming_config(const uvc_config_t *config);

/**
 * @brief Pre-config UAC driver with params from known USB Camera Descriptor
 * 
 * @param config config struct described in uvc_config_t
 * @return esp_err_t 
 *         ESP_ERR_INVALID_ARG Args not supported
 *         ESP_OK Config driver succeed
 */
esp_err_t uac_streaming_config(const uac_config_t *config);

/**
 * @brief Start usb streaming with pre-configs, usb driver will create internal tasks
 * to handle usb data from stream pipe, and run user's callback after new frame ready.
 * 
 * @return
 *         ESP_ERR_INVALID_STATE streaming not configured, or streaming running 
 *         ESP_ERR_INVALID_ARG args not supported
 *         ESP_FAIL start failed
 *         ESP_OK start succeed
 */
esp_err_t usb_streaming_start(void);

/**
 * @brief Stop current usb streaming, internal tasks will be delete, related resourse will be free
 * 
 * @return 
 *         ESP_ERR_INVALID_STATE not inited
 *         ESP_OK stop succeed
 *         ESP_ERR_TIMEOUT stop wait timeout
 */
esp_err_t usb_streaming_stop(void);

/**
 * @brief Control specified streaming features
 * @Note suspend/resume only supported for isochronous mode stream
 * @param stream stream type defined in usb_stream_t
 * @return
 *         ESP_ERR_INVALID_STATE not inited
 *         ESP_FAIL resume failed
 *         ESP_ERR_NOT_SUPPORTED not support
 *         ESP_OK resume succeed
 *         ESP_ERR_TIMEOUT resume wait timeout
 */
esp_err_t usb_streaming_control(usb_stream_t stream, stream_ctrl_t ctrl_type, void *ctrl_value);

/**
 * @brief Write data to the speaker buffer, will be send out when USB is ready
 * 
 * @param data pointer to the data to be written
 * @param data_bytes The size of the data to be written.
 * @param timeout_ms The timeout value for the write operation.
 * 
 * @return
 *         ESP_ERR_INVALID_STATE not inited
 *         ESP_FAIL push data failed
 *         ESP_OK succeed
 */
esp_err_t uac_spk_streaming_write(void *data, size_t data_bytes, size_t timeout_ms);

/**
 * @brief Read data from internal mic buffer, the actual size will be returned
 * 
 * @param buf pointer to the buffer to store the received data
 * @param buf_size The size of the data buffer.
 * @param data_bytes The actual size read from buffer
 * @param timeout_ms The timeout value for the read operation.
 * 
 * @return
 *         ESP_ERR_INVALID_STATE not inited
 *         ESP_FAIL push data failed
 *         ESP_OK succeed
 */
esp_err_t uac_mic_streaming_read(void *buf, size_t buf_size, size_t *data_bytes, size_t timeout_ms);

#ifdef __cplusplus
}
#endif