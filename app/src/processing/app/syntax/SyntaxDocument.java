/*
 * SyntaxDocument.java - Document that can be tokenized
 * Copyright (C) 1999 Slava Pestov
 *
 * You may use and modify this package for any purpose. Redistribution is
 * permitted, in both source and binary form, provided that this notice
 * remains intact in all source distributions of this package.
 */

package processing.app.syntax;

import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.undo.UndoableEdit;

/**
 * A document implementation that can be tokenized by the syntax highlighting
 * system.
 *
 * @author Slava Pestov
 */
public class SyntaxDocument extends PlainDocument
{
        /**
         * Returns the token marker that is to be used to split lines
         * of this document up into tokens. May return null if this
         * document is not to be colorized.
         */
        public TokenMarker getTokenMarker()
        {
                return tokenMarker;
        }

        /**
         * Sets the token marker that is to be used to split lines of
         * this document up into tokens. May throw an exception if
         * this is not supported for this type of document.
         * @param tm The new token marker
         */
        public void setTokenMarker(TokenMarker tm)
        {
                tokenMarker = tm;
                if(tm == null)
                        return;
                tokenMarker.insertLines(0,getDefaultRootElement()
                        .getElementCount());
                tokenizeLines();
        }

        /**
         * Reparses the document, by passing all lines to the token
         * marker. This should be called after the document is first
         * loaded.
         */
        public void tokenizeLines()
        {
                tokenizeLines(0,getDefaultRootElement().getElementCount());
        }

        /**
         * Reparses the document, by passing the specified lines to the
         * token marker. This should be called after a large quantity of
         * text is first inserted.
         * @param start The first line to parse
         * @param len The number of lines, after the first one to parse
         */
        public void tokenizeLines(int start, int len)
        {
                if(tokenMarker == null || !tokenMarker.supportsMultilineTokens())
                        return;

                Segment lineSegment = new Segment();
                Element map = getDefaultRootElement();

                len += start;

                try
                {
                        for(int i = start; i < len; i++)
                        {
                                Element lineElement = map.getElement(i);
                                int lineStart = lineElement.getStartOffset();
                                getText(lineStart,lineElement.getEndOffset()
                                        - lineStart - 1,lineSegment);
                                tokenMarker.markTokens(lineSegment,i);
                        }
                }
                catch(BadLocationException bl)
                {
                        bl.printStackTrace();
                }
        }

        /**
         * Starts a compound edit that can be undone in one operation.
         * Subclasses that implement undo should override this method;
         * this class has no undo functionality so this method is
         * empty.
         */
        public void beginCompoundEdit() {}

        /**
         * Ends a compound edit that can be undone in one operation.
         * Subclasses that implement undo should override this method;
         * this class has no undo functionality so this method is
         * empty.
         */
        public void endCompoundEdit() {}

        /**
         * Adds an undoable edit to this document's undo list. The edit
         * should be ignored if something is currently being undone.
         * @param edit The undoable edit
         *
         * @since jEdit 2.2pre1
         */
        public void addUndoableEdit(UndoableEdit edit) {}

        // protected members
        protected TokenMarker tokenMarker;

        /**
         * We overwrite this method to update the token marker
         * state immediately so that any event listeners get a
         * consistent token marker.
         */
        protected void fireInsertUpdate(DocumentEvent evt)
        {
                if(tokenMarker != null)
                {
                        DocumentEvent.ElementChange ch = evt.getChange(
                                getDefaultRootElement());
                        if(ch != null)
                        {
                                tokenMarker.insertLines(ch.getIndex() + 1,
                                        ch.getChildrenAdded().length -
                                        ch.getChildrenRemoved().length);
                        }
                }

                super.fireInsertUpdate(evt);
        }

        /**
         * We overwrite this method to update the token marker
         * state immediately so that any event listeners get a
         * consistent token marker.
         */
        protected void fireRemoveUpdate(DocumentEvent evt)
        {
                if(tokenMarker != null)
                {
                        DocumentEvent.ElementChange ch = evt.getChange(
                                getDefaultRootElement());
                        if(ch != null)
                        {
                                tokenMarker.deleteLines(ch.getIndex() + 1,
                                        ch.getChildrenRemoved().length -
                                        ch.getChildrenAdded().length);
                        }
                }

                super.fireRemoveUpdate(evt);
        }
}
