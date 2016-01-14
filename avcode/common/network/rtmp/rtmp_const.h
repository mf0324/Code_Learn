/**
 * @file rtmp_const.h   RTMP Constant
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.19
 *
 *
 */

#ifndef __RTMP_CONST_H__
#define __RTMP_CONST_H__

/** 
 * @name RTMP
 * @{
 */

/** Version */
#define RTMP_VERSION                                       (3)
/** Communication Port */
#define RTMP_COMM_PORT                                     (1935)
/** Max Chunk Stream */
#define RTMP_MAX_CHUNK_STREAM                              (65600)
/** Chunk Stream ID Sys */
#define RTMP_CHUNK_STREAM_ID_SYS                           (2)


/** C0 Size */
#define RTMP_C0_SIZE                                       (1)
/** S0 Size */
#define RTMP_S0_SIZE                                       (1)
/** C1 Size */
#define RTMP_C1_SIZE                                       (1536)
/** C1 Random Data Size */
#define RTMP_C1_RANDOM_DATA_SIZE                           (1528)
/** c1 key size */
#define RTMP_C1_KEY_SIZE                                   (128)
/** c1 digest size */
#define RTMP_C1_DIGEST_SIZE                                (32)
/** S1 Size */
#define RTMP_S1_SIZE                                       (RTMP_C1_SIZE)
/** S1 Random Data Size */
#define RTMP_S1_RANDOM_DATA_SIZE                           (RTMP_C1_RANDOM_DATA_SIZE)
/** C2 Size */
#define RTMP_C2_SIZE                                       (1536)
/** C2 Random Data Size */
#define RTMP_C2_RANDOM_DATA_SIZE                           (1528)
/** S2 Size */
#define RTMP_S2_SIZE                                       (RTMP_C2_SIZE)
/** S2 Random Data Size */
#define RTMP_S2_RANDOM_DATA_SIZE                           (RTMP_C2_RANDOM_DATA_SIZE)
/** time size */
#define RTMP_TIME_SIZE                                     (4)
/** version size */
#define RTMP_VERSION_SIZE                                  (4)
/** c1s1 offset size */
#define RTMP_C1S1_OFFSET_SIZE                              (4)
/** c1s1 key size */
#define RTMP_C1S1_KEY_SIZE                                 (764)
/** c1s1 digest size */
#define RTMP_C1S1_DIGEST_SIZE                              (764)
/** c1s1 join size */
#define RTMP_C1S1_JOIN_SIZE                                (RTMP_C1_SIZE - RTMP_C1_DIGEST_SIZE)
/** c1s1 max digest offset size */
#define RTMP_C1S1_MAX_DIGEST_OFFSET_SIZE                   (RTMP_C1S1_DIGEST_SIZE - RTMP_C1_DIGEST_SIZE - RTMP_C1S1_OFFSET_SIZE)
/** c1s1 max key offset size */
#define RTMP_C1S1_MAX_KEY_OFFSET_SIZE                      (RTMP_C1S1_KEY_SIZE - RTMP_C1_KEY_SIZE - RTMP_C1S1_OFFSET_SIZE)


