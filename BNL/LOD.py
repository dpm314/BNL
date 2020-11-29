from PyQt4 import QtCore, QtGui
import winsound
import msvcrt
import serial as pys
import numpy as np
from collections import deque
import Queue
import sys
import struct
import threading
import time
import numpy as np
import datetime
import os
import threading
import cPickle as pickle
import csv

DEFAULT_COMPORT  = 'COM1'
DEFAULT_BAUDRATE = 230400
CSV_HEADER = ['MCU Timestamp (sec)', 'PC Timestamp (sec)', 'Time Since Epoch (sec)']
MAGIC_START_STRING = '\xa5\xa5\xa5\xa5'
MAGIC_END_STRING   = '\xb6\xb6\xb6\xb6'

TX_NEXT_IS_CHAR  = chr(5)  # 0x05
TX_NEXT_IS_FLOAT = chr(6)  # 0x06
TX_CHARFLAG_TEST = chr(10) #0x0A

MESSAGE_TYPES_TOASCII = {
                1:'LOG',
                2:'ASCII_DATA',
                3:'BINARY_FLOAT',
                4:'FLAG'}


MESSAGE_FLAGS_TOASCII ={
						11:'MESSAGE_FLAG_NO_FLAG',
                        99:'TIMESTAMP',
                        100:'MESSAGE_FLAG_LOP_DETECTED',
                        101:'MESSAGE_FLAG_LOP_COUNTER'
                        }

FLOAT_FLAGS_TOASCII = { 65:'FLOAT_FLAG_NEW_MODE',
                        66:'TODO'
                        }

TX_FLAGS            = {'AUTOSTART':'a',                         #0x61 = 97
                       }

FLOAT_FLAGS_TONUM = dict( (v,k) for k,v in FLOAT_FLAGS_TOASCII.iteritems() )
MESSAGE_FLAGS_TONUM = dict( (v,k) for k,v in MESSAGE_FLAGS_TOASCII.iteritems() )

DONT_PRINT_PACKETS = [ MESSAGE_FLAGS_TONUM['TIMESTAMP'], MESSAGE_FLAGS_TONUM['MESSAGE_FLAG_LOP_COUNTER'] ]
PACKET_TIMESTAMP_TO_SECONDS = 100.0 


def printUsage():
    print 'Usage: python lopCL_main.py [comport] [baudRate]'
    print 'Example: python lopCL_main.py com14 57600'
    print 'Available Com Ports: '
    print serial_ports()
    sys.exit()
def serial_ports():
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = pys.Serial(port)
            s.close()
            result.append(port)
        except (OSError, pys.SerialException):
            pass
    return result
####################################################

class packetParserThread(QtCore.QThread):
    def __init__(self, manager):
        print ' packetParserThread: __init__() '
        QtCore.QThread.__init__(self)
        self.packetQueue = manager.packetQueue
        self.daemon = True
        self.PACKETTHREAD_RUNNING = True
        self.manager = manager

    def run(self):
        while(self.PACKETTHREAD_RUNNING):
            self.parsePacket( self.packetQueue.get() ) #Queue.Queue.get() will block the thread by itself
        print 'PACKET PARSING THREAD SHUTTING DOWN'

    def parsePacket(self, aPacket):
        #print aPacket
        if MESSAGE_FLAGS_TOASCII[aPacket.messageFlag] == 'MESSAGE_FLAG_LOP_DETECTED':
            self.manager.addLopRecord( aPacket.messageTimeStampMS/PACKET_TIMESTAMP_TO_SECONDS )
            self.manager.reportLop()

