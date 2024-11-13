#!/bin/bash

username=`whoami`
if [ $username = "root" ]
then
    tcashio_home=/$username/tcashnetwork
else
    tcashio_home=/home/$username/tcashnetwork
fi

if [ -z ${tcashIO_HOME} ];then
sed -i "/tcashIO_HOME/d"     ~/.bashrc
sed -i '$a\export tcashIO_HOME='$tcashio_home        ~/.bashrc
source ~/.bashrc
else
echo $tcashIO_HOME
fi