/**
 * @file gen_error.h  General Error Constant
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.04.26
 *
 *
 */

#ifndef __GEN_ERROR_H__
#define __GEN_ERROR_H__

/** 
 * @name success
 * @{
 */

/** success */
#define GEN_SUCCESS                                        (0)
/** make success result */
#define GEN_SUCCESS_RESULT(code)                           (GEN_SUCCESS + code)
/** ok */
#define GEN_S_OK                                           GEN_SUCCESS_RESULT(0)

/** 
 * @}
 */

/** 
 * @name error
 * @{
 */

/** error */
#define GEN_ERROR                                          (0x80000000)
/** make error result */
#define GEN_ERROR_RESULT(code)                             (GEN_ERROR + code)
/** fail */
#define GEN_E_FAIL                                         GEN_ERROR_RESULT(0x0)
/** invalid param */
#define GEN_E_INVALID_PARAM                                GEN_ERROR_RESULT(0x1)
/** not enough memory */
#define GEN_E_NOT_ENOUGH_MEM                               GEN_ERROR_RESULT(0x2)
/** not initialize */
#define GEN_E_NOT_INIT                                     GEN_ERROR_RESULT(0x3)
/** need more data */
#define GEN_E_NEED_MORE_DATA                               GEN_ERROR_RESULT(0x4)
/** already exist */
#define GEN_E_ALREADY_EXIST                                GEN_ERROR_RESULT(0x5)
/** not exist */
#define GEN_E_NOT_EXIST                                    GEN_ERROR_RESULT(0x6)
/** not found */
#define GEN_E_NOT_FOUND                                    GEN_ERROR_RESULT(0x7)
/** not support */
#define GEN_E_NOT_SUPPORT                                  GEN_ERROR_RESULT(0x8)
/** not implement */
#define GEN_E_NOT_IMPLEMENT                                GEN_ERROR_RESULT(0x9)
/** invalid status */
#define GEN_E_INVALID_STATUS                               GEN_ERROR_RESULT(0xA)
/** inner error */
#define GEN_E_INNER_ERROR                                  GEN_ERROR_RESULT(0xB)
/** need more buf */
#define GEN_E_NEED_MORE_BUF                                GEN_ERROR_RESULT(0xC)
/** input overflow */
#define GEN_E_INPUT_OVERFLOW                               GEN_ERROR_RESULT(0xD)
/** input underflow */
#define GEN_E_INPUT_UNDERFLOW                              GEN_ERROR_RESULT(0xE)
/** output overflow */
#define GEN_E_OUTPUT_OVERFLOW                              GEN_ERROR_RESULT(0xF)
/** output underflow */
#define GEN_E_OUTPUT_UNDERFLOW                             GEN_ERROR_RESULT(0x10)
/** io pending */
#define GEN_E_IO_PENDING                                   GEN_ERROR_RESULT(0x11)
/** access deny */
#define GEN_E_ACCESS_DENY                                  GEN_ERROR_RESULT(0x12)
/** wrong format */
#define GEN_E_WRONG_FORMAT                                 GEN_ERROR_RESULT(0x13)
/** open fail */
#define GEN_E_OPEN_FAIL                                    GEN_ERROR_RESULT(0x14)
/** read fail */
#define GEN_E_READ_FAIL                                    GEN_ERROR_RESULT(0x15)
/** write fail */
#define GEN_E_WRITE_FAIL                                   GEN_ERROR_RESULT(0x16)
/** not enough resource */
#define GEN_E_NOT_ENOUGH_RESOURCE                          GEN_ERROR_RESULT(0x17)
/** io limited */
#define GEN_E_IO_LIMITED                                   GEN_ERROR_RESULT(0x18)

/** 
 * @}
 */

#endif //__GEN_ERROR_H__
