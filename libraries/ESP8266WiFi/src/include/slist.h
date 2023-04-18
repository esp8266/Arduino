#ifndef SLIST_H
#define SLIST_H

template<typename T>
class SList {
public:
  SList() : _next(0) { }

protected:

  static void _add(T* self) {
    T* tmp = _s_first;
    _s_first = self;
    self->_next = tmp;
  }

  static void _remove(T* self) {
    if (_s_first == self) {
      _s_first = self->_next;
      self->_next = 0;
      return;
    }

    for (T* prev = _s_first; prev->_next; prev = prev->_next) {
      if (prev->_next == self) {
        prev->_next = self->_next;
        self->_next = 0;
        return;
      }
    }
  }

  static T* _s_first;
  T* _next;
};

template<typename T>
T* slist_append_tail(T* head, T* item) {
    if (!head)
        return item;
    T* last = head;
    while(last->next())
        last = last->next();
    last->next(item);
    return head;
}

#endif //SLIST_H
