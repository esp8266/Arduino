/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

package antlr;
//package processing.app.preproc;


import antlr.*;
import antlr.collections.impl.BitSet;

/**
 * This class provides TokenStreamHiddenTokenFilters with the concept of
 * tokens which can be copied so that they are seen by both the hidden token
 * stream as well as the parser itself.  This is useful when one wants to use
 * an existing parser (like the Java parser included with ANTLR) that throws
 * away some tokens to create a parse tree which can be used to spit out
 * a copy of the code with only minor modifications.
 *
 * This code is partially derived from the public domain ANLTR TokenStream
 */
public class TokenStreamCopyingHiddenTokenFilter
  extends TokenStreamHiddenTokenFilter
  implements TokenStream {

  protected BitSet copyMask;
  CommonHiddenStreamToken hiddenCopy = null;

  public TokenStreamCopyingHiddenTokenFilter(TokenStream input) {
    super(input);
    copyMask = new BitSet();
  }

  /**
   * Indicate that all tokens of type tokenType should be copied.  The copy
   * is put in the stream of hidden tokens, and the original is returned in the
   * stream of normal tokens.
   *
   * @param tokenType   integer representing the token type to copied
   */
  public void copy(int tokenType) {
    copyMask.add(tokenType);
  }

  /**
   * Create a clone of the important parts of the given token.  Note that this
   * does NOT copy the hiddenBefore and hiddenAfter fields.
   *
   * @param t   token to partially clone
   * @return    newly created partial clone
   */
  public CommonHiddenStreamToken partialCloneToken(CommonHiddenStreamToken t) {

    CommonHiddenStreamToken u = new CommonHiddenStreamToken(t.getType(),
                                                            t.getText());
    u.setColumn(t.getColumn());
    u.setLine(t.getLine());
    u.setFilename(t.getFilename());

    return u;
  }

  public void linkAndCopyToken(CommonHiddenStreamToken prev,
                               CommonHiddenStreamToken monitored) {
    // create a copy of the token in the lookahead for use as hidden token
    hiddenCopy = partialCloneToken(LA(1));

    // attach copy to the previous token, whether hidden or monitored
    prev.setHiddenAfter(hiddenCopy);

    // if previous token was hidden, set the hiddenBefore pointer of the
    // copy to point back to it
    if (prev != monitored) {
      hiddenCopy.setHiddenBefore(prev);
    }

    // we don't want the non-hidden copy to link back to the hidden
    // copy on the next pass through this function, so we leave
    // lastHiddenToken alone

    //System.err.println("hidden copy: " + hiddenCopy.toString());

    return;
  }

  private void consumeFirst() throws TokenStreamException {
    consume(); // get first token of input stream

    // Handle situation where hidden or discarded tokens
    // appear first in input stream
    CommonHiddenStreamToken p=null;

    // while hidden, copied, or discarded scarf tokens
    while ( hideMask.member(LA(1).getType()) ||
            discardMask.member(LA(1).getType()) ||
            copyMask.member(LA(1).getType()) ) {

      // if we've hit one of the tokens that needs to be copied, we copy it
      // and then break out of the loop, because the parser needs to see it
      // too
      //
      if (copyMask.member(LA(1).getType())) {

        // copy the token in the lookahead
        hiddenCopy = partialCloneToken(LA(1));

        // if there's an existing token before this, link that and the
        // copy together
        if (p != null) {
          p.setHiddenAfter(hiddenCopy);
          hiddenCopy.setHiddenBefore(p); // double-link
        }

        lastHiddenToken = hiddenCopy;
        if (firstHidden == null) {
          firstHidden = hiddenCopy;
        }

        // we don't want to consume this token, because it also needs to
        // be passed through to the parser, so break out of the while look
        // entirely
        //
        break;
      } else if (hideMask.member(LA(1).getType())) {
        if (p != null) {
          p.setHiddenAfter(LA(1));
          LA(1).setHiddenBefore(p); // double-link
        }
        p = LA(1);

        lastHiddenToken = p;
        if (firstHidden == null) {
          firstHidden = p; // record hidden token if first
        }
      }
      consume();
    }
  }

  /** Return the next monitored token.
   *  Test the token following the monitored token.
   *  If following is another monitored token, save it
   *  for the next invocation of nextToken (like a single
   *  lookahead token) and return it then.
   *  If following is unmonitored, nondiscarded (hidden)
   *  channel token, add it to the monitored token.
   *
   *  Note: EOF must be a monitored Token.
   */
  public Token nextToken() throws TokenStreamException {
    // handle an initial condition; don't want to get lookahead
    // token of this splitter until first call to nextToken
    if (LA(1) == null) {
      consumeFirst();
    }

    //System.err.println();

    // we always consume hidden tokens after monitored, thus,
    // upon entry LA(1) is a monitored token.
    CommonHiddenStreamToken monitored = LA(1);

    // point to hidden tokens found during last invocation
    monitored.setHiddenBefore(lastHiddenToken);
    lastHiddenToken = null;

    // Look for hidden tokens, hook them into list emanating
    // from the monitored tokens.
    consume();
    CommonHiddenStreamToken prev = monitored;

    // deal with as many not-purely-monitored tokens as possible
    while ( hideMask.member(LA(1).getType()) ||
            discardMask.member(LA(1).getType()) ||
            copyMask.member(LA(1).getType()) ) {

      if (copyMask.member(LA(1).getType())) {

        // copy the token and link it backwards
        if (hiddenCopy != null) {
          linkAndCopyToken(hiddenCopy, monitored);
        } else {
          linkAndCopyToken(prev, monitored);
        }

        // we now need to parse it as a monitored token, so we return, which
        // avoids the consume() call at the end of this loop.  the next call
        // will parse it as a monitored token.
        //System.err.println("returned: " + monitored.toString());
        return monitored;

      } else if (hideMask.member(LA(1).getType())) {

        // attach the hidden token to the monitored in a chain
        // link forwards
        prev.setHiddenAfter(LA(1));

        // link backwards
        if (prev != monitored) { //hidden cannot point to monitored tokens
          LA(1).setHiddenBefore(prev);
        } else if (hiddenCopy != null) {
          hiddenCopy.setHiddenAfter(LA(1));
          LA(1).setHiddenBefore(hiddenCopy);
          hiddenCopy = null;
        }

        //System.err.println("hidden: " + prev.getHiddenAfter().toString() + "\" after: " + prev.toString());
        prev = lastHiddenToken = LA(1);
      }

      consume();
    }

    // remember the last hidden token for next time around
    if (hiddenCopy != null) {
      lastHiddenToken = hiddenCopy;
      hiddenCopy = null;
    }

    //System.err.println("returned: " + monitored.toString());
    return monitored;
  }
}
