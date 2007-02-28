#ifndef __BLOCKINGQUEUE_H__
#define __BLOCKINGQUEUE_H__


const int QUEUE_SIZE = 128;

class QueuedPacket {
 public:
  QueuedPacket(char *, int, short);
  QueuedPacket();
  ~QueuedPacket();

  void update(char *, int, short);
  void clear();

  char data[512];
  int dataSize;
  short packetType;
};

class PacketQueue {
 public:
  PacketQueue();
  ~PacketQueue();
  void enqueuePacket(char *, int, short);
  QueuedPacket dequeuePacket();

 private:
  void advanceHead();
  void advanceTail();
  QueuedPacket queue[QUEUE_SIZE];
  int queueHead;
  int queueTail;
};

#endif
