#pragma once

#include "xmbus/xevent.h"
#include "xdata/xblock.h"
#include "xstatistic/xbasic_size.hpp"
#include "xstatistic/xstatistic.h"

NS_BEG2(tcash, mbus)

class xevent_account_t : public xbus_event_t, public xstatistic::xstatistic_obj_face_t {
public:
    enum _minor_type_ {
        none,
        add_role,
        remove_role,
    };

    xevent_account_t(_minor_type_ type)
    : xbus_event_t(xevent_major_type_account, type, to_listener, true), xstatistic::xstatistic_obj_face_t(xstatistic::enum_statistic_event_account) {
    }

    ~xevent_account_t() {statistic_del();}

    virtual int32_t get_class_type() const override {return xstatistic::enum_statistic_event_account;}
private:
    virtual size_t get_object_size_real() const override {
        return sizeof(*this) + get_size(get_result_data());
    }
};

using xevent_account_ptr_t = xobject_ptr_t<xevent_account_t>;

class xevent_account_add_role_t : public xevent_account_t {
public:

    xevent_account_add_role_t(
            const std::string &_address,
            direction_type dir = to_listener,
            bool _sync = true) :
    xevent_account_t(add_role),
    address(_address) {
    }

    std::string address;
};

class xevent_account_remove_role_t : public xevent_account_t {
public:

    xevent_account_remove_role_t(
            const std::string &_address,
            direction_type dir = to_listener,
            bool _sync = true) :
    xevent_account_t(remove_role),
    address(_address) {
    }

    std::string address;
};

NS_END2