/** Chunk Size */
#define RTMP_CHUNK_SIZE                                    (128)
/** Chunk Basic Header Min Size */
#define RTMP_CHUNK_BASIC_HEADER_MIN_SIZE                   (1)
/** Chunk Basic Header Size1 */
#define RTMP_CHUNK_BASIC_HEADER_SIZE_1                     (1)
/** Chunk Basic Header Size2 */
#define RTMP_CHUNK_BASIC_HEADER_SIZE_2                     (2)
/** Chunk Basic Header Size3 */
#define RTMP_CHUNK_BASIC_HEADER_SIZE_3                     (3)
/** Chunk Stream ID1 Begin */
#define RTMP_CHUNK_STREAM_ID1_BEGIN                        (3)
/** Chunk Stream ID1 End */
#define RTMP_CHUNK_STREAM_ID1_END                          (63)
/** Chunk Stream ID2 Begin */
#define RTMP_CHUNK_STREAM_ID2_BEGIN                        (64)
/** Chunk Stream ID2 End */
#define RTMP_CHUNK_STREAM_ID2_END                          (319)
/** Chunk Stream ID3 Begin */
#define RTMP_CHUNK_STREAM_ID3_BEGIN                        (64)
/** Chunk Stream ID3 End */
#define RTMP_CHUNK_STREAM_ID3_END                          (65599)
/** Chunk Type0 */
#define RTMP_CHUNK_TYPE_0                                  (0)
/** Chunk Type1 */
#define RTMP_CHUNK_TYPE_1                                  (1)
/** Chunk Type2 */
#define RTMP_CHUNK_TYPE_2                                  (2)
/** Chunk Type3 */
#define RTMP_CHUNK_TYPE_3                                  (3)
/** Msg Header Size0 */
#define RTMP_CHUNK_MSG_HEADER_SIZE_0                       (11)
/** Msg Header Size1 */
#define RTMP_CHUNK_MSG_HEADER_SIZE_1                       (7)
/** Msg Header Size2 */
#define RTMP_CHUNK_MSG_HEADER_SIZE_2                       (3)
/** Msg Header Size3 */
#define RTMP_CHUNK_MSG_HEADER_SIZE_3                       (0)
/** Ext Timestamp Size */
#define RTMP_CHUNK_EXT_TS_SIZE                             (4)
/** Msg Timestamp Size */
#define RTMP_CHUNK_MSG_TS_SIZE                             (3)
/** Msg Len Size */
#define RTMP_CHUNK_MSG_LEN_SIZE                            (3)
/** Msg Type ID Size */
#define RTMP_CHUNK_MSG_TYPE_ID_SIZE                        (1)
/** Msg Stream ID Size */
#define RTMP_CHUNK_MSG_STREAM_ID_SIZE                      (4)
/** Msg Timestamp Delta Size */
#define RTMP_CHUNK_MSG_TS_DELTA_SIZE                       (3)

/** control chunk stream id */
#define RTMP_CTRL_CHUNK_STREAM_ID                          (0x2)

/** Chunk Invalid Timestamp */
#define RTMP_CHUNK_INVALID_TIMESTAMP                       (0xFFFFFF)

/** Set Chunk Size */
#define RTMP_MSG_TYPE_SET_CHUNK_SIZE                       (1)
/** Abort */
#define RTMP_MSG_TYPE_ABORT                                (2)
/** Acknowledgement */
#define RTMP_MSG_TYPE_ACK                                  (3)
/** User Control */
#define RTMP_MSG_TYPE_USER_CONTROL                         (4)
/** Window Acknowledgement Size */
#define RTMP_MSG_TYPE_WINDOW_ACK_SIZE                      (5)
/** Set Peer Bandwidth */
#define RTMP_MSG_TYPE_SET_PEER_BANDWIDTH                   (6)
/** Audio */
#define RTMP_MSG_TYPE_AUDIO                                (8)
/** Video */
#define RTMP_MSG_TYPE_VIDEO                                (9)
/** AMF3 Data */
#define RTMP_MSG_TYPE_AMF3_DATA                            (15)
/** AMF3 Share Object */
#define RTMP_MSG_TYPE_AMF3_SHARE_OBJ                       (16)
/** AMF3 Cmd */
#define RTMP_MSG_TYPE_AMF3_CMD                             (17)
/** AMF0 Data */
#define RTMP_MSG_TYPE_AMF0_DATA                            (18)
/** AMF0 Share Object */
#define RTMP_MSG_TYPE_AMF0_SHARE_OBJ                       (19)
/** AMF0 Cmd */
#define RTMP_MSG_TYPE_AMF0_CMD                             (20)
/** Aggregate */
#define RTMP_MSG_TYPE_AGGREGATE                            (22)

/** amf3 cmd payload offset */
#define RTMP_AMF3_CMD_PAYLOAD_OFFSET                       (1)

/** set chunk size */
#define RTMP_SET_CHUNK_SIZE_MSG_PAYLOAD_SIZE               (4)
/** abort */
#define RTMP_ABORT_MSG_PAYLOAD_SIZE                        (4)
/** acknowledgement */
#define RTMP_ACK_MSG_PAYLOAD_SIZE                          (4)
/** window acknowledgement size */
#define RTMP_WIN_ACK_MSG_PAYLOAD_SIZE                      (4)
/** set peer bandwidth */
#define RTMP_SET_PEER_BANDWIDTH_MSG_PAYLOAD_SIZE           (5)

/** event type size */
#define RTMP_EVENT_TYPE_SIZE                               (2)
/** event payload size */
#define RTMP_EVENT_PAYLOAD_SIZE                            (4)

/** chunk size(4096) */
#define RTMP_CHUNK_SIZE_4K                                 (4096)
/** win ack size */
#define RTMP_WIN_ACK_SIZE                                  (2500000)
/** dynamic */
#define RTMP_LIMIT_TYPE_DYNAMIC                            (2)


