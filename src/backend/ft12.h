/*
    EIBD eib bus access and management daemon
    Copyright (C) 2005-2011 Martin Koegler <mkoegler@auto.tuwien.ac.at>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef EIB_FT12_H
#define EIB_FT12_H

#include <termios.h>

#include "iobuf.h"
#include "lowlevel.h"
#include "emi_common.h"
#include "lowlatency.h"
#include "link.h"

/** FT1.2 lowlevel driver*/
DRIVER_(FT12Driver,LowLevelAdapter,ft12)
{
public:
  FT12Driver(const LinkConnectPtr_& c, IniSection& s) : LowLevelAdapter(c,s) {}
  virtual ~FT12Driver();

  bool setup();
  virtual EMIVer getVersion() { return vEMI2; }
private:
  bool make_EMI();
};

DRIVER_(FT12cemiDriver, FT12Driver, ft12cemi)
{
public:
  FT12cemiDriver(const LinkConnectPtr_& c, IniSection& s) : FT12Driver(c,s) {}
  virtual ~FT12cemiDriver();

  virtual EMIVer getVersion() { return vCEMI; }
};

class FT12serial : public LowLevelDriver
{
  /** old serial config */
  low_latency_save sold;
  /** device file name */
  std::string dev;
  /** file descriptor */
  int fd;

  RecvBuf recvbuf;
  SendBuf sendbuf;
  size_t read_cb(uint8_t *buf, size_t len);
  void error_cb();
  /** saved termios */
  struct termios old;
  /** send state */
  int sendflag;
  /** recevie state */
  int recvflag;
  /** packet send buffer */
  CArray out;
  /** frame in receiving */
  CArray akt;
  /** last received frame */
  CArray last;
  /** repeatcount of the transmitting frame */
  int repeatcount;
  /** state */
  bool send_wait;

  /** set up send and recv buffers, timers, etc. */
  void setup_buffers();

  ev::async trigger; void trigger_cb (ev::async &w, int revents);
  ev::timer timer; void timer_cb (ev::timer &w, int revents);
  ev::timer sendtimer; void sendtimer_cb (ev::timer &w, int revents);
  /** process incoming data */
  void process_read(bool is_timeout);

public:
  FT12serial (LowLevelIface* parent, IniSection &s);
  virtual ~FT12serial ();
  bool setup ();
  void start();
  void stop();

  void send_Data (CArray& l);
  void SendReset ();
};

#endif
