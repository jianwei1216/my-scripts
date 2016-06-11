#!/bin/sh

manager="/usr/local/digioceanfs_manager/"
gui="/usr/local/digioceanfs_gui/"
client="/usr/local/digioceanfs_client/"

\cp -r /root/git/MGMT/digioceanfs_manager/core/* ${manager}
\cp -r /root/git/MGMT/digioceanfs_gui/core/* ${gui}
\cp -r /root/git/MGMT/digioceanfs_client/core/* ${client}

python /root/git/MGMT/mkpyc.py ${manager}
python /root/git/MGMT/mkpyc.py ${gui}
python /root/git/MGMT/mkpyc.py ${client}