/** 
 * @}
 */

/** 
 * @name User Control Msg
 * @{
 */

/** Event Type Size */
#define RTMP_USER_CONTROL_EVENT_TYPE_SIZE                  (2)
/** Stream Begin */
#define RTMP_USER_CONTROL_EVENT_TYPE_STREAM_BEGIN          (0)
/** Stream EOF */
#define RTMP_USER_CONTROL_EVENT_TYPE_STREAM_EOF            (1)
/** Stream Dry */
#define RTMP_USER_CONTROL_EVENT_TYPE_STREAM_DRY            (2)
/** Set Buffer Length */
#define RTMP_USER_CONTROL_EVENT_TYPE_SET_BUF_LEN           (3)
/** Stream Is Record */
#define RTMP_USER_CONTROL_EVENT_TYPE_STREAM_IS_RECORD      (4)
/** Ping Request */
#define RTMP_USER_CONTROL_EVENT_TYPE_PING_REQ              (6)
/** Ping Response */
#define RTMP_USER_CONTROL_EVENT_TYPE_PING_RESP             (7)

/** Stream Begin Data Size */
#define RTMP_STREAM_BEGIN_EVENT_DATA_SIZE                  (4)
/** Stream EOF Data Size */
#define RTMP_STREAM_EOF_EVENT_DATA_SIZE                    (4)
/** Stream Dry Data Size */
#define RTMP_STREAM_DRY_EVENT_DATA_SIZE                    (4)
/** Set Buffer Length Data Size */
#define RTMP_SET_BUF_LEN_EVENT_DATA_SIZE                   (8)
/** Stream Is Record Data Size */
#define RTMP_STREAM_IS_RECORD_EVENT_DATA_SIZE              (4)
/** Ping Request Data Size */
#define RTMP_PING_REQ_EVENT_DATA_SIZE                      (4)
/** Ping Resp Data Size */
#define RTMP_PING_RESP_EVENT_DATA_SIZE                     (4)


/** 
 * @}
 */


/** 
 * @name Cmd Msg
 * @{
 */

/** NetConnection */
#define RTMP_CMD_MSG_OBJ_NET_CONNECTION                    "NetConnection"
/** NetStream */
#define RTMP_CMD_MSG_OBJ_NET_STREAM                        "NetStream"
/** Connect */
#define RTMP_CMD_CONNECT                                   "connect"
/** Call */
#define RTMP_CMD_CALL                                      "call"
/** Close */
#define RTMP_CMD_CLOSE                                     "close"
/** CreateStream */
#define RTMP_CMD_CREATE_STREAM                             "createStream"
/** FCSubscribe */
#define RTMP_CMD_FC_SUBSCRIBE                              "FCSubscribe"
/** _checkbw */
#define RTMP_CMD_CHECK_BW                                  "_checkbw"
/** onbwdone */
#define RTMP_CMD_ON_BW_DONE                                "onBWDone"
/** Result */
#define RTMP_CMD_RESULT                                    "_result"
/** Error */
#define RTMP_CMD_ERROR                                     "_error"
/** Play */
#define RTMP_CMD_PLAY                                      "play"
/** Play2 */
#define RTMP_CMD_PLAY2                                     "play2"
/** DeleteStream */
#define RTMP_CMD_DELETE_STREAM                             "deleteStream"
/** CloseStream */
#define RTMP_CMD_CLOSE_STREAM                              "closeStream"
/** ReceiveAudio */
#define RTMP_CMD_RECEIVE_AUDIO                             "receiveAudio"
/** ReceiveVideo */
#define RTMP_CMD_RECEIVE_VIDEO                             "receiveVideo"
/** Publish */
#define RTMP_CMD_PUBLISH                                   "publish"
/** Seek */
#define RTMP_CMD_SEEK                                      "seek"
/** Pause */
#define RTMP_CMD_PAUSE                                     "pause"
/** onstatus */
#define RTMP_CMD_ON_STATUS                                 "onStatus"
/** |RtmpSampleAccess */
#define RTMP_CMD_SAMPLE_ACCESS                             "|RtmpSampleAccess"


