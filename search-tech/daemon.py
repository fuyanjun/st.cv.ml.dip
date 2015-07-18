# coding=utf-8
__author__ = 'fuyanjun'


import sys, os, time, atexit, logging
from signal import SIGTERM, SIGKILL
import signal

import time
import socket

class Daemon:
    def __init__(self, logger, pidfile, stdin='/dev/null', stdout='/dev/null', stderr='/dev/null'):
        self.logger = logger
        self.stdin = stdin
        self.stdout = stdout
        self.stderr = stderr
        self.pidfile = pidfile

    def _daemonize(self):
        try:
            pid = os.fork()
            if pid > 0:
                sys.exit(0)
        except OSError, e:
            self.logger.error('fork #1 failed: %d (%s)' % (e.errno, e.strerror))
            sys.exit(1)
        os.setsid()
        os.chdir("/")
        os.umask(0)

        try:
            pid = os.fork()
            if pid > 0:
                sys.exit(0)
        except OSError, e:
            self.logger.error('fork #2 failed: %d (%s)' % (e.errno, e.strerror))
            sys.exit(1)

        sys.stdout.flush()
        sys.stderr.flush()
        si = file(self.stdin, 'r')
        so = file(self.stdout, 'a+')
        se = file(self.stderr, 'a+', 0)
        os.dup2(si.fileno(), sys.stdin.fileno())
        os.dup2(so.fileno(), sys.stdout.fileno())
        os.dup2(se.fileno(), sys.stderr.fileno())

        atexit.register(self.delpid)
        pid = str(os.getpid())
        file(self.pidfile, 'w+').write('%s\n' % pid)

    def delpid(self):
        os.remove(self.pidfile)
        self.logger.info('delpid called')

    def start(self):
        try:
            pf = file(self.pidfile, 'r')
            pid = int(pf.read().strip())
            pf.close()
        except IOError:
            pid = None

        if pid:
            message = 'pidfile %s already exist. Daemon already running?\n'
            print message % self.pidfile
            self.logger.error(message % self.pidfile)
            sys.exit(1)

        self._daemonize()
        self._run()

    def stop(self):
        try:
            pf = file(self.pidfile, 'r')
            pid = int(pf.read().strip())
            pf.close()
        except IOError:
            pid = None

        if not pid:
            message = 'pidfile %s does not exist. Daemon not running?\n'
            print message % self.pidfile
            self.logger.error(message % self.pidfile)
            return

        try:
            while 1:
                os.kill(pid, SIGTERM)
                time.sleep(0.1)
        except OSError, err:
            err = str(err)
            if err.find('No such process') > 0:
                if os.path.exists(self.pidfile):
                    os.remove(self.pidfile)
            else:
                #print str(err)
                self.logger.error(err)
                sys.exit(1)

    def restart(self):
        self.stop()
        self.start()


class MyDaemon(Daemon):
    def _run(self):
        while True:
            self.logger.info('daemon is running')
            time.sleep(5)


if __name__ == '__main__':
    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s %(levelname)s %(message)s',
        filename='/var/log/mydaemon.log')

    daemon = SearchdMoniter(logging.getLogger('info'), '/tmp/searchdmon.pid')
    if len(sys.argv) == 2:
        if 'start' == sys.argv[1]:
            daemon.start()
        elif 'stop' == sys.argv[1]:
            daemon.stop()
        elif 'restart' == sys.argv[1]:
            daemon.restart()
        else:
            print 'Unknown command'
            sys.exit(2)
        sys.exit(0)
    else:
        print 'usage: %s start|stop|restart' % sys.argv[0]
        sys.exit(2)

'''
第一个fork是为了让shell返回，同时让你完成setsid（从你的控制终端移除，这样就不会意外地收到信号）。setsid使得这个进程成为“会话领导（session leader）”，即如果这个进程打开任何终端，该终端就会成为此进程的控制终端。我们不需要一个守护进程有任何控制终端，所以我们又fork一次。在第二次fork之后，此进程不再是一个“会话领导”，这样它就能打开任何文件（包括终端）且不会意外地再次获得一个控制终端

另外说明：
umask()函数为进程设置文件模式创建屏蔽字，并返回以前的值
在shell命令行输入：umask 就可知当前文件模式创建屏蔽字
常见的几种umask值是002，022和027，002阻止其他用户写你的文件，022阻止同组成员和其他用户写你的文件，027阻止同组成员写你的文件以及其他用户读写或执行你的文件
rwx-rwx-rwx 代表是777 所有的人都具有权限读写与执行

chmod()改变文件的权限位
int dup(int filedes) 返回新文件描述符一定是当前文件描述符中的最小数值
int dup2(int filedes, int filedes2);这两个函数返回的新文件描述符与参数filedes共享同一个文件表项。
'''
