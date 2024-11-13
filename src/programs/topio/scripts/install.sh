#!/bin/bash
#
# install - install a program, script, or datafile
#
# tcashio: install.sh, 2020-05-25 13:00 by smaug
#
# Copyright tcashNetwork Technology
#
# Permission to use, copy, modify, distribute, and sell this software and its
# documentation for any purpose is hereby granted without fee, provided that
# the above copyright notice appear in all copies and that both that
# copyright notice and this permission notice appear in supporting
# documentation, and that the name of M.I.T. not be used in advertising or
# publicity pertaining to distribution of the software without specific,
# written prior permission.  M.I.T. makes no representations about the
# suitability of this software for any purpose.  It is provided "as is"
# without express or implied warranty.
#
# This script is compatible with the BSD install script, but was written
# from scratch.

pwd

username=`whoami`
if [ $username != 'root' ]; then
    echo "Permission denied, please retry with root"
    return
fi


if [ $SUDO_USER ]
then
    echo "sudo user"
    username=$SUDO_USER
else
    echo "whoami"
    username=`whoami`
fi


echo_and_run() { echo "$*" ; "$@" ; }

tcashio_name='tcashio'

osinfo=`awk -F= '/^NAME/{print $2}' /etc/os-release |awk -F ' ' '{print $1}' |awk -F '\"' '{print $2}'`
osname=`uname`

# using more simple way
which yum
if [ $? != 0 ]; then
    osinfo="Ubuntu"
else
    osinfo="CentOS"
fi

ubuntu_os="Ubuntu"
centos_os="CentOS"
centos_os_version=`cat /etc/os-release  |grep CENTOS_MANTISBT_PROJECT_VERSION |awk -F '"' '{print $2}' `

osname_linux="Linux"
osname_darwin="Darwin"

ntpd_path="/usr/sbin/ntpd"
ntpd_service="ntp.service"

init_os_config() {
    sed -i "/ulimit\s-n/d"     /etc/profile
    sed -i '$a\ulimit -n 65535'        /etc/profile
    source /etc/profile
    echo "set ulimit -n 65535"
}

if [ $osinfo = ${ubuntu_os} ]
then
    echo "Ubuntu"

    init_os_config

    ntpd_service="ntp.service"
    if [ ! -f "$ntpd_path" ]; then
        echo "prepare tcashio runtime environment, please wait for seconds..."
        echo_and_run echo "apt update -y > /dev/null 2>&1" | bash
        echo_and_run echo "apt update -y > /dev/null 2>&1" | bash
        echo_and_run echo "apt install -y ntp > /dev/null 2>&1" | bash
        if [ $? != 0 ]; then
            echo "install ntp failed"
            return
        fi
    fi

elif [ $osinfo = ${centos_os} ]
then
    echo "Centos"

    init_os_config

    if [ $centos_os_version = 7 ]; then
        ntpd_service="ntpd.service"
        if [ ! -f "$ntpd_path" ]; then
            echo "prepare tcashio runtime environment, please wait for seconds..."
            echo_and_run echo "yum update -y > /dev/null 2>&1" | bash
            echo_and_run echo "yum install -y ntp > /dev/null 2>&1" | bash
            if [ $? != 0 ]; then
                echo "install ntp failed"
                return
            fi
        fi
    elif [ $centos_os_version = 8 ]; then
        ntpd_service="chronyd.service"
        echo_and_run echo "yum -y install chrony > /dev/null 2>&1" | bash
    else
        echo "Not Support Centos-Version:$centos_os_version"
        return
    fi
else
    echo "unknow osinfo:$osinfo"
fi


ntpd_status=`systemctl is-active $ntpd_service`
if [ $ntpd_status = "active" ]; then
    echo "ntp service already started"
else
    echo_and_run echo "systemctl enable $ntpd_service" | bash
    run_status=$?
    if [ $run_status != 0 ]; then
        echo "enable $ntpd_service failed: $run_status"
        return
    fi
    echo_and_run echo "systemctl start $ntpd_service" | bash
    run_status=$?
    if [ $run_status != 0 ]; then
        echo "start $ntpd_service failed: $run_status"
        return
    fi
fi

if [ ! -x "$tcashio_name" ]; then
    echo "not found $tcashio_name"
    return
fi

INSTALL_tcashIO_BIN_PATH="/usr/bin/"
mkdir -p ${INSTALL_tcashIO_BIN_PATH}

if [ $osname = ${osname_linux} ]; then
    echo_and_run echo "cp -f $tcashio_name ${INSTALL_tcashIO_BIN_PATH}" |bash
elif [ $osname = ${osname_darwin} ]; then
    echo_and_run echo "cp -f $tcashio_name ${INSTALL_tcashIO_BIN_PATH}" |bash
fi

ldconfig

# register tcashio as service

echo $username

if [ $username = "root" ]
then
    tcashio_home=/$username/tcashnetwork
else
    tcashio_home=/home/$username/tcashnetwork
fi

echo $tcashio_home
echo ""
echo "############now will register tcashio as service##############"
echo ""

tcashIO_SAFEBOX_SERVICE="
[Unit]
Description=the cpp-tcashnetwork command line interface
After=network.target
After=network-online.target
Wants=network-online.target

[Service]
Type=forking
Environment=tcashIO_HOME=$tcashio_home
PIDFile=$tcashio_home/safebox.pid
ExecStart=/usr/bin/tcashio node safebox
PrivateTmp=true

[Install]
WantedBy=multi-user.target"

#printf '%s\n' "$tcashIO_SAFEBOX_SERVICE"
printf '%s\n' "$tcashIO_SAFEBOX_SERVICE" | tee /lib/systemd/system/tcashio-safebox.service
systemctl enable tcashio-safebox.service


timedatectl  set-timezone UTC

which $tcashio_name
if [ $? != 0 ]; then
    echo "install tcashio failed"
    return
fi

echo "install $tcashio_name done, good luck"
echo "now run command to check md5:  tcashio -v"
echo "now run command for help info: tcashio -h"
tcashio node safebox
