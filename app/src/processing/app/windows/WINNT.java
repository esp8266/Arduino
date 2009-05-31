/*
 * WINNT.java
 *
 * Created on 8. August 2007, 13:41
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package processing.app.windows;

/**
 *
 * @author TB
 */
public interface WINNT {
  public final static int DELETE       = 0x00010000;
  public final static int READ_CONTROL = 0x00020000;
  public final static int WRITE_DAC    = 0x00040000;
  public final static int WRITE_OWNER  = 0x00080000;
  public final static int SYNCHRONIZE  = 0x00100000;

  public final static int STANDARD_RIGHTS_REQUIRED = 0x000F0000;

  public final static int STANDARD_RIGHTS_READ    = READ_CONTROL;
  public final static int STANDARD_RIGHTS_WRITE   = READ_CONTROL;
  public final static int STANDARD_RIGHTS_EXECUTE = READ_CONTROL;

  public final static int STANDARD_RIGHTS_ALL = 0x001F0000;

  public final static int SPECIFIC_RIGHTS_ALL = 0x0000FFFF;

  public final static int GENERIC_EXECUTE = 0x20000000;
  
  public final static int SERVICE_WIN32_OWN_PROCESS = 0x00000010;
  
  public final static int KEY_QUERY_VALUE        = 0x0001;
  public final static int KEY_SET_VALUE          = 0x0002;
  public final static int KEY_CREATE_SUB_KEY     = 0x0004;
  public final static int KEY_ENUMERATE_SUB_KEYS = 0x0008;
  public final static int KEY_NOTIFY             = 0x0010;
  public final static int KEY_CREATE_LINK        = 0x0020;
  
  public final static int KEY_READ  = ((STANDARD_RIGHTS_READ | KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_NOTIFY) & (~SYNCHRONIZE));
  public final static int KEY_WRITE = ((STANDARD_RIGHTS_WRITE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY) & (~SYNCHRONIZE));
  
  public final static int REG_NONE                       = 0;   // No value type
  public final static int REG_SZ                         = 1;   // Unicode nul terminated string
  public final static int REG_EXPAND_SZ                  = 2;   // Unicode nul terminated string
                                                                // (with environment variable references)
  public final static int REG_BINARY                     = 3;   // Free form binary
  public final static int REG_DWORD                      = 4;   // 32-bit number
  public final static int REG_DWORD_LITTLE_ENDIAN        = 4;   // 32-bit number (same as REG_DWORD)
  public final static int REG_DWORD_BIG_ENDIAN           = 5;   // 32-bit number
  public final static int REG_LINK                       = 6;   // Symbolic Link (unicode)
  public final static int REG_MULTI_SZ                   = 7;   // Multiple Unicode strings
  public final static int REG_RESOURCE_LIST              = 8;   // Resource list in the resource map
  public final static int REG_FULL_RESOURCE_DESCRIPTOR   = 9;  // Resource list in the hardware description
  public final static int REG_RESOURCE_REQUIREMENTS_LIST = 10;
  
  public final static int REG_OPTION_RESERVED       = 0x00000000;   // Parameter is reserved
  public final static int REG_OPTION_NON_VOLATILE   = 0x00000000;   // Key is preserved
                                                                    // when system is rebooted
  public final static int REG_OPTION_VOLATILE       = 0x00000001;   // Key is not preserved
                                                                    // when system is rebooted
  public final static int REG_OPTION_CREATE_LINK    = 0x00000002;   // Created key is a
                                                                    // symbolic link
  public final static int REG_OPTION_BACKUP_RESTORE = 0x00000004;   // open for backup or restore
                                                                    // special access rules
                                                                    // privilege required
  public final static int REG_OPTION_OPEN_LINK      = 0x00000008;   // Open symbolic link
  
}
