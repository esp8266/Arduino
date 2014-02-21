/*
 * TextUtilities.java - Utility functions used by the text area classes
 * Copyright (C) 1999 Slava Pestov
 *
 * You may use and modify this package for any purpose. Redistribution is
 * permitted, in both source and binary form, provided that this notice
 * remains intact in all source distributions of this package.
 */

package processing.app.syntax;

import javax.swing.text.*;

/**
 * Class with several utility functions used by the text area component.
 * @author Slava Pestov
 */
public class TextUtilities
{
        /**
         * Returns the offset of the bracket matching the one at the
         * specified offset of the document, or -1 if the bracket is
         * unmatched (or if the character is not a bracket).
         * @param doc The document
         * @param offset The offset
         * @exception BadLocationException If an out-of-bounds access
         * was attempted on the document text
         */
        public static int findMatchingBracket(Document doc, int offset)
                throws BadLocationException
        {
                if(doc.getLength() == 0)
                        return -1;
                char c = doc.getText(offset,1).charAt(0);
                char cprime; // c` - corresponding character
                boolean direction; // true = back, false = forward

                switch(c)
                {
                case '(': cprime = ')'; direction = false; break;
                case ')': cprime = '('; direction = true; break;
                case '[': cprime = ']'; direction = false; break;
                case ']': cprime = '['; direction = true; break;
                case '{': cprime = '}'; direction = false; break;
                case '}': cprime = '{'; direction = true; break;
                default: return -1;
                }

                int count;

                // How to merge these two cases is left as an exercise
                // for the reader.

                // Go back or forward
                if(direction)
                {
                        // Count is 1 initially because we have already
                        // `found' one closing bracket
                        count = 1;

                        // Get text[0,offset-1];
                        String text = doc.getText(0,offset);

                        // Scan backwards
                        for(int i = offset - 1; i >= 0; i--)
                        {
                                // If text[i] == c, we have found another
                                // closing bracket, therefore we will need
                                // two opening brackets to complete the
                                // match.
                                char x = text.charAt(i);
                                if(x == c)
                                        count++;

                                // If text[i] == cprime, we have found a
                                // opening bracket, so we return i if
                                // --count == 0
                                else if(x == cprime)
                                {
                                        if(--count == 0)
                                                return i;
                                }
                        }
                }
                else
                {
                        // Count is 1 initially because we have already
                        // `found' one opening bracket
                        count = 1;

                        // So we don't have to + 1 in every loop
                        offset++;

                        // Number of characters to check
                        int len = doc.getLength() - offset;

                        // Get text[offset+1,len];
                        String text = doc.getText(offset,len);

                        // Scan forwards
                        for(int i = 0; i < len; i++)
                        {
                                // If text[i] == c, we have found another
                                // opening bracket, therefore we will need
                                // two closing brackets to complete the
                                // match.
                                char x = text.charAt(i);

                                if(x == c)
                                        count++;

                                // If text[i] == cprime, we have found an
                                // closing bracket, so we return i if
                                // --count == 0
                                else if(x == cprime)
                                {
                                        if(--count == 0)
                                                return i + offset;
                                }
                        }
                }

                // Nothing found
                return -1;
        }

        /**
         * Locates the start of the word at the specified position.
         * @param line The text
         * @param pos The position
         */
        public static int findWordStart(String line, int pos, String noWordSep)
        {
                char ch = line.charAt(pos - 1);

                if(noWordSep == null)
                        noWordSep = "";
                boolean selectNoLetter = (!Character.isLetterOrDigit(ch)
                        && noWordSep.indexOf(ch) == -1);

                int wordStart = 0;
                for(int i = pos - 1; i >= 0; i--)
                {
                        ch = line.charAt(i);
                        if(selectNoLetter ^ (!Character.isLetterOrDigit(ch) &&
                                noWordSep.indexOf(ch) == -1))
                        {
                                wordStart = i + 1;
                                break;
                        }
                }

                return wordStart;
        }

        /**
         * Locates the end of the word at the specified position.
         * @param line The text
         * @param pos The position
         */
        public static int findWordEnd(String line, int pos, String noWordSep)
        {
                char ch = line.charAt(pos);

                if(noWordSep == null)
                        noWordSep = "";
                boolean selectNoLetter = (!Character.isLetterOrDigit(ch)
                        && noWordSep.indexOf(ch) == -1);

                int wordEnd = line.length();
                for(int i = pos; i < line.length(); i++)
                {
                        ch = line.charAt(i);
                        if(selectNoLetter ^ (!Character.isLetterOrDigit(ch) &&
                                noWordSep.indexOf(ch) == -1))
                        {
                                wordEnd = i;
                                break;
                        }
                }
                return wordEnd;
        }
}
