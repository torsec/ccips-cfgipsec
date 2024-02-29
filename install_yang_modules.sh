#!/bin/bash

set -e

sysrepoctl -i yang/ietf-inet-types.yang  --permissions=666 -v3
sysrepoctl -i yang/ietf-netconf-acm.yang  --permissions=666 -v3
sysrepoctl -i yang/ietf-yang-types.yang  --permissions=666 -v3
sysrepoctl -i yang/ietf-i2nsf-ikec.yang  --permissions=666 -v3
sysrepoctl -i yang/ietf-i2nsf-ikeless.yang  --permissions=666 -v3
sysrepoctl -c ietf-i2nsf-ikeless -e ikeless-notification  -v3
sysrepocfg --import=yang/disable-acm.xml -f xml --datastore startup --module ietf-netconf-acm -v3
sysrepocfg --import=yang/disable-acm.xml -f xml --datastore running --module ietf-netconf-acm -v3
