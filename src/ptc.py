import socket
import struct
import math

def _socket_readline(sock):
    line = ""
    while not line.endswith('\n'):
        d = sock.recv(1024)
        if not d: continue
        line += d
    return line

class CommunicationError(Exception):
    pass

class ServoError(Exception):
    pass

class PTController(object):

    RAD_PER_ENC = math.radians(360.0) / 0xFFF
    HOME_ENC = 0x7FF
    
    MIN_TILT = math.radians(-55)
    MAX_TILT = math.radians(60)
    MIN_PAN = math.radians(-160)
    MAX_PAN = math.radians(160)

    def __init__(self, ip='192.168.1.60', debug=True, check_response=True):
        self._ip = ip
        self._struct = struct.Struct('<ccBHc')
        self._init_sock()

        self._laser = False
        self._laser_power = int(255)
        self._status_led = False
        self._debug_enable = debug
        self._check_response = check_response

    def _debug(self, msg, send):
        if self._debug_enable:
            print "-->" if send else "<--",
            print " ".join(["{:02X}".format(ord(c)) for c in msg])

    def _init_sock(self):
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._sock.connect((self._ip,23))

    def _send(self, cmd, _type, val):
        def do_send():
            dat = self._struct.pack('@',cmd,_type,val,'=')
            self._debug(dat,send=True)
            self._sock.send(dat)
            dat = _socket_readline(self._sock)
            self._debug(dat,send=False)

            _,cmdok,cmdresp = dat.split(',')
            cmdok = int(cmdok,16)
            cmdresp = int(cmdresp,16)

            if self._check_response:
                if cmdok == 0xFF:
                    raise CommunicationError("command: %s" % cmd)
                elif cmdok != 0x00:
                    raise ServoError("command: %s error: %x" % (cmd,cmdok))
                
            return cmdresp

        try:
            return do_send()
        except socket.error:
            try:
                self._sock.close()
                self._init_sock()
            except:
                pass
            return do_send()

    def _send_angle(self, axis, rad):
        enc_tics = int(math.ceil(rad / self.RAD_PER_ENC))
        enc_tics += self.HOME_ENC
        self._send(axis,0,enc_tics)

    def pan(self, rad=0, deg=None):
        val = min(self.MAX_PAN,
                max(self.MIN_PAN,
                    rad if deg == None else math.radians(deg)))
        self._send_angle('P', val)

    def tilt(self, rad=0, deg=None):
        val = min(self.MAX_TILT,
                max(self.MIN_TILT,
                    rad if deg == None else math.radians(deg)))
        self._send_angle('T', val)


    def _send_laser(self):
        cmd = "L" if self._laser else "l"
        self._send(cmd,0,self._laser_power)

    def brightness(self, level):
        pwr = min(1.0,max(0.0,level))
        self._laser_power = int(pwr*255.0)
        self._send_laser()

    def wait_for_movement(self, enable):
        return self._send('W' if enable else 'w',0,0)

    def ping(self, id):
        self._send('p',0,id)

    @property
    def laser(self):
        return self._laser
    @laser.setter
    def laser(self, val):
        self._laser = True if val else False
        self._send_laser()

    @property
    def status_led(self):
        return self._laser
    @status_led.setter
    def status_led(self, val):
        cmd = "S" if val else "s"
        self._status_led = cmd == "S"
        self._send(cmd,0,0)

    @property
    def temperature(self):
        return self._send('t',0,0)

    @property
    def voltage(self):
        return self._send('v',0,0)

if __name__ == "__main__":
    import random, time

    try:
        __IPYTHON__
        inside_ipy = True
    except NameError:
        inside_ipy = False

    ptc = PTController(debug=True, check_response=False)
    ptc.wait_for_movement(True)
    while not inside_ipy:
        ptc.pan(deg=random.randint(-80,80))
        ptc.tilt(deg=random.randint(-30,30))
        temp = ptc.temperature
        volt = ptc.voltage
        print ""
        print "T=",temp
        print "V=",volt
        time.sleep(0.5)

