#!/bin/sh

manager="/usr/local/digioceanfs_manager/"
gui="/usr/local/digioceanfs_gui/"
client="/usr/local/digioceanfs_client/"

\cp -r /root/git/MGMT/digioceanfs_manager/core/* ${manager}
\cp -r /root/git/MGMT/digioceanfs_gui/core/* ${gui}
\cp -r /root/git/MGMT/digioceanfs_client/* ${client}

python /root/git/MGMT/mkpyc.py ${manager}
python /root/git/MGMT/mkpyc.py ${gui}
python /root/git/MGMT/mkpyc.py ${client}

msgfmt -o ${gui}i18n/zh_CN/LC_MESSAGES/messages.mo ${gui}i18n/zh_CN/LC_MESSAGES/messages.po
digioceanfs-gui stop
digioceanfs-gui start

