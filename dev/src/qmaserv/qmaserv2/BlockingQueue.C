
#include "global.h"
#include "BlockingQueue.h"


QueuedPacket::QueuedPacket(char *packetData, int packetSize, short packetType) {
  this->update(packetData, packetSize, packetType);
}

QueuedPacket::QueuedPacket() {
  this->clear();
}

QueuedPacket::~QueuedPacket() {
  return;
}

void QueuedPacket::update(char *packetData, int packetSize, short packetType) {
  this->dataSize = packetSize;
  this->packetType = packetType;
  memcpy(this->data, packetData, packetSize);
}

void QueuedPacket::clear() {
  this->dataSize = 0;
  memcpy(this->data, "\0", 1);
  this->packetType = 0;
}

/************************************************************/
PacketQueue::PacketQueue() {
  queueHead = 0;
  queueTail = 0;
}

PacketQueue::~PacketQueue() {
  return;
}

void PacketQueue::enqueuePacket(char *data, int dataSize, short packetType) {
  this->queue[this->queueTail].update(data, dataSize, packetType);
  this->advanceTail();
}


QueuedPacket PacketQueue::dequeuePacket() {
  QueuedPacket ret = this->queue[this->queueHead];
  QueuedPacket *ptr = &this->queue[this->queueHead];
  ptr->clear();
  if(ret.dataSize) {
    // don't advance the head if this was empty to begin with
    this->advanceHead();
  }  
  return ret;
}


void PacketQueue::advanceTail() {

  this->queueTail++;
  if(this->queueTail == QUEUE_SIZE) {
    this->queueTail = 0;
  }

  if(this->queue[this->queueTail].dataSize != 0) {
    std::cout << "XXX Blocking queue lapped" << std::endl;
    // reset the head to where teh tail is now, since this
    // is the new head
    this->queueHead = this->queueTail;
  }
}

void PacketQueue::advanceHead() {
  this->queueHead++;
  if(this->queueHead == QUEUE_SIZE) {
    this->queueHead = 0;
  }
}
  
/**
 * Test code
 
int main(int argc, char *argv[]) {
  PacketQueue pq = PacketQueue();

  for(int i = 0; i < 129; i++) {
    pq.enqueuePacket("packet1", strlen("packet1")+1, i);
  }
  
  QueuedPacket thisPacket = pq.dequeuePacket();

  while(thisPacket.dataSize != 0) {
    printf("Dequeued: %d\n", thisPacket.packetType);
    thisPacket = pq.dequeuePacket();
  }
}
*/
