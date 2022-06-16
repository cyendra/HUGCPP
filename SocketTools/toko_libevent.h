//
// Created by cyendra on 2022/5/24.
//

#ifndef SOCKETTOOLS_TOKO_LIBEVENT_H
#define SOCKETTOOLS_TOKO_LIBEVENT_H
#define NAMESPACE_TOKO_START namespace toko{
#define NAMESPACE_TOKO_END }
NAMESPACE_TOKO_START

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <iostream>
#include <functional>
#include <vector>
typedef void (*evconnlistener_cb)(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *);



class TokoEventBase{
private:
    event_base *_base;
    std::vector<evconnlistener*> _listener_list;
    
public:
    TokoEventBase() {
        _base = event_base_new();
        if (!_base) {
            std::cout << "Could not initialize libevent!" << std::endl;
        }
    }

    ~TokoEventBase() {
        for (const auto &item: _listener_list) {
            evconnlistener_free(*item);
        }
    }

    bool ConnectAndListen(sockaddr_in& sin) {

        evconnlistener_cb fun;
        evconnlistener* listener = evconnlistener_new_bind(_base, fun, (void *)this,
                                           LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
                                           (struct sockaddr*)&sin,
                                           sizeof(sin));
        if (!listener) {
            fprintf(stderr, "Could not create a listener!\n");
            return false;
        }
        _listener_list.push_back(&listener);
//        _listener_list .push_back(&listener);
        return true;
    }


};



NAMESPACE_TOKO_END
#endif //SOCKETTOOLS_TOKO_LIBEVENT_H
