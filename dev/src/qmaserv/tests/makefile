CXX      = g++
CXXFLAGS = -g $(INCL)
LD       = $(CXX)
UTILS=/home/phil/code/qmabase/src/qmaserv/utils
MSGS=/home/phil/code/qmabase/src/qmaserv/msgs
DECL=/home/phil/code/qmabase/src/qmaserv/include
QMA=/home/phil/code/qmabase/src/qmaserv/qmaserv
CSERV=/home/phil/code/qmabase/src/qmaserv/cserv
UTIL = /home/phil/code/qmabase/src/util
UNIT=/home/phil/thirdparty/unit++
INCL     = -I$(UNIT)/unit++  -I$(QMA) -I$(UTILS) -I$(MSGS) -I$(DECL) -I$(CSERV)
LDLIBS   = -L$(UNIT)/unit++ -L$(UTIL) -lutil -lnsl -lposix4 -lsocket -lunit++
LDFLAGS  = $(LDLIBS)


files = testClock.C testFile.C testSplit.C testTime.C testSuite.C 


qma = $(QMA)/ClockUtils.o $(QMA)/Continuity.o $(QMA)/StateInfoVO.o $(QMA)/SplitSOD.o $(QMA)/FrameUtils.o $(QMA)/QueueDP.o $(QMA)/CheckPCQS.o $(QMA)/CheckLCQS.o $(QMA)/ProcessDCMult.o $(QMA)/ProcessDCComp.o $(QMA)/ProcessDCD32.o $(QMA)/ProcessIQ.o $(QMA)/DetectorOptionsVO.o $(QMA)/ClockProcVO.o $(QMA)/NetStationVO.o $(QMA)/SecondOfData.o $(QMA)/CreatePacket.o $(QMA)/OptionBitsVO.o $(QMA)/Blockette.o $(QMA)/LogTimingVO.o $(QMA)/ChanFreqMap.o $(QMA)/PCQ.o $(QMA)/LCQ.o $(QMA)/LCQVO.o $(QMA)/ConfigInfoVO.o $(QMA)/TokenBuffer.o $(QMA)/TokenVO.o $(QMA)/PacketMarker.o $(QMA)/SlidingWindow.o $(QMA)/AckCounter.o $(QMA)/TimeServer.o $(QMA)/ReceiveMsgs.o $(QMA)/RxMsg.o $(QMA)/SendCmds.o $(QMA)/CountDownTimer.o $(QMA)/TxCmd.o $(QMA)/ReadConfig.o $(QMA)/Verbose.o $(QMA)/StateMachine.o $(QMA)/ConfigVO.o $(QMA)/QMA_Port.o $(QMA)/Cleanup.o $(QMA)/qmautils.o ./qmatest/qmatest.o

headers = $(QMA)/ClockUtils.o $(QMA)/Continuity.h $(QMA)/StateInfoVO.h $(QMA)/QueueDP.h $(QMA)/PacketQueue.h $(QMA)/CheckPCQS.h $(QMA)/CheckLCQS.h $(QMA)/ProcessDCMult.h $(QMA)/ProcessDCComp.h $(QMA)/ProcessDCD32.h $(QMA)/ProcessIQ.h $(QMA)/DetectorOptionsVO.h $(QMA)/ClockProcVO.h $(QMA)/NetStationVO.h $(QMA)/SecondOfData.h $(QMA)/CreatePacket.h $(QMA)/OptionBits.h $(QMA)/Blockette.h $(QMA)/LogTimingVO.h $(QMA)/PCQ.h $(QMA)/LCQ.h $(QMA)/LCQVO.h $(QMA)/ConfigInfoVO.h $(QMA)/TokenBuffer.h $(QMA)/TokenVO.h $(QMA)/PacketMarker.h $(QMA)/SlidingWindow.h $(QMA)/AckCounter.h $(QMA)/TimeServer.h $(QMA)/BTI.h $(QMA)/ReceiveMsgs.h $(QMA)/RxMsg.h $(QMA)/SendCmds.h $(QMA)/CountDowntimer.h $(QMA)/TxCmd.h $(QMA)/ReadConfig.h $(QMA)/Verbose.h $(QMA)/StateMachine.C $(QMA)/ConfigVO.h $(QMA)/QMA_Port.h $(QMA)/Cleanup.C $(QMA)/qmautils.h $(QMA)/qmaserv.h

cserv = $(CSERV)/buffers.o $(CSERV)/clink.o $(CSERV)/commands.o $(CSERV)/cscfg.o $(CSERV)/cserv.o $(CSERV)/qmacfg.o

utils = $(UTILS)/CharUtils.o $(UTILS)/UnpackComp.o $(UTILS)/CRC.o $(UTILS)/findMD5.o $(UTILS)/md5.o $(UTILS)/ModuloCounter.o $(UTILS)/TimeOfDay.o $(UTILS)/SeqnoList.o

msgs = $(MSGS)/c1_umsg.o $(MSGS)/c1_stat.o $(MSGS)/c1_rqstat.o $(MSGS)/Field.o $(MSGS)/QDPHeader.o $(MSGS)/Packet.o $(MSGS)/PacketElement.o $(MSGS)/c1_mem.o $(MSGS)/c1_rqmem.o $(MSGS)/c1_cack.o $(MSGS)/c1_dsrv.o $(MSGS)/c1_rqsrv.o $(MSGS)/c1_srvch.o $(MSGS)/c1_srvrsp.o $(MSGS)/c1_rqflgs.o $(MSGS)/c1_flgs.o $(MSGS)/c1_cerr.o $(MSGS)/c1_log.o $(MSGS)/c1_sglob.o $(MSGS)/c1_fix.o $(MSGS)/c1_sc.o $(MSGS)/dt_open.o $(MSGS)/dt_data.o $(MSGS)/dt_fill.o $(MSGS)/dt_dack.o

sources = $(headers) $(files)
objects = $(qma) $(cserv) $(utils) $(msgs) $(files:.C=.o)

program =  testSuite

.C.o:
	$(CXX) $(CXXFLAGS) -c $<

ALL =   $(program)

$(program):$(objects) $(files)
	$(LD) $(objects) -o $@ $(LDFLAGS)

all:            $(ALL)

clean:
	-rm *.o

veryclean:	clean
		-rm -f *~ core
