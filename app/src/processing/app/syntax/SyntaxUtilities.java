/*
 * SyntaxUtilities.java - Utility functions used by syntax colorizing
 * Copyright (C) 1999 Slava Pestov
 *
 * You may use and modify this package for any purpose. Redistribution is
 * permitted, in both source and binary form, provided that this notice
 * remains intact in all source distributions of this package.
 */

package processing.app.syntax;

import javax.swing.text.*;
import java.awt.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


/**
 * Class with several utility functions used by jEdit's syntax colorizing
 * subsystem.
 *
 * @author Slava Pestov
 */
public class SyntaxUtilities
{
  /**
   * Checks if a subregion of a <code>Segment</code> is equal to a
   * string.
   * @param ignoreCase True if case should be ignored, false otherwise
   * @param text The segment
   * @param offset The offset into the segment
   * @param match The string to match
   */
  public static boolean regionMatches(boolean ignoreCase, Segment text,
                                      int offset, String match)
  {
    int length = offset + match.length();
    char[] textArray = text.array;
    if(length > text.offset + text.count)
      return false;
    for(int i = offset, j = 0; i < length; i++, j++)
      {
        char c1 = textArray[i];
        char c2 = match.charAt(j);
        if(ignoreCase)
          {
            c1 = Character.toUpperCase(c1);
            c2 = Character.toUpperCase(c2);
          }
        if(c1 != c2)
          return false;
      }
    return true;
  }


  /**
   * Checks if a subregion of a <code>Segment</code> is equal to a
   * character array.
   * @param ignoreCase True if case should be ignored, false otherwise
   * @param text The segment
   * @param offset The offset into the segment
   * @param match The character array to match
   */
  public static boolean regionMatches(boolean ignoreCase, Segment text,
                                      int offset, char[] match)
  {
    int length = offset + match.length;
    char[] textArray = text.array;
    if(length > text.offset + text.count)
      return false;
    for(int i = offset, j = 0; i < length; i++, j++)
      {
        char c1 = textArray[i];
        char c2 = match[j];
        if(ignoreCase)
          {
            c1 = Character.toUpperCase(c1);
            c2 = Character.toUpperCase(c2);
          }
        if(c1 != c2)
          return false;
      }
    return true;
  }


  /**
   * Returns the default style table. This can be passed to the
   * <code>setStyles()</code> method of <code>SyntaxDocument</code>
   * to use the default syntax styles.
   */
  public static SyntaxStyle[] getDefaultSyntaxStyles()
  {
    SyntaxStyle[] styles = new SyntaxStyle[Token.ID_COUNT];

    styles[Token.COMMENT1] = new SyntaxStyle(Color.black,true,false,false);
    styles[Token.COMMENT2] = new SyntaxStyle(new Color(0x990033),true,false,false);
    styles[Token.KEYWORD1] = new SyntaxStyle(Color.black,false,true,false);
    styles[Token.KEYWORD2] = new SyntaxStyle(Color.magenta,false,false,false);
    styles[Token.KEYWORD3] = new SyntaxStyle(new Color(0x009600),false,false,false);
    styles[Token.LITERAL1] = new SyntaxStyle(new Color(0x650099),false,false,false);
    styles[Token.LITERAL2] = new SyntaxStyle(new Color(0x650099),false,true,false);
    styles[Token.LABEL] = new SyntaxStyle(new Color(0x990033),false,true,false);
    styles[Token.OPERATOR] = new SyntaxStyle(Color.black,false,true,false);
    styles[Token.URL] = new SyntaxStyle(Color.blue,true,false,false);
    styles[Token.INVALID] = new SyntaxStyle(Color.red,false,true,false);

    return styles;
  }


