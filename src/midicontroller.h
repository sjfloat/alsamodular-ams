#ifndef MIDICONTROLLER_H
#define MIDICONTROLLER_H

#include <qobject.h>
#include <qlist.h>
#include <alsa/asoundlib.h>

class MidiControllerContext  {
public:
  QList <class MidiControllableBase *> mcAbles;
          
public:
  MidiControllerContext() {}

  void setMidiValueRT(unsigned int value);
};


class MidiControllerKey {

public:
  static unsigned int mkKey(unsigned char type, unsigned char ch,
          unsigned int param) {
    unsigned int key = ((unsigned int)type << 28) |
        (((unsigned int)ch & 0xF) << 24) | ((param & 0xFF) << 16) | 0xFFFF;
    return key;
  }

protected:
  unsigned int key;

public:
  bool isValid() {
    return !(*this == MidiControllerKey());
  }
  unsigned int getKey() const {
    return key;
  }
  unsigned char type() const {
    return (key >> 28) & 0xF;
  }
  unsigned char ch() const {
    return (key & 0x0F000000) >> 24;
  }
  unsigned int param() const {
    return (key & 0x00FF0000) >> 16;
  }

  MidiControllerKey()
    : key(mkKey(0, 0, 0))
  {}
  MidiControllerKey(unsigned char type, unsigned char ch, unsigned int param)
    : key(mkKey(type, ch, param))
  {}
  MidiControllerKey(snd_seq_event_t *ev)
    : key(mkKey(ev->type, ev->data.control.channel,
		ev->type == SND_SEQ_EVENT_PITCHBEND ?
		0 : (ev->type == SND_SEQ_EVENT_NOTEON ||
                    ev->type == SND_SEQ_EVENT_NOTEOFF) ?
		ev->data.note.note : ev->data.control.param))
  {}
  MidiControllerKey(const MidiControllerKey &other)
    : key(other.key)
  {} 
  MidiControllerKey(MidiControllerKey &other)
    : key(other.key)
  {} 
  MidiControllerKey(unsigned int key)
    : key(key | 0xFFFF)
  {} 

  bool operator == (const MidiControllerKey &other ) const {
    return key == other.key;
  }
  bool operator < (const MidiControllerKey &other ) const {
    return key < other.key;
  }
};
                              
class MidiController: public MidiControllerKey {

public:
  MidiControllerContext *context;

public:
  MidiController()
    : context(NULL)
  {}
  MidiController(unsigned char type, unsigned char ch, unsigned int param)
    : MidiControllerKey(type, ch, param)
    , context(NULL)
  {}
  MidiController(snd_seq_event_t *ev)
    : MidiControllerKey(ev)
    , context(NULL)
  {}
  MidiController(const MidiController &other)
    : MidiControllerKey(other)
    , context(other.context)
  {} 
  MidiController(MidiController &other)
    : MidiControllerKey(other)
    , context(other.context)
  {
    //    std::cout << __PRETTY_FUNCTION__ << " " << other.context << " " << context << std::endl;
    other.context = NULL;
  } 
  MidiController(unsigned int key)
    : MidiControllerKey(key)
    , context(NULL)
  {} 

  ~MidiController() {
    delete context;
  }
};

#endif