class Packet(object):
    def __init__(self, manager):
        self.manager = manager
        self.headerMagicNumber = []
        self.tailMagicNumber   = []
        self.messageType = []
        self.messageFlag = []
        self.messageLengthBytes = []
        self.messageTimeStampMS = 0
        self.payload = [] #will be list of 4-Byte packets
        self.tail = []
        self.checkSum = False #initialize to failed
        self.headerLengthBytes = 8 #header magic number (4 bytes) and header data (4 bytes)
        self.headMagicNumberLengthBytes = 4
        self.tailMagicNumberLengthBytes = 4
        self.tailLengthBytes = 0
        self._readHeader()

    def __str__(self):
        messageString = ''
        try:
            messageString = messageString + '*\t' + 'Message Type: ' + MESSAGE_TYPES_TOASCII[self.messageType] + '\n'
        except KeyError:
            print 'Message Type Not Understood with self.messageType: ' + str(self.messageType)
        try:
            messageString = messageString + '*\t' + 'Message Flag: ' + MESSAGE_FLAGS_TOASCII[self.messageFlag] + '\n'
        except KeyError:
            print 'Message Type Not Understood with self.Message Flag: ' + str(self.messageFlag)
        messageString = messageString + '*\t' + 'Message Length Bytes: ' + str(self.messageLengthBytes) + '\n'
        messageString = messageString + '*\t' + 'TimeStamp (ms): ' + str(self.messageTimeStampMS) + '\n'
        messageString = messageString + '*\t' + 'Payload: '  + str(self.payload)  + '\n'
        messageString = messageString + '*\t' + 'Checksum: ' + str(self.checkSum) + '\n'
        return messageString

    def __getstate__(self):
        odict = self.__dict__.copy()
        del odict['manager']
        return odict
    def _readTail(self):
        #Will probably be a 4 byte checksum and a (different) 4 byte magic number to verify termination
        #self.tail = self.manager.comPortBuffer.popOldestBytes(self.tailLengthBytes)
        self.tailMagicNumber = self.manager.comPortBuffer.popOldestBytes(self.tailMagicNumberLengthBytes)

    def _verifyCheckSum(self):
        self.checkSum = ( self.tailMagicNumber == MAGIC_END_STRING )
        return self.checkSum

    def _readHeader(self):
        #globalThreadLock.acquire()
        self.headerMagicNumber = self.manager.comPortBuffer.buffer[0] + self.manager.comPortBuffer.buffer[1] + self.manager.comPortBuffer.buffer[2] + self.manager.comPortBuffer.buffer[3]
        self.messageType = ord( self.manager.comPortBuffer.buffer[4] )
        self.messageFlag = ord( self.manager.comPortBuffer.buffer[5] )
        self.messageLengthBytes = struct.unpack('=H',self.manager.comPortBuffer.buffer[6] + self.manager.comPortBuffer.buffer[7] )[0]
        self.messageTimeStampMS   = struct.unpack('=L',self.manager.comPortBuffer.buffer[8] + self.manager.comPortBuffer.buffer[9] + self.manager.comPortBuffer.buffer[10] + self.manager.comPortBuffer.buffer[11])[0]
        #globalThreadLock.release()

    def _readLogPayload(self):
        #globalThreadLock.acquire()
        self.payload = self.manager.comPortBuffer.popOldestBytes(self.messageLengthBytes)
        #globalThreadLock.release()

    def _readFloatArrayPayload(self):
        self.payload = np.zeros( self.messageLengthBytes / 4 )
        for i in range(len(self.payload)):
            #self.manager.updateHardwareCOMBufferLength()
            #globalThreadLock.acquire()
            self.payload[i] = struct.unpack('=f',self.manager.comPortBuffer.popOldestBytes(4) )[0]
            #globalThreadLock.release()

    def readPacket(self):
        #pop magic number and header off com port buffer
        self.manager.comPortBuffer.popOldestBytes( self.headMagicNumberLengthBytes )
        self.manager.comPortBuffer.popOldestBytes( self.headerLengthBytes )
        #read payload, popping data as it is parsed
        if MESSAGE_TYPES_TOASCII[self.messageType] == 'LOG':   #ASCII Log Message
            self._readLogPayload()
        elif self.messageType == 3: #Binary float array 
            self._readFloatArrayPayload()
        elif self.messageType == 2: #ASCII Data
            pass #Not implemented, will need to search for end-of-line etc. probably don't need anymore since we can add floats to log messages 
        elif self.messageType == 4:
            pass #don't read payload, there is no payload for FLAG messages
        self._readTail()
        self._verifyCheckSum()