/** app */
#define RTMP_CONNECT_PROP_APP                              "app"
/** flashVer */
#define RTMP_CONNECT_PROP_FLASH_VER                        "flashVer"
/** swfUrl */
#define RTMP_CONNECT_PROP_SWF_URL                          "swfUrl"
/** tcUrl */
#define RTMP_CONNECT_PROP_TC_URL                           "tcUrl"
/** fpad */
#define RTMP_CONNECT_PROP_FPAD                             "fpad"
/** capabilities */
#define RTMP_CONNECT_PROP_CAPABILITIES                     "capabilities"
/** audioCodecs */
#define RTMP_CONNECT_PROP_AUDIO_CODECS                     "audioCodecs"
/** videoCodecs */
#define RTMP_CONNECT_PROP_VIDEO_CODECS                     "videoCodecs"
/** videoFunction */
#define RTMP_CONNECT_PROP_VIDEO_FUNCTION                   "videoFunction"
/** pageUrl */
#define RTMP_CONNECT_PROP_PAGE_URL                         "pageUrl"
/** objectEncoding */
#define RTMP_CONNECT_PROP_OBJECT_ENCODING                  "objectEncoding"
/** code */
#define RTMP_CONNECT_PROP_CODE                             "code"
/** desc */
#define RTMP_CONNECT_PROP_DESC                             "description"
/** details */
#define RTMP_CONNECT_PROP_DETAILS                          "details"
/** clientid */
#define RTMP_CONNECT_PROP_CLIENT_ID                        "clientid"
/** fmsVer */
#define RTMP_CONNECT_PROP_FMS_VER                          "fmsVer"
/** mode */
#define RTMP_CONNECT_PROP_MODE                             "mode"
/** level */
#define RTMP_CONNECT_PROP_LEVEL                            "level"
/** data */
#define RTMP_CONNECT_PROP_DATA                             "data"
/** version */
#define RTMP_CONNECT_PROP_VERSION                          "version"

/** live */
#define RTMP_STR_LIVE                                      "live"

/** status */
#define RTMP_NETWORK_LEVEL_STATUS                          "status"

/** code success */
#define RTMP_CONNECT_CODE_SUCCESS                          "NetConnection.Connect.Success"
/** desc success */
#define RTMP_CONNECT_DESC_SUCCESS                          "Connection succeeded."

/** publish success */
#define RTMP_PUBLISH_CODE_SUCCESS                          "NetStream.Publish.Start"
/** publish success */
#define RTMP_PUBLISH_DESC_SUCCESS                          "is now published."

/** connect transaction id */
#define RTMP_CONNECT_TRANSACTION_ID                        (0x1)

/** net stream play reset */
#define RTMP_NET_STREAM_CODE_PLAY_RESET                    "NetStream.Play.Reset"
/** net stream play reset */
#define RTMP_NET_STREAM_DESC_PLAY_RESET                    "Playing and resetting"

/** net stream play start */
#define RTMP_NET_STREAM_CODE_PLAY_START                    "NetStream.Play.Start"
/** net stream play start */
#define RTMP_NET_STREAM_DESC_PLAY_START                    "Started playing"

/** 
 * @}
 */

/** 
 * @name audio codecs
 * @{
 */

/** none */
#define RTMP_AUDIO_SUPPORT_SND_NONE                        (0x0001)
/** adpcm */
#define RTMP_AUDIO_SUPPORT_SND_ADPCM                       (0x0002)
/** mp3 */
#define RTMP_AUDIO_SUPPORT_SND_MP3                         (0x0004)
/** intel */
#define RTMP_AUDIO_SUPPORT_SND_INTEL                       (0x0008)
/** unused */
#define RTMP_AUDIO_SUPPORT_SND_UNUSED                      (0x0010)
/** nelly8 */
#define RTMP_AUDIO_SUPPORT_SND_NELLY8                      (0x0020)
/** nelly */
#define RTMP_AUDIO_SUPPORT_SND_NELLY                       (0x0040)
/** G711A */
#define RTMP_AUDIO_SUPPORT_SND_G711A                       (0x0080)
/** G711U */
#define RTMP_AUDIO_SUPPORT_SND_G711U                       (0x0100)
/** nelly16 */
#define RTMP_AUDIO_SUPPORT_SND_NELLY16                     (0x0200)
/** aac */
#define RTMP_AUDIO_SUPPORT_SND_AAC                         (0x0400)
/** speex */
#define RTMP_AUDIO_SUPPORT_SND_SPEEX                       (0x0800)
/** all */
#define RTMP_AUDIO_SUPPORT_SND_ALL                         (0x0FFF)


/** 
 * @}
 */



#endif ///__RTMP_CONST_H__
