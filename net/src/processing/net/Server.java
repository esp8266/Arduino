/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Server - basic network server implementation
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-2007 Ben Fry and Casey Reas
  The previous version of this code was developed by Hernando Barragan

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

package processing.net;
import processing.core.*;

import java.io.*;
import java.lang.reflect.*;
import java.net.*;


public class Server implements Runnable {

  PApplet parent;
  Method serverEventMethod;

  Thread thread;
  ServerSocket server;
  int port;
  
  /** Number of clients currently connected. */
  public int clientCount;
  /** Array of client objects, useful length is determined by clientCount. */
  public Client[] clients;


  public Server(PApplet parent, int port) {
    this.parent = parent;
    this.port = port;

    try {
      server = new ServerSocket(this.port);
      //clients = new Vector();
      clients = new Client[10];

      thread = new Thread(this);
      thread.start();

      parent.registerDispose(this);

      // reflection to check whether host applet has a call for
      // public void serverEvent(Server s, Client c);
      // which is called when a new guy connects
      try {
        serverEventMethod =
          parent.getClass().getMethod("serverEvent",
                                      new Class[] { Server.class,
                                                    Client.class });
      } catch (Exception e) {
        // no such method, or an error.. which is fine, just ignore
      }

    } catch (IOException e) {
      e.printStackTrace();
      thread = null;
      //errorMessage("<init>", e);
    }
  }


  /**
   * Disconnect a particular client.
   */
  public void disconnect(Client client) {
    //client.stop();
    client.dispose();
    int index = clientIndex(client);
    if (index != -1) {
      removeIndex(index);
    }
  }
  
  
  protected void removeIndex(int index) {
    clientCount--;
    // shift down the remaining clients
    for (int i = index; i < clientCount; i++) {
      clients[i] = clients[i+1];
    }
    // mark last empty var for garbage collection
    clients[clientCount] = null;
  }
  
  
  protected void addClient(Client client) {
    if (clientCount == clients.length) {
      clients = (Client[]) PApplet.expand(clients);
    }
    clients[clientCount++] = client;
  }
  
  
  protected int clientIndex(Client client) {
    for (int i = 0; i < clientCount; i++) {
      if (clients[i] == client) {
        return i;
      }
    }
    return -1;
  }


  // the last index used for available. can't just cycle through
  // the clients in order from 0 each time, because if client 0 won't
  // shut up, then the rest of the clients will never be heard from.
  int lastAvailable = -1;

  /**
   * Returns the next client in line that has something to say.
   */
  public Client available() {
    synchronized (clients) {
      int index = lastAvailable + 1;
      if (index >= clientCount) index = 0;

      for (int i = 0; i < clientCount; i++) {
        int which = (index + i) % clientCount;
        Client client = clients[which];
        if (client.available() > 0) {
          lastAvailable = which;
          return client;
        }
      }
    }
    return null;
  }


  /**
   * Disconnect all clients and stop the server.
   * <p/>
   * Use this to shut down the server if you finish using it while your applet 
   * is still running. Otherwise, it will be automatically be shut down by the 
   * host PApplet using dispose(), which is identical. 
   */
  public void stop() {
    dispose();
  }


  /**
   * Disconnect all clients and stop the server: internal use only.
   */
  public void dispose() {
    try {
      thread = null;

      if (clients != null) {
        for (int i = 0; i < clientCount; i++) {
          disconnect(clients[i]);
        }
        clientCount = 0;
        clients = null;
      }

      if (server != null) {
        server.close();
        server = null;
      }

    } catch (IOException e) {
      e.printStackTrace();
      //errorMessage("stop", e);
    }
  }


  public void run() {
    while (Thread.currentThread() == thread) {
      try {
        Socket socket = server.accept();
        Client client = new Client(parent, socket);
        synchronized (clients) {
          addClient(client);
          if (serverEventMethod != null) {
            try {
              serverEventMethod.invoke(parent, new Object[] { this, client });
            } catch (Exception e) {
              System.err.println("Disabling serverEvent() for port " + port);
              e.printStackTrace();
              serverEventMethod = null;
            }
          }
        }
      } catch (IOException e) {
        //errorMessage("run", e);
        e.printStackTrace();
        thread = null;
      }
      try {
        Thread.sleep(8);
      } catch (InterruptedException ex) { }
    }
  }


  /**
   * Write a value to all the connected clients.
   * See Client.write() for operational details.
   */
  public void write(int what) {  // will also cover char
    int index = 0;
    while (index < clientCount) {
      clients[index].write(what);
      if (clients[index].active()) {
        index++;
      } else {
        removeIndex(index);
      }
    }
  }


  /**
   * Write a byte array to all the connected clients.
   * See Client.write() for operational details.
   */
  public void write(byte what[]) {
    int index = 0;
    while (index < clientCount) {
      clients[index].write(what);
      if (clients[index].active()) {
        index++;
      } else {
        removeIndex(index);
      }
    }
  }


  /**
   * Write a String to all the connected clients.
   * See Client.write() for operational details.
   */
  public void write(String what) {
    int index = 0;
    while (index < clientCount) {
      clients[index].write(what);
      if (clients[index].active()) {
        index++;
      } else {
        removeIndex(index);
      }
    }
  }


  /**
   * General error reporting, all corraled here just in case
   * I think of something slightly more intelligent to do.
   */
//  public void errorMessage(String where, Exception e) {
//    parent.die("Error inside Server." + where + "()", e);
//    //System.err.println("Error inside Server." + where + "()");
//    //e.printStackTrace(System.err);
//  }
}
