#!/bin/sh
# Usage: <LTPM_name>
# Clears matches for LTPM_name from database

echo "delete from scores where LTPM_name = '$1'" | mysql -u LTPM LTPM
