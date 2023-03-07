#ifndef MSG_CONTENT_H
#define MSG_CONTENT_H

#include "inttypes.h"

typedef struct {
    uint16_t type;      /**< Type field. */
    union {
      void *ptr;        /**< Pointer content field. */
      uint32_t value;   /**< Value content field. */
    };                  /**< Content of the message. */
} msg_content_t;

#endif /* MSG_CONTENT_H */
