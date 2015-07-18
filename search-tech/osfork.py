# coding=utf-8
__author__ = 'fuyanjun'

import os


def child():
    print 'A new child:', os.getpid()
    print 'Parent id is:', os.getppid()
    os._exit(0)


def parent():
    while True:
        newpid = os.fork()
        print newpid
        if newpid == 0:
            child()
        else:
            pids = (os.getpid(), newpid)
            print "parent:%d,child:%d" % pids
            print "parent parent:", os.getppid()
        if raw_input() == 'q':
            break


parent()
'''
加载了os模块之后，parent函数中fork()函数生成了一个子进程(子进程从fork处开始往下执行)，返回值newpid有两个，一个为0，用以表示子进程，
一个是大于0的整数，用以表示父进程，这个数正是子进程的pid. 通过print语句可以清晰看到两个返回值。如果fork()返回值是一个负值，则表明子
进程生成不成功(这个简单程序中没有考虑这种情况)。如果newpid==0，则表明进入到了子进程，也就是child()函数中，在子进程中输出了自己的id
和父进程的id。如果进入了else语句， 则表明newpid>0，进入到父进程中，在父进程中os.getpid()得到自己的id,fork()返回值newpid表示了子
进程的id,同时输出了父进程的父进程的id. 通过实验我们可以看到if和else语句的执行顺序是不确定的，子、父进程的执行顺序由操作系统的调度算
法来决定。
'''
