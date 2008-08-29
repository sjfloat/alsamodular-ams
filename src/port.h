#ifndef PORT_H
#define PORT_H

#include <QColor>
#include <QList>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QString>
#include <QWidget>

#include "synthdata.h"
#include "port_popup.h"

#define PORT_DEFAULT_WIDTH  80
#define PORT_DEFAULT_HEIGHT 15

enum dirType { PORT_IN, PORT_OUT };
enum outTypeEnum { outType_audio, outType_control };

class Port : public QWidget
{
  Q_OBJECT

    QString portName;
    int portNameWidth;
    bool highlighted;
    QList<Port*> connectedPortList;

  public:
    Port(const QString &p_portName, dirType p_dir, int p_index,
            class Module *parent, int p_color = 0);
    ~Port();
    int connectTo(Port *port);
    float **getinputdata (void);

    class Module *module;
    dirType dir;
    int index;
    QColor jackColor, cableColor, &colorFont;
    outTypeEnum outType;
    QList<outTypeEnum> outTypeAcceptList;
    void popupMenuClicked(PopupMenu::portAction);
    bool hasConnectedPort();
    Port* needsConnectionToPort();
    void disconnectClicked();  
    void cableGrayClicked();
    void cableRedClicked();
    void cableGreenClicked();
    void cableBlueClicked();
    void cableYellowClicked();
    void cableDefaultClicked();
    void jackColorClicked();
    void cableColorClicked();
    bool isInPort();
    bool hasIndex(int);
    void removeAllConnectedPorts();
    void removeAllConnectionsTo(Port*);
    void setHighlighted(bool);

  protected:
    virtual void paintEvent(QPaintEvent *ev);
    virtual void mousePressEvent(QMouseEvent* );
    
  signals:
    void portClicked(Port*);
    void portDisconnected();
};
  
#endif