class comPortBufferThread(threading.Thread):
    def __init__(self, manager):
        print ' comPortBufferThread: __init__()'
        threading.Thread.__init__(self)
        self.manager = manager
        self.comPort = manager.PSoC
        self.failedPacketCount = 0
        self.buffer = deque()
        self.daemon = True
        self.COMTHREAD_RUNNING = True

    def run(self):
        while(self.COMTHREAD_RUNNING):
            numBytesReady = self.comPort.inWaiting()
            if numBytesReady > 0:
                line = self.comPort.read( numBytesReady )
                for c in line:
                    self.buffer.append(c)

            if( (len(self.buffer) > (12) ) and ( self._headIsMagicNumber() )): # 12 = newPacket.headMagicNumberLengthBytes + newPacket.headerLengthBytes. NEED to change this if header modified
                newPacket = Packet(self.manager) # does not pop from deque!
                                                 # does populate header information
                if( len(self.buffer) >= (newPacket.headMagicNumberLengthBytes + newPacket.headerLengthBytes + newPacket.messageLengthBytes + newPacket.tailLengthBytes + newPacket.tailMagicNumberLengthBytes) ):
                    newPacket.readPacket() #pops all of this packet off comPort buffer deque (including head and tail magic numbers, header, payload and tail. 
                    if( newPacket.checkSum == True ):
                        self.manager.packetQueue.put( newPacket )
                    else:
                        #Todo: raise an exception to be caught by main thread 
                        #self.manager.packetQueue.put( newPacket )
                        self.failedPacketCount = self.failedPacketCount + 1
                        #self.manager.updateFailedPacketCount()
                        print 'CHECKSUM FAILED on packet:'
                        print newPacket
                        for _ in range(10):
                            print '---------'
            else:
                if( (len(self.buffer) >= 4) and (not self._headIsMagicNumber() )):
                    try:
                        #print' popping: ' + self.buffer.popleft()
                        self.buffer.popleft()
                    except:
                        print 'error popping from comPortBuffer'
                    #could add>> while(( (len(self.buffer) >= 4) and (not self._headIsMagicNumber())):
                    #                self.buffer.popleft()

    def popNewestBytes(self,N):
        byteList = ''
        for _ in xrange(N):
            byteList = byteList + self.buffer.pop()
        return byteList

    def popOldestBytes(self,N):
        #globalThreadLock.acquire()
        byteList = ''
        try:
            for _ in xrange(N):
                byteList = byteList + self.buffer.popleft()
        except:
            print 'TRIED TO POP MORE BYTES THAN EXIST!'
            byteList = ''
        #globalThreadLock.release()
        return byteList
                
    def _headIsMagicNumber(self):
        return (MAGIC_START_STRING == self.buffer[0] + self.buffer[1] + self.buffer[2] + self.buffer[3])

class TX_Uart_Driver():
    def __init__(self, comPort, manager):
        self.comPort = comPort
        self.manager = manager

    def sendChar(self, charMessage):
        self.comPort.write( TX_NEXT_IS_CHAR )
        self.comPort.write( str(charMessage) )

    def sendFloat(self, aFloat, floatFlag):
        self.comPort.write( TX_NEXT_IS_FLOAT )
        self.comPort.write( str(floatFlag) )
        floatAsChars = np.float32(aFloat).tostring()
        for c in floatAsChars:
            self.comPort.write( str(c) )

