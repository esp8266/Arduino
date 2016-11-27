#ifndef _OUTBOX_H_
#define _OUTBOX_H_
#include "c_types.h"
#ifdef  __cplusplus
extern "C" {
#endif
typedef struct mqtt_outbox {
    char *buffer;
    int len;
    int msg_id;
    int msg_type;
    int tick_created;
    int retry_count;
    int pending;
    int remove_on_sent;
    struct mqtt_outbox *next;
    struct mqtt_outbox *prev;
} mqtt_outbox;

mqtt_outbox *ob_create();
mqtt_outbox *ob_put(mqtt_outbox *ob, uint8_t *data, int len, int msg_id, int msg_type, int tick, int remove_on_sent);
mqtt_outbox *ob_get(mqtt_outbox *ob, int msg_id);
int ob_del_id(mqtt_outbox *ob, int msg_id);
mqtt_outbox *ob_del_id_type(mqtt_outbox *ob, int msg_id, int msg_type);
mqtt_outbox *ob_del_ob(mqtt_outbox *del) ;
mqtt_outbox *ob_del_oldest(mqtt_outbox *ob);
mqtt_outbox *ob_get_oldest_no_pending(mqtt_outbox *ob);
int ob_del_expired(mqtt_outbox *ob, int current_tick, int timeout);
int ob_get_size(mqtt_outbox *ob);
int ob_cleanup(mqtt_outbox *ob, int max_size);
void ob_destroy(mqtt_outbox *ob);
mqtt_outbox *ob_get_top(mqtt_outbox *ob);
#ifdef  __cplusplus
}
#endif
#endif
