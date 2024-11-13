// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#ifndef _tcashIO_SETPROCTITLE_H_INCLUDED_
#    define _tcashIO_SETPROCTITLE_H_INCLUDED_

#    define tcashIO_PROCTITLE_ERROR -1
#    define tcashIO_PROCTITLE_OK 0

int tcashio_init_setproctitle();
void tcashio_setproctitle(const char * title);

#endif /* _tcashIO_SETPROCTITLE_H_INCLUDED_ */