class LOP_CL_Manager():
    def __init__(self,comPort = DEFAULT_COMPORT, baudRate = DEFAULT_BAUDRATE):
        self.packetQueue = Queue.Queue()
        self._initPSoC(comPort,baudRate)
        
        self.LOP_Records = []
        self.LOPFileOpen = 0
        
        self._initFiles()
        
        self.TX_Uart_Driver = TX_Uart_Driver(self.PSoC, self)
        self.comPortBuffer = comPortBufferThread(self)
        self.packetParser  = packetParserThread(self)
        
        self.RUNNING = False

    def _initFiles(self):
        self.filePrefix = os.getcwd() #Could put custom file path prefix here to point to specific data directory
        self.todayString = self._makeTodayString()
        self.nowString   = self._makeNowString()
        LOPFileName = self.filePrefix + '\\' + self.todayString + '\\' + self.nowString + '_LOP_TimeStamps.csv'
        if not os.path.exists(self.todayString):
            os.makedirs(self.filePrefix + '\\' + self.todayString + '\\')
        self.LOPFile = open(LOPFileName, 'wb', 512)
        self.csvWriter = csv.writer(self.LOPFile, delimiter = ',')
        self.csvWriter.writerow(CSV_HEADER)
        self.LOPFileOpen = 1

    def addLopRecord(self, PSoC_cpuTimestamp_sec = -1.0):
        aLopRecord = LOP_Record(PSoC_cpuTimestamp = PSoC_cpuTimestamp_sec)
        self.LOP_Records.append(aLopRecord)
        self._writeToLOPFile(aLopRecord)
        #print aLopRecord

    def _writeToLOPFile(self,aRecord):
        self.csvWriter.writerow( [str(aRecord.PSoC_cpuTimestamp), str(aRecord.PC_timeStamp), str(aRecord.secSinceEpoch)] )
        #if (self.logFileOpen):
        #    self.logFile.write( aRecord.__str__() )
        #if (self.LOPFileOpen):
        #    self.LOPFile.write( str(aRecord.secSinceEpoch) + '\n' )

    def reportLop(self):
        print self.LOP_Records[-1]
        for _ in range(4):
            winsound.Beep(3520,250)
            #winsound.Beep(2637,100)
        #print '\a'

        

    def go(self):
        """ Does not return until LOP_CL_Manager:: self.RUNNING = 0 """
        self.RUNNING = True

        self.comPortBuffer.COMTHREAD_RUNNING   = True
        self.comPortBuffer.start()
        
        self.packetParser.PACKETTHREAD_RUNNING = True
        self.packetParser.start()
        time.sleep(.5) # give threads a chance to startup
        #print 'All helper threads started'

        while(self.RUNNING):
            time.sleep(1)
            print '.'

        self.RUNNING = 0
        self.closeEvent()
    
    def _makeTodayString(self):
        now = datetime.datetime.now()
        todayString = str(now.year).zfill(4) + '_' + str(now.month).zfill(2) + '_' + str(now.day).zfill(2)
        return todayString
    def _makeNowString(self):
        now = datetime.datetime.now()
        nowString = str(now.hour).zfill(2) + str(now.minute).zfill(2) + str(now.second).zfill(2)
        return nowString

    def _closeAllFiles(self):
        print '\tCLOSING FILES'
        self.LOPFile.close()
        return

    def _initPSoC(self, comPort,baudRate):
        try:
            #self.PSoC = pys.Serial(comPort,baudrate=baudRate, bytesize=pys.EIGHTBITS, parity=pys.PARITY_NONE, stopbits=pys.STOPBITS_ONE, timeout=1)
            self.PSoC = pys.Serial()
            self.PSoC.close()
            self.PSoC.open()
            self.PSoC.flushInput()
        except:
            print 'FATAL Error opening PSoC on port: ', comPort
            printUsage()
            sys.exit()
    def closeEvent(self):
        print 'CLOSE EVENT'
        self._closeAllFiles()
        print '\tSTOPPING comPortBuffer'
        self.comPortBuffer.COMTHREAD_RUNNING   = False
        print '\tSTOPPING packetParser'
        self.packetParser.PACKETTHREAD_RUNNING = False
        print '\tSTOPPING Manager'   
        self.MANAGER_RUNNING                   = False
        time.sleep(.5)
        self.PSoC.close()

class LOP_Record():
    def __init__(self, PSoC_cpuTimestamp = -1.0, PC_timeStamp = None):
        if PC_timeStamp is None:
            self.PC_timeStamp = datetime.datetime.now()
        else:
            self.PC_timeStamp = PC_timeStamp
        self.PSoC_cpuTimestamp = PSoC_cpuTimestamp
        self.EPOCH = datetime.datetime.utcfromtimestamp(0) #can change this for example to beginning of day etc. to change offset
        self.secSinceEpoch = (self.PC_timeStamp - self.EPOCH).total_seconds()
    def __str__(self):
        printString = 'LOP DETECTED' + '\n'
        printString = printString + 'MCU Timestamp (sec): '       + str(self.PSoC_cpuTimestamp)   + '\n'
        printString = printString + 'PC Timestamp: ' + str(self.PC_timeStamp)        + '\n'
        printString = printString + 'sec since Epoch: '+ str(self.secSinceEpoch)       + '\n'
        return printString

if __name__ == "__main__":
    comPort  = DEFAULT_COMPORT
    baudRate = DEFAULT_BAUDRATE
    # if 1 == len(sys.argv):
    #     printUsage()
    # if len(sys.argv) > 1:
    #     comPort = sys.argv[1]
    #     if (comPort.lower() not in [cp.lower() for cp in serial_ports()] ):
    #         printUsage()       
    # if len(sys.argv) > 2:
    #     baudRate = sys.argv[2]
    print 'Using Baudrate: ' + str(baudRate)
    print 'Using COMPORT:' + comPort
    theManager = LOP_CL_Manager(comPort,baudRate)
    
    try:
        theManager.go()
    except KeyboardInterrupt:
        theManager.closeEvent()
        sys.exit(0)