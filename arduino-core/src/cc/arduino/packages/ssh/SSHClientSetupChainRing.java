package cc.arduino.packages.ssh;

import cc.arduino.packages.BoardPort;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;

import java.io.IOException;

public interface SSHClientSetupChainRing {

  /*
  Chain is actually useless as default JSCH behaviour is to follow SSH Server authentication methods list
   */
  Session setup(BoardPort port, JSch jSch) throws JSchException, IOException;

}
