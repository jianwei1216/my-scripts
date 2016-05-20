#!/usr/bin/python
# -*- coding:UTF-8 -*-

import os
import thread
import time
import sys

class Cluster:
        url = ""
        gitdir = ""
        workdir = ""
        nodes = ()

        def __init__(self, gitdir, url, node):
                self.url = url
                self.gitdir = gitdir
                tmpstr = self.url.split("/")
                if cmp(self.url[-1], "/") == 0:
                        tmpdir = tmpstr[-2]
                else:
                        tmpdir = tmpstr[-1]

                if cmp(self.gitdir[-1], "/") == 0:
                        currdir = self.gitdir + tmpdir
                else:
                        currdir = self.gitdir + "/" + tmpdir
                self.workdir = currdir
                self.nodes = node
                print self.url, self.gitdir, self.workdir, self.nodes

        def __clone(self, host, lock):
                clone_cmd = "ssh " + host + " " + "\"" + "cd " + self.gitdir + ";"\
                            + "git clone " + self.url + "\""
                print clone_cmd
                os.system(clone_cmd)
                lock.release()
                
        def __pull(self, host, lock):
                pull_cmd = "ssh " + host + " \"" + "cd " + self.workdir + ";"\
                           + "git pull;" + "\""
                os.system(pull_cmd)
                lock.release()

        def __install(self, host, lock):
                autogen = "./autogen.sh;"
                config = "./configure --build=x86_64-unknown-linux-gnu "\
                         "--host=x86_64-unknown-linux-gnu "\
                         "--target=x86_64-redhat-linux-gnu "\
                         "--program-prefix= --prefix=/usr "\
                         "--exec-prefix=/usr --bindir=/usr/bin "\
                         "--sbindir=/usr/sbin --sysconfdir=/etc "\
                         "--datadir=/usr/share --includedir=/usr/include "\
                         "--libdir=/usr/lib64 --libexecdir=/usr/libexec "\
                         "--localstatedir=/var --sharedstatedir=/var/lib "\
                         "--mandir=/usr/share/man --infodir=/usr/share/info "\
                         "--enable-systemtap=no --enable-debug --enable-bd-xlator;"
                make = "make -j8;make install;"

                install_cmd = "ssh " + host + " \"" + "cd " + self.workdir + ";"\
                            + autogen + config + make + "\""
                print install_cmd
                os.system (install_cmd)
                lock.release()

        def __uninstall(self, host, lock):
                uninstall_cmd = "ssh " + host + " \"" + "cd " + self.workdir + ";"\
                            + "make uninstall; make clean" + "\""
                os.system (uninstall_cmd)
                lock.release()

        def __rm_work_dir(self, host, lock):
                rm_cmd = "ssh " + host + " \"" + "rm -rf " + self.workdir + "\""
                os.system (rm_cmd)
                lock.release()

        def __multi_thread(self, func):
                locks = []
                for host in self.nodes:
                        try:
                                lock = thread.allocate_lock()
                                lock.acquire()
                                locks.append(lock)
                                thread.start_new_thread(func, (host, lock))
                        except:
                                print "Error: unable to start thread!"
                for i in locks:
                        while i.locked():
                                pass

        def clone(self):
                self.__multi_thread(self.__clone)

        def pull(self):
                self.__multi_thread(self.__pull)

        def install(self):
                self.__multi_thread(self.__install)

        def uninstall(self):
                self.__multi_thread(self.__uninstall)

        def rm_work_dir(self):
                self.__multi_thread(self.__rm_work_dir)

#class Mgmt(Cluster):
#        digioceanfs_client = "/usr/local/digioceanfs_client/"
#        digioceanfs_gui = "/usr/local/digioceanfs_gui/"
#        digioceanfs_manager = "/usr/local/digioceanfs_manager/utils/"
#        def __mkpyc(self):
#                copy_cmd = "ssh " + host + " \"" + "cd " + self.digioceanfs_client + ";" + "\""
#                os.system (rm_cmd)
#                lock.release()
#
#        def mkpyc(self):
#
def cluster_test(gitdir, giturl, nodes, cmd):
        c = Cluster(gitdir, giturl, nodes) 
        if cmd == "clone":
                c.clone()
        elif cmd == "pull":
                c.pull()
        elif cmd == "install":
                c.install()
        elif cmd == "uninstall":
                c.uninstall()
        elif cmd == "rm":
                c.rm_work_dir()
        else:
                print "cmd: clone|pull|install|uninstall|rm"
                exit()

def mgmt_test(gitdir, giturl, nodes, cmd):
        m = Mgmt(gitdir, giturl, nodes)
        m.mkpyc()

if __name__ == "__main__":
        if len(sys.argv) < 2:
                print "python cluster.py gitdir giturl nodes [clone|pull|install|uninstall]\n"\
                      "e.g. python cluster.py",\
                      "/root/git 10.10.12.16:/root/git/glusterfs node-{1,2,3} clone\n"\
                      "Notes:\nwhen all these args given, will record in the ./.cluster.py.config,\n"\
                      "if not, read the config file to get these args\n"
                exit()

        config_file_pathname = './.cluster.py.config'
        gitdir = ''
        giturl = ''
        nodes = ()
        cmd = sys.argv[-1]
        if len(sys.argv) >= 5:
                gitdir = sys.argv[1]
                giturl = sys.argv[2]
                nodes = tuple(sys.argv[3:-1])

        # judge file exists or not
        has_config_file = os.path.isfile(config_file_pathname)
        if ('' == gitdir) or ('' == giturl) or (() == nodes):
                # if config_file_is_not_exists, fatal error
                if has_config_file == False:
                        print 'Error: don\'t have ' + config_file_pathname +\
                              '!Please give all args!!!'
                        exit(-1)
                # if config_file_is_exists, read file and get these args
                fp = open (config_file_pathname, 'rb')
                fargs = fp.readline()
                args = fargs.split(' ')
                if len(args) < 3:
                        print 'Fatal error: cannot read the correct args from '\
                              ' the ./.cluster.py.config, please give all args!'
                        exit(-1)
                gitdir = args[0]
                giturl = args[1]
                nodes = tuple(args[2:])
        else:
                # need update config_file.
                if has_config_file == False:
                        os.mknod(config_file_pathname)
                fp = open (config_file_pathname, 'wb')
                fp.truncate(0)
                fp.write(gitdir + ' ')
                fp.write(giturl + ' ')
                for i in range(0, len(nodes)):
                        if i == len(nodes)-1:
                                fp.write(nodes[i])
                        else:
                                fp.write(nodes[i] + ' ')

        fp.close()
        cluster_test (gitdir, giturl, nodes, cmd)