  /**
   * Paints the specified line onto the graphics context. Note that this
   * method munges the offset and count values of the segment.
   * @param line The line segment
   * @param tokens The token list for the line
   * @param styles The syntax style list
   * @param expander The tab expander used to determine tab stops. May
   * be null
   * @param gfx The graphics context
   * @param x The x co-ordinate
   * @param y The y co-ordinate
   * @return The x co-ordinate, plus the width of the painted string
   */
  public static int paintSyntaxLine(Segment line, Token tokens,
                                    SyntaxStyle[] styles,
                                    TabExpander expander, Graphics gfx,
                                    int x, int y)
  {
    Font defaultFont = gfx.getFont();
    Color defaultColor = gfx.getColor();

    for(;;)
      {
        byte id = tokens.id;
        if(id == Token.END)
          break;

        int length = tokens.length;
        if(id == Token.NULL)
          {
            if(!defaultColor.equals(gfx.getColor()))
              gfx.setColor(defaultColor);
            if(!defaultFont.equals(gfx.getFont()))
              gfx.setFont(defaultFont);
          }
        else
          styles[id].setGraphicsFlags(gfx,defaultFont);

        line.count = length;
        if (id == Token.COMMENT1 || id == Token.COMMENT2)
          x = drawTabbedCommentsText(line, x, y, gfx, expander, styles, styles[id]);
        else
          x = Utilities.drawTabbedText(line, x, y, gfx, expander, 0);        
        line.offset += length;

        tokens = tokens.next;
      }

    return x;
  }

  /**
   * Parse comments and identify "@schematics <b>&lt;something&gt;</b>" pattern.
   * 
   * @param line
   *          A string to parse
   * @return <b>null</b> if the pattern is not found, otherwise an array of
   *         String is returned: the elements with index 0, 1 and 2 are
   *         respectively the preamble, the <b>&lt;something&gt;</b> stuff, and
   *         the remaining part of the string.
   */
  public static String[] parseCommentUrls(String line) {
    Matcher m = urlPattern.matcher(line.toString());
    if (!m.find())
      return null;

    String res[] = new String[3];
    res[0] = line.substring(0, m.start(1));
    res[1] = line.substring(m.start(1), m.end(1));
    res[2] = line.substring(m.end(1));
    // System.out.println("0 =>"+res[0]+"<\n1 =>"+res[1]+"< \n2 =>"+res[2]+"<");
    return res;
  }

  static private Pattern urlPattern = Pattern.compile(
      "((?:https?|ftp)://" +                // ( Protocol
      "(?:(?:[\\w_\\-]+:)?[\\w_\\-]+@)?" +  // Username and password
      "(?:[\\w_\\-]+\\.)+[\\w_\\-]+" +      // Domain name
      "(?::[0-9]{1,5})?" +                  // Port 
      "(?:/[\\w_\\-./?%&=+]*)?)" +          // Path ) 
      "(?:\\s|$)");                         // whitespace or EOL

  public static Segment stringToSegment(String v) {
    return new Segment(v.toCharArray(), 0, v.length());
  }

  private static int drawTabbedCommentsText(Segment line, int x, int y,
      Graphics gfx, TabExpander expander, SyntaxStyle[] styles, 
      SyntaxStyle commentStyle) {

    String parse[] = parseCommentUrls(line.toString());
    if (parse == null)
      // Revert to plain writing.
      return Utilities.drawTabbedText(line, x, y, gfx, expander, 0);
    Segment pre = stringToSegment(parse[0]);
    Segment tag = stringToSegment(parse[1]);
    Segment post = stringToSegment(parse[2]);

    if (pre.count>0)
      x = Utilities.drawTabbedText(pre, x, y, gfx, expander, 0);

    Font f = gfx.getFont();
    styles[Token.URL].setGraphicsFlags(gfx, f);
    x = Utilities.drawTabbedText(tag, x, y, gfx, expander, 0);

    commentStyle.setGraphicsFlags(gfx, f);
    if (post.count>0)
      x = Utilities.drawTabbedText(post, x, y, gfx, expander, 0);
    return x;
  }

  // private members
  private SyntaxUtilities() {}
}
