import thread,sys

char = None
Getch = None


try:
    from msvcrt import getch  # try to import Windows version
    Getch = getch
except ImportError:
    def getch():   # define non-Windows version
        import sys, tty, termios
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch
    Getch = getch
            
def keypress():
    global char
    while(1):
        char = getch()


def listen():
    thread.start_new_thread(keypress, ())

def charPressed():
    global char
    if (char == '\x03') :
        print "keyboard interrupt"
        sys.exit()
    tmp = char
    char = None
    return tmp
    
    
if __name__ == "__main__":
    import time, sys
    i = 0
    listen()
    while 1:
        i += 1
        print i
        mychar = charPressed()
        if (mychar != None):
            print mychar
        time.sleep(1)