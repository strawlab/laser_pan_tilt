import socket
import struct
import math

class PTController(object):

    RAD_PER_ENC = math.radians(360.0) / 0xFFF
    HOME_ENC = 0x7FF

    def __init__(self, ip='192.168.1.60'):
        self._ip = ip
        self._struct = struct.Struct('<ccBHc')
        self._init_sock()

    def _init_sock(self):
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._sock.connect((self._ip,23))

    def _send(self, cmd, _type, val):
        def do_send():
            self._sock.send(self._struct.pack('@',cmd,_type,val,'='))
        try:
            do_send()
        except socket.error:
            try:
                self._sock.close()
                self._init_sock()
            except:
                pass
            do_send()

    def _send_angle(self, axis, rad):
        enc_tics = int(math.ceil(rad / self.RAD_PER_ENC))
        enc_tics += self.HOME_ENC
        print "angle",axis,enc_tics
        self._send(axis,0,enc_tics)

    def pan(self, rad=0, deg=None):
        self._send_angle('P', rad if deg == None else math.radians(deg))

    def tilt(self, rad=0, deg=None):
        self._send_angle('T', rad if deg == None else math.radians(deg))

