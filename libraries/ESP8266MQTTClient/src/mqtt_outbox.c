/*
* @Author: Tuan PM
* @Date:   2016-10-02 09:45:51
* @Last Modified by:   TuanPM
* @Last Modified time: 2016-11-27 11:41:57
*/

#include "mqtt_outbox.h"
static mqtt_outbox *ob_set_data(mqtt_outbox *ob, char *data, int len, int msg_id, int msg_type, int tick, int remove_on_sent)
{
    ob->buffer = malloc(len);
    if(ob->buffer == NULL)
        return NULL;
    ob->len = len;
    ob->msg_id = msg_id;
    ob->msg_type = msg_type;
    ob->retry_count = 0;
    ob->tick_created = tick;
    ob->next = NULL;
    ob->prev = NULL;
    ob->pending = 0;
    ob->remove_on_sent = remove_on_sent;
    memcpy(ob->buffer, data, len);
    return ob;
}
mqtt_outbox *ob_create()
{
    mqtt_outbox *ob = (mqtt_outbox *) malloc(sizeof(mqtt_outbox));
    if(ob)
        memset(ob, 0, sizeof(mqtt_outbox));
    return ob;
}

mqtt_outbox *ob_get_oldest_no_pending(mqtt_outbox *ob)
{
    mqtt_outbox *oldest = ob->next;
    while(oldest != NULL && oldest->pending == 1) {
        oldest = oldest->next;
    }
    if(oldest != NULL && oldest->pending == 0)
        return oldest;
    return NULL;
}
mqtt_outbox *ob_get_top(mqtt_outbox *ob)
{
    mqtt_outbox *top = ob;
    while(top->next != NULL) {
        top = top->next;
    }
    return top;
}


mqtt_outbox *ob_put(mqtt_outbox *ob, uint8_t *data, int len, int msg_id, int msg_type, int tick, int remove_on_sent)
{
    mqtt_outbox *top = ob_get_top(ob);
    top->next = ob_create();
    if(top->next) {
        ob_set_data(top->next, data, len, msg_id, msg_type, tick, remove_on_sent);
        top->next->prev = top;
    }
    return top->next;
}

mqtt_outbox *ob_get(mqtt_outbox *ob, int msg_id)
{
    mqtt_outbox *found = ob->next;
    while(found != NULL) {
        if(found->msg_id == msg_id) {
            // found->retry_count ++;
            return found;
        } else {
            found = found->next;    //ignore root
        }
    }
    return NULL;
}
mqtt_outbox *ob_del_ob(mqtt_outbox *del)
{
    mqtt_outbox *next = NULL;
    if(del->next) {
        next = del->next;
        del->prev->next = del->next;
        del->next->prev = del->prev;
    } else {
        del->prev->next = NULL;
    }
    free(del->buffer);
    free(del);
    return next;
}
mqtt_outbox *ob_del_id_type(mqtt_outbox *ob, int msg_id, int msg_type)
{
    mqtt_outbox *found = ob->next;
    while(found != NULL) {
        if(found->msg_id == msg_id && found->msg_type == msg_type) {
            return ob_del_ob(found);
        } else {
            found = found->next;
        }
    }
    return NULL;
}
int ob_del_id(mqtt_outbox *ob, int msg_id)
{
    int deleted = 0;
    mqtt_outbox *found = ob->next;
    while(found != NULL) {
        if(found->msg_id == msg_id) {
            deleted ++;
            found = ob_del_ob(found);
        } else {
            found = found->next;
        }
    }
    return deleted;
}
mqtt_outbox *ob_del_oldest(mqtt_outbox *ob)
{
    mqtt_outbox *oldest = ob->next;
    if(oldest) {
        return ob_del_ob(oldest);
    }
    return NULL;
}

int ob_del_expired(mqtt_outbox *ob, int current_tick, int timeout)
{
    int del_count = 0;
    mqtt_outbox *found = ob->next;
    while(found != NULL) {
        if(current_tick - found->tick_created > timeout) {
            found = ob_del_ob(found);
            del_count ++;
        } else {
            found = found->next;
        }
    }
    return del_count;
}

int ob_get_size(mqtt_outbox *ob)
{
    mqtt_outbox *found = ob->next;
    int sz = 0;
    while(found != NULL) {
        sz += found->len;
        found = found->next; 
    }
    return sz;
}
int ob_cleanup(mqtt_outbox *ob, int max_size)
{
    while(ob_get_size(ob) > max_size) {
        ob_del_oldest(ob);
    }
    return ob_get_size(ob);
}
void ob_destroy(mqtt_outbox *ob)
{
    while(ob_del_oldest(ob));
    free(ob);
}
