/*
 * WINBASE.java
 *
 * Created on 5. September 2007, 11:24
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package processing.app.windows;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;

/**
 *
 * @author TB
 */
public interface WINBASE {
/*
typedef struct _SECURITY_ATTRIBUTES {
  DWORD nLength;
  LPVOID lpSecurityDescriptor;
  BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, 
 *PSECURITY_ATTRIBUTES, 
 *LPSECURITY_ATTRIBUTES;*/
  public static class SECURITY_ATTRIBUTES extends Structure {
    public int nLength;
    public Pointer lpSecurityDescriptor;
    public boolean bInheritHandle;
  }
  
/*
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;*/
  public static class FILETIME extends Structure {
    public int dwLowDateTime;
    public int dwHighDateTime;
  }
}
