/*
 * Options.java
 *
 * Created on 8. August 2007, 17:07
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package processing.app.windows;

import static com.sun.jna.Library.*;
import com.sun.jna.win32.*;
import java.util.*;

/**
 *
 * @author TB
 */
public interface Options {
  Map<String, Object> UNICODE_OPTIONS = new HashMap<String, Object>() {
    {
      put(OPTION_TYPE_MAPPER, W32APITypeMapper.UNICODE);
      put(OPTION_FUNCTION_MAPPER, W32APIFunctionMapper.UNICODE);
    }
  };
}
