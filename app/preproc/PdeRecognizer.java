// $ANTLR 2.7.2: "expandedpde.g" -> "PdeRecognizer.java"$

package processing.app.preproc;

import processing.app.*;

import antlr.TokenBuffer;
import antlr.TokenStreamException;
import antlr.TokenStreamIOException;
import antlr.ANTLRException;
import antlr.LLkParser;
import antlr.Token;
import antlr.TokenStream;
import antlr.RecognitionException;
import antlr.NoViableAltException;
import antlr.MismatchedTokenException;
import antlr.SemanticException;
import antlr.ParserSharedInputState;
import antlr.collections.impl.BitSet;
import antlr.collections.AST;
import java.util.Hashtable;
import antlr.ASTFactory;
import antlr.ASTPair;
import antlr.collections.impl.ASTArray;

public class PdeRecognizer extends antlr.LLkParser       implements PdePartialTokenTypes
 {

protected PdeRecognizer(TokenBuffer tokenBuf, int k) {
  super(tokenBuf,k);
  tokenNames = _tokenNames;
  buildTokenTypeASTClassMap();
  astFactory = new ASTFactory(getTokenTypeToASTClassMap());
}

public PdeRecognizer(TokenBuffer tokenBuf) {
  this(tokenBuf,2);
}

protected PdeRecognizer(TokenStream lexer, int k) {
  super(lexer,k);
  tokenNames = _tokenNames;
  buildTokenTypeASTClassMap();
  astFactory = new ASTFactory(getTokenTypeToASTClassMap());
}

public PdeRecognizer(TokenStream lexer) {
  this(lexer,2);
}

public PdeRecognizer(ParserSharedInputState state) {
  super(state,2);
  tokenNames = _tokenNames;
  buildTokenTypeASTClassMap();
  astFactory = new ASTFactory(getTokenTypeToASTClassMap());
}

	public final void pdeProgram() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST pdeProgram_AST = null;
		
		boolean synPredMatched3 = false;
		if (((_tokenSet_0.member(LA(1))) && (_tokenSet_1.member(LA(2))))) {
			int _m3 = mark();
			synPredMatched3 = true;
			inputState.guessing++;
			try {
				{
				if ((LA(1)==LITERAL_public)) {
					match(LITERAL_public);
					match(LITERAL_class);
				}
				else if ((LA(1)==LITERAL_import)) {
					match(LITERAL_import);
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
			}
			catch (RecognitionException pe) {
				synPredMatched3 = false;
			}
			rewind(_m3);
			inputState.guessing--;
		}
		if ( synPredMatched3 ) {
			javaProgram();
			astFactory.addASTChild(currentAST, returnAST);
			if ( inputState.guessing==0 ) {
				PdePreprocessor.programType = PdePreprocessor.JAVA;
			}
			pdeProgram_AST = (AST)currentAST.root;
		}
		else {
			boolean synPredMatched7 = false;
			if (((_tokenSet_2.member(LA(1))) && (_tokenSet_3.member(LA(2))))) {
				int _m7 = mark();
				synPredMatched7 = true;
				inputState.guessing++;
				try {
					{
					{
					_loop6:
					do {
						// nongreedy exit test
						if ((LA(1)==LITERAL_void) && (LA(2)==IDENT)) break _loop6;
						if ((_tokenSet_2.member(LA(1))) && (_tokenSet_4.member(LA(2)))) {
							possiblyEmptyField();
						}
						else {
							break _loop6;
						}
						
					} while (true);
					}
					match(LITERAL_void);
					match(IDENT);
					match(LPAREN);
					}
				}
				catch (RecognitionException pe) {
					synPredMatched7 = false;
				}
				rewind(_m7);
				inputState.guessing--;
			}
			if ( synPredMatched7 ) {
				activeProgram();
				astFactory.addASTChild(currentAST, returnAST);
				if ( inputState.guessing==0 ) {
					PdePreprocessor.programType = PdePreprocessor.ACTIVE;
				}
				pdeProgram_AST = (AST)currentAST.root;
			}
			else if ((_tokenSet_5.member(LA(1))) && (_tokenSet_6.member(LA(2)))) {
				staticProgram();
				astFactory.addASTChild(currentAST, returnAST);
				if ( inputState.guessing==0 ) {
					PdePreprocessor.programType = PdePreprocessor.STATIC;
				}
				pdeProgram_AST = (AST)currentAST.root;
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			returnAST = pdeProgram_AST;
		}
		
	public final void javaProgram() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST javaProgram_AST = null;
		
		compilationUnit();
		astFactory.addASTChild(currentAST, returnAST);
		javaProgram_AST = (AST)currentAST.root;
		returnAST = javaProgram_AST;
	}
	
	public final void possiblyEmptyField() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST possiblyEmptyField_AST = null;
		Token  s = null;
		AST s_AST = null;
		
		if ((_tokenSet_7.member(LA(1)))) {
			field();
			astFactory.addASTChild(currentAST, returnAST);
			possiblyEmptyField_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==SEMI)) {
			s = LT(1);
			s_AST = astFactory.create(s);
			astFactory.addASTChild(currentAST, s_AST);
			match(SEMI);
			if ( inputState.guessing==0 ) {
				s_AST.setType(EMPTY_FIELD);
			}
			possiblyEmptyField_AST = (AST)currentAST.root;
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = possiblyEmptyField_AST;
	}
	
	public final void activeProgram() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST activeProgram_AST = null;
		
		{
		int _cnt11=0;
		_loop11:
		do {
			if ((_tokenSet_2.member(LA(1)))) {
				possiblyEmptyField();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				if ( _cnt11>=1 ) { break _loop11; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt11++;
		} while (true);
		}
		activeProgram_AST = (AST)currentAST.root;
		returnAST = activeProgram_AST;
	}
	
	public final void staticProgram() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST staticProgram_AST = null;
		
		{
		_loop14:
		do {
			if ((_tokenSet_8.member(LA(1)))) {
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop14;
			}
			
		} while (true);
		}
		staticProgram_AST = (AST)currentAST.root;
		returnAST = staticProgram_AST;
	}
	
	public final void compilationUnit() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST compilationUnit_AST = null;
		
		{
		if ((LA(1)==LITERAL_package)) {
			packageDefinition();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_9.member(LA(1)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		{
		_loop42:
		do {
			if ((LA(1)==LITERAL_import)) {
				importDefinition();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop42;
			}
			
		} while (true);
		}
		{
		_loop44:
		do {
			if ((_tokenSet_10.member(LA(1)))) {
				typeDefinition();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop44;
			}
			
		} while (true);
		}
		match(Token.EOF_TYPE);
		compilationUnit_AST = (AST)currentAST.root;
		returnAST = compilationUnit_AST;
	}
	
	public final void statement() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST statement_AST = null;
		AST m_AST = null;
		Token  c = null;
		AST c_AST = null;
		Token  s = null;
		AST s_AST = null;
		
		switch ( LA(1)) {
		case LCURLY:
		{
			compoundStatement();
			astFactory.addASTChild(currentAST, returnAST);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_if:
		{
			AST tmp2_AST = null;
			tmp2_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp2_AST);
			match(LITERAL_if);
			match(LPAREN);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			{
			if ((LA(1)==LITERAL_else) && (_tokenSet_8.member(LA(2)))) {
				match(LITERAL_else);
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((_tokenSet_11.member(LA(1))) && (_tokenSet_12.member(LA(2)))) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_for:
		{
			AST tmp6_AST = null;
			tmp6_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp6_AST);
			match(LITERAL_for);
			match(LPAREN);
			forInit();
			astFactory.addASTChild(currentAST, returnAST);
			match(SEMI);
			forCond();
			astFactory.addASTChild(currentAST, returnAST);
			match(SEMI);
			forIter();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_while:
		{
			AST tmp11_AST = null;
			tmp11_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp11_AST);
			match(LITERAL_while);
			match(LPAREN);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_do:
		{
			AST tmp14_AST = null;
			tmp14_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp14_AST);
			match(LITERAL_do);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			match(LITERAL_while);
			match(LPAREN);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_break:
		{
			AST tmp19_AST = null;
			tmp19_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp19_AST);
			match(LITERAL_break);
			{
			if ((LA(1)==IDENT)) {
				AST tmp20_AST = null;
				tmp20_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp20_AST);
				match(IDENT);
			}
			else if ((LA(1)==SEMI)) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_continue:
		{
			AST tmp22_AST = null;
			tmp22_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp22_AST);
			match(LITERAL_continue);
			{
			if ((LA(1)==IDENT)) {
				AST tmp23_AST = null;
				tmp23_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp23_AST);
				match(IDENT);
			}
			else if ((LA(1)==SEMI)) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_return:
		{
			AST tmp25_AST = null;
			tmp25_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp25_AST);
			match(LITERAL_return);
			{
			if ((_tokenSet_13.member(LA(1)))) {
				expression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((LA(1)==SEMI)) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_switch:
		{
			AST tmp27_AST = null;
			tmp27_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp27_AST);
			match(LITERAL_switch);
			match(LPAREN);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			match(LCURLY);
			{
			_loop126:
			do {
				if ((LA(1)==LITERAL_case||LA(1)==LITERAL_default)) {
					casesGroup();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop126;
				}
				
			} while (true);
			}
			match(RCURLY);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_try:
		{
			tryBlock();
			astFactory.addASTChild(currentAST, returnAST);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_throw:
		{
			AST tmp32_AST = null;
			tmp32_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp32_AST);
			match(LITERAL_throw);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_assert:
		{
			AST tmp34_AST = null;
			tmp34_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp34_AST);
			match(LITERAL_assert);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			{
			if ((LA(1)==COLON)) {
				match(COLON);
				expression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((LA(1)==SEMI)) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case SEMI:
		{
			s = LT(1);
			s_AST = astFactory.create(s);
			astFactory.addASTChild(currentAST, s_AST);
			match(SEMI);
			if ( inputState.guessing==0 ) {
				s_AST.setType(EMPTY_STAT);
			}
			statement_AST = (AST)currentAST.root;
			break;
		}
		default:
			boolean synPredMatched120 = false;
			if (((_tokenSet_14.member(LA(1))) && (_tokenSet_15.member(LA(2))))) {
				int _m120 = mark();
				synPredMatched120 = true;
				inputState.guessing++;
				try {
					{
					declaration();
					}
				}
				catch (RecognitionException pe) {
					synPredMatched120 = false;
				}
				rewind(_m120);
				inputState.guessing--;
			}
			if ( synPredMatched120 ) {
				declaration();
				astFactory.addASTChild(currentAST, returnAST);
				match(SEMI);
				statement_AST = (AST)currentAST.root;
			}
			else if ((_tokenSet_13.member(LA(1))) && (_tokenSet_16.member(LA(2)))) {
				expression();
				astFactory.addASTChild(currentAST, returnAST);
				match(SEMI);
				statement_AST = (AST)currentAST.root;
			}
			else if ((_tokenSet_17.member(LA(1))) && (_tokenSet_18.member(LA(2)))) {
				modifiers();
				m_AST = (AST)returnAST;
				classDefinition(m_AST);
				astFactory.addASTChild(currentAST, returnAST);
				statement_AST = (AST)currentAST.root;
			}
			else if ((LA(1)==IDENT) && (LA(2)==COLON)) {
				AST tmp39_AST = null;
				tmp39_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp39_AST);
				match(IDENT);
				c = LT(1);
				c_AST = astFactory.create(c);
				astFactory.makeASTRoot(currentAST, c_AST);
				match(COLON);
				if ( inputState.guessing==0 ) {
					c_AST.setType(LABELED_STAT);
				}
				statement();
				astFactory.addASTChild(currentAST, returnAST);
				statement_AST = (AST)currentAST.root;
			}
			else if ((LA(1)==LITERAL_synchronized) && (LA(2)==LPAREN)) {
				AST tmp40_AST = null;
				tmp40_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp40_AST);
				match(LITERAL_synchronized);
				match(LPAREN);
				expression();
				astFactory.addASTChild(currentAST, returnAST);
				match(RPAREN);
				compoundStatement();
				astFactory.addASTChild(currentAST, returnAST);
				statement_AST = (AST)currentAST.root;
			}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = statement_AST;
	}
	
	public final void constant() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST constant_AST = null;
		
		if ((LA(1)==NUM_INT)) {
			AST tmp43_AST = null;
			tmp43_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp43_AST);
			match(NUM_INT);
			constant_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==CHAR_LITERAL)) {
			AST tmp44_AST = null;
			tmp44_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp44_AST);
			match(CHAR_LITERAL);
			constant_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==STRING_LITERAL)) {
			AST tmp45_AST = null;
			tmp45_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp45_AST);
			match(STRING_LITERAL);
			constant_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==NUM_FLOAT)) {
			AST tmp46_AST = null;
			tmp46_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp46_AST);
			match(NUM_FLOAT);
			constant_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==NUM_LONG)) {
			AST tmp47_AST = null;
			tmp47_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp47_AST);
			match(NUM_LONG);
			constant_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==NUM_DOUBLE)) {
			AST tmp48_AST = null;
			tmp48_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp48_AST);
			match(NUM_DOUBLE);
			constant_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==WEBCOLOR_LITERAL)) {
			webcolor_literal();
			astFactory.addASTChild(currentAST, returnAST);
			constant_AST = (AST)currentAST.root;
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = constant_AST;
	}
	
	public final void webcolor_literal() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST webcolor_literal_AST = null;
		Token  w = null;
		AST w_AST = null;
		
		w = LT(1);
		w_AST = astFactory.create(w);
		astFactory.addASTChild(currentAST, w_AST);
		match(WEBCOLOR_LITERAL);
		if (!( Preferences.getBoolean("preproc.web_colors") && 
      w.getText().length() == 6 ))
		  throw new SemanticException(" Preferences.getBoolean(\"preproc.web_colors\") && \n      w.getText().length() == 6 ");
		webcolor_literal_AST = (AST)currentAST.root;
		returnAST = webcolor_literal_AST;
	}
	
	public final void builtInConsCastType() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST builtInConsCastType_AST = null;
		
		if ((LA(1)==LITERAL_void)) {
			AST tmp49_AST = null;
			tmp49_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp49_AST);
			match(LITERAL_void);
			builtInConsCastType_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_boolean)) {
			AST tmp50_AST = null;
			tmp50_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp50_AST);
			match(LITERAL_boolean);
			builtInConsCastType_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_byte)) {
			AST tmp51_AST = null;
			tmp51_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp51_AST);
			match(LITERAL_byte);
			builtInConsCastType_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_char)) {
			AST tmp52_AST = null;
			tmp52_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp52_AST);
			match(LITERAL_char);
			builtInConsCastType_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_short)) {
			AST tmp53_AST = null;
			tmp53_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp53_AST);
			match(LITERAL_short);
			builtInConsCastType_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_int)) {
			AST tmp54_AST = null;
			tmp54_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp54_AST);
			match(LITERAL_int);
			builtInConsCastType_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_float)) {
			AST tmp55_AST = null;
			tmp55_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp55_AST);
			match(LITERAL_float);
			builtInConsCastType_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_long)) {
			AST tmp56_AST = null;
			tmp56_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp56_AST);
			match(LITERAL_long);
			builtInConsCastType_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_double)) {
			AST tmp57_AST = null;
			tmp57_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp57_AST);
			match(LITERAL_double);
			builtInConsCastType_AST = (AST)currentAST.root;
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = builtInConsCastType_AST;
	}
	
	public final void builtInType() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST builtInType_AST = null;
		
		if (((LA(1) >= LITERAL_void && LA(1) <= LITERAL_double))) {
			builtInConsCastType();
			astFactory.addASTChild(currentAST, returnAST);
			builtInType_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_color)) {
			AST tmp58_AST = null;
			tmp58_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp58_AST);
			match(LITERAL_color);
			if (!( Preferences.getBoolean("preproc.color_datatype") ))
			  throw new SemanticException(" Preferences.getBoolean(\"preproc.color_datatype\") ");
			builtInType_AST = (AST)currentAST.root;
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = builtInType_AST;
	}
	
	public final void constructorCast() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST constructorCast_AST = null;
		AST t_AST = null;
		AST e_AST = null;
		
		consCastTypeSpec(true);
		t_AST = (AST)returnAST;
		match(LPAREN);
		expression();
		e_AST = (AST)returnAST;
		match(RPAREN);
		if (!( e_AST.getType() != STRING_LITERAL ||
            ( t_AST.getType() == LITERAL_byte ||
              t_AST.getType() == LITERAL_double ||
              t_AST.getType() == LITERAL_float ||
              t_AST.getType() == LITERAL_int ||
              t_AST.getType() == LITERAL_long ||
              t_AST.getType() == LITERAL_short ) ))
		  throw new SemanticException(" e_AST.getType() != STRING_LITERAL ||\n            ( t_AST.getType() == LITERAL_byte ||\n              t_AST.getType() == LITERAL_double ||\n              t_AST.getType() == LITERAL_float ||\n              t_AST.getType() == LITERAL_int ||\n              t_AST.getType() == LITERAL_long ||\n              t_AST.getType() == LITERAL_short ) ");
		if ( inputState.guessing==0 ) {
			constructorCast_AST = (AST)currentAST.root;
			constructorCast_AST = (AST)astFactory.make( (new ASTArray(3)).add(astFactory.create(CONSTRUCTOR_CAST,"CONSTRUCTOR_CAST")).add(t_AST).add(e_AST));
			currentAST.root = constructorCast_AST;
			currentAST.child = constructorCast_AST!=null &&constructorCast_AST.getFirstChild()!=null ?
				constructorCast_AST.getFirstChild() : constructorCast_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = constructorCast_AST;
	}
	
	public final void consCastTypeSpec(
		boolean addImagNode
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST consCastTypeSpec_AST = null;
		
		builtInConsCastTypeSpec(addImagNode);
		astFactory.addASTChild(currentAST, returnAST);
		consCastTypeSpec_AST = (AST)currentAST.root;
		returnAST = consCastTypeSpec_AST;
	}
	
	public final void expression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST expression_AST = null;
		
		assignmentExpression();
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			expression_AST = (AST)currentAST.root;
			expression_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(EXPR,"EXPR")).add(expression_AST));
			currentAST.root = expression_AST;
			currentAST.child = expression_AST!=null &&expression_AST.getFirstChild()!=null ?
				expression_AST.getFirstChild() : expression_AST;
			currentAST.advanceChildToEnd();
		}
		expression_AST = (AST)currentAST.root;
		returnAST = expression_AST;
	}
	
	public final void builtInConsCastTypeSpec(
		boolean addImagNode
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST builtInConsCastTypeSpec_AST = null;
		
		builtInConsCastType();
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			builtInConsCastTypeSpec_AST = (AST)currentAST.root;
			
			if ( addImagNode ) {
			builtInConsCastTypeSpec_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(TYPE,"TYPE")).add(builtInConsCastTypeSpec_AST));
			}
			
			currentAST.root = builtInConsCastTypeSpec_AST;
			currentAST.child = builtInConsCastTypeSpec_AST!=null &&builtInConsCastTypeSpec_AST.getFirstChild()!=null ?
				builtInConsCastTypeSpec_AST.getFirstChild() : builtInConsCastTypeSpec_AST;
			currentAST.advanceChildToEnd();
		}
		builtInConsCastTypeSpec_AST = (AST)currentAST.root;
		returnAST = builtInConsCastTypeSpec_AST;
	}
	
	public final void colorMethodCall() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST colorMethodCall_AST = null;
		Token  c = null;
		AST c_AST = null;
		Token  lp = null;
		AST lp_AST = null;
		
		c = LT(1);
		c_AST = astFactory.create(c);
		astFactory.addASTChild(currentAST, c_AST);
		match(LITERAL_color);
		if ( inputState.guessing==0 ) {
			c_AST.setType(IDENT);
		}
		lp = LT(1);
		lp_AST = astFactory.create(lp);
		astFactory.makeASTRoot(currentAST, lp_AST);
		match(LPAREN);
		if ( inputState.guessing==0 ) {
			lp_AST.setType(METHOD_CALL);
		}
		argList();
		astFactory.addASTChild(currentAST, returnAST);
		match(RPAREN);
		colorMethodCall_AST = (AST)currentAST.root;
		returnAST = colorMethodCall_AST;
	}
	
	public final void argList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST argList_AST = null;
		
		{
		if ((_tokenSet_13.member(LA(1)))) {
			expressionList();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((LA(1)==RPAREN)) {
			if ( inputState.guessing==0 ) {
				argList_AST = (AST)currentAST.root;
				argList_AST = astFactory.create(ELIST,"ELIST");
				currentAST.root = argList_AST;
				currentAST.child = argList_AST!=null &&argList_AST.getFirstChild()!=null ?
					argList_AST.getFirstChild() : argList_AST;
				currentAST.advanceChildToEnd();
			}
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		argList_AST = (AST)currentAST.root;
		returnAST = argList_AST;
	}
	
	public final void primaryExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST primaryExpression_AST = null;
		Token  lbt = null;
		AST lbt_AST = null;
		
		boolean synPredMatched25 = false;
		if ((((LA(1) >= LITERAL_void && LA(1) <= LITERAL_double)) && (LA(2)==LPAREN))) {
			int _m25 = mark();
			synPredMatched25 = true;
			inputState.guessing++;
			try {
				{
				consCastTypeSpec(false);
				match(LPAREN);
				}
			}
			catch (RecognitionException pe) {
				synPredMatched25 = false;
			}
			rewind(_m25);
			inputState.guessing--;
		}
		if ( synPredMatched25 ) {
			constructorCast();
			astFactory.addASTChild(currentAST, returnAST);
			if (!( Preferences.getBoolean("preproc.enhanced_casting") ))
			  throw new SemanticException(" Preferences.getBoolean(\"preproc.enhanced_casting\") ");
			primaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==IDENT)) {
			identPrimary();
			astFactory.addASTChild(currentAST, returnAST);
			{
			if ((LA(1)==DOT) && (LA(2)==LITERAL_class)) {
				AST tmp62_AST = null;
				tmp62_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp62_AST);
				match(DOT);
				AST tmp63_AST = null;
				tmp63_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp63_AST);
				match(LITERAL_class);
			}
			else if ((_tokenSet_19.member(LA(1))) && (_tokenSet_20.member(LA(2)))) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			primaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==NUM_INT||LA(1)==CHAR_LITERAL||LA(1)==STRING_LITERAL||LA(1)==NUM_FLOAT||LA(1)==NUM_LONG||LA(1)==NUM_DOUBLE||LA(1)==WEBCOLOR_LITERAL)) {
			constant();
			astFactory.addASTChild(currentAST, returnAST);
			primaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_true)) {
			AST tmp64_AST = null;
			tmp64_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp64_AST);
			match(LITERAL_true);
			primaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_false)) {
			AST tmp65_AST = null;
			tmp65_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp65_AST);
			match(LITERAL_false);
			primaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_null)) {
			AST tmp66_AST = null;
			tmp66_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp66_AST);
			match(LITERAL_null);
			primaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_new)) {
			newExpression();
			astFactory.addASTChild(currentAST, returnAST);
			primaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_this)) {
			AST tmp67_AST = null;
			tmp67_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp67_AST);
			match(LITERAL_this);
			primaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_super)) {
			AST tmp68_AST = null;
			tmp68_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp68_AST);
			match(LITERAL_super);
			primaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LPAREN)) {
			match(LPAREN);
			assignmentExpression();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			primaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LITERAL_color) && (LA(2)==LPAREN)) {
			colorMethodCall();
			astFactory.addASTChild(currentAST, returnAST);
			primaryExpression_AST = (AST)currentAST.root;
		}
		else if ((_tokenSet_21.member(LA(1))) && (LA(2)==LBRACK||LA(2)==DOT)) {
			builtInType();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop28:
			do {
				if ((LA(1)==LBRACK)) {
					lbt = LT(1);
					lbt_AST = astFactory.create(lbt);
					astFactory.makeASTRoot(currentAST, lbt_AST);
					match(LBRACK);
					if ( inputState.guessing==0 ) {
						lbt_AST.setType(ARRAY_DECLARATOR);
					}
					match(RBRACK);
				}
				else {
					break _loop28;
				}
				
			} while (true);
			}
			AST tmp72_AST = null;
			tmp72_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp72_AST);
			match(DOT);
			AST tmp73_AST = null;
			tmp73_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp73_AST);
			match(LITERAL_class);
			primaryExpression_AST = (AST)currentAST.root;
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = primaryExpression_AST;
	}
	
	public final void identPrimary() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST identPrimary_AST = null;
		Token  lp = null;
		AST lp_AST = null;
		Token  lbc = null;
		AST lbc_AST = null;
		
		AST tmp74_AST = null;
		tmp74_AST = astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp74_AST);
		match(IDENT);
		{
		_loop209:
		do {
			if ((LA(1)==DOT) && (LA(2)==IDENT)) {
				AST tmp75_AST = null;
				tmp75_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp75_AST);
				match(DOT);
				AST tmp76_AST = null;
				tmp76_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp76_AST);
				match(IDENT);
			}
			else {
				break _loop209;
			}
			
		} while (true);
		}
		{
		if ((LA(1)==LPAREN)) {
			{
			lp = LT(1);
			lp_AST = astFactory.create(lp);
			astFactory.makeASTRoot(currentAST, lp_AST);
			match(LPAREN);
			if ( inputState.guessing==0 ) {
				lp_AST.setType(METHOD_CALL);
			}
			argList();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			}
		}
		else if ((LA(1)==LBRACK) && (LA(2)==RBRACK)) {
			{
			int _cnt213=0;
			_loop213:
			do {
				if ((LA(1)==LBRACK) && (LA(2)==RBRACK)) {
					lbc = LT(1);
					lbc_AST = astFactory.create(lbc);
					astFactory.makeASTRoot(currentAST, lbc_AST);
					match(LBRACK);
					if ( inputState.guessing==0 ) {
						lbc_AST.setType(ARRAY_DECLARATOR);
					}
					match(RBRACK);
				}
				else {
					if ( _cnt213>=1 ) { break _loop213; } else {throw new NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt213++;
			} while (true);
			}
		}
		else if ((_tokenSet_19.member(LA(1))) && (_tokenSet_20.member(LA(2)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		identPrimary_AST = (AST)currentAST.root;
		returnAST = identPrimary_AST;
	}
	
	public final void newExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST newExpression_AST = null;
		
		AST tmp79_AST = null;
		tmp79_AST = astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp79_AST);
		match(LITERAL_new);
		type();
		astFactory.addASTChild(currentAST, returnAST);
		{
		if ((LA(1)==LPAREN)) {
			match(LPAREN);
			argList();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			{
			if ((LA(1)==LCURLY)) {
				classBlock();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((_tokenSet_19.member(LA(1)))) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else if ((LA(1)==LBRACK)) {
			newArrayDeclarator();
			astFactory.addASTChild(currentAST, returnAST);
			{
			if ((LA(1)==LCURLY)) {
				arrayInitializer();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((_tokenSet_19.member(LA(1)))) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		newExpression_AST = (AST)currentAST.root;
		returnAST = newExpression_AST;
	}
	
	public final void assignmentExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST assignmentExpression_AST = null;
		
		conditionalExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		if ((_tokenSet_22.member(LA(1)))) {
			{
			switch ( LA(1)) {
			case ASSIGN:
			{
				AST tmp82_AST = null;
				tmp82_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp82_AST);
				match(ASSIGN);
				break;
			}
			case PLUS_ASSIGN:
			{
				AST tmp83_AST = null;
				tmp83_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp83_AST);
				match(PLUS_ASSIGN);
				break;
			}
			case MINUS_ASSIGN:
			{
				AST tmp84_AST = null;
				tmp84_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp84_AST);
				match(MINUS_ASSIGN);
				break;
			}
			case STAR_ASSIGN:
			{
				AST tmp85_AST = null;
				tmp85_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp85_AST);
				match(STAR_ASSIGN);
				break;
			}
			case DIV_ASSIGN:
			{
				AST tmp86_AST = null;
				tmp86_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp86_AST);
				match(DIV_ASSIGN);
				break;
			}
			case MOD_ASSIGN:
			{
				AST tmp87_AST = null;
				tmp87_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp87_AST);
				match(MOD_ASSIGN);
				break;
			}
			case SR_ASSIGN:
			{
				AST tmp88_AST = null;
				tmp88_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp88_AST);
				match(SR_ASSIGN);
				break;
			}
			case BSR_ASSIGN:
			{
				AST tmp89_AST = null;
				tmp89_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp89_AST);
				match(BSR_ASSIGN);
				break;
			}
			case SL_ASSIGN:
			{
				AST tmp90_AST = null;
				tmp90_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp90_AST);
				match(SL_ASSIGN);
				break;
			}
			case BAND_ASSIGN:
			{
				AST tmp91_AST = null;
				tmp91_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp91_AST);
				match(BAND_ASSIGN);
				break;
			}
			case BXOR_ASSIGN:
			{
				AST tmp92_AST = null;
				tmp92_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp92_AST);
				match(BXOR_ASSIGN);
				break;
			}
			case BOR_ASSIGN:
			{
				AST tmp93_AST = null;
				tmp93_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp93_AST);
				match(BOR_ASSIGN);
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			assignmentExpression();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((LA(1)==SEMI||LA(1)==RBRACK||LA(1)==IDENT||LA(1)==RCURLY||LA(1)==COMMA||LA(1)==RPAREN||LA(1)==COLON)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		assignmentExpression_AST = (AST)currentAST.root;
		returnAST = assignmentExpression_AST;
	}
	
	public final void variableDefinitions(
		AST mods, AST t
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST variableDefinitions_AST = null;
		AST vd_AST = null;
		
		variableDeclarator(getASTFactory().dupTree(mods),
                              getASTFactory().dupTree(t));
		vd_AST = (AST)returnAST;
		if ( inputState.guessing==0 ) {
			variableDefinitions_AST = (AST)currentAST.root;
			variableDefinitions_AST = (AST)astFactory.make( (new ASTArray(4)).add(astFactory.create(VARIABLE_DEF,"VARIABLE_DEF")).add(mods).add(t).add(vd_AST));
			currentAST.root = variableDefinitions_AST;
			currentAST.child = variableDefinitions_AST!=null &&variableDefinitions_AST.getFirstChild()!=null ?
				variableDefinitions_AST.getFirstChild() : variableDefinitions_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = variableDefinitions_AST;
	}
	
	public final void variableDeclarator(
		AST mods, AST t
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST variableDeclarator_AST = null;
		Token  id = null;
		AST id_AST = null;
		Token  lb = null;
		AST lb_AST = null;
		AST v_AST = null;
		
		{
		int _cnt35=0;
		_loop35:
		do {
			if ((LA(1)==IDENT)) {
				id = LT(1);
				id_AST = astFactory.create(id);
				astFactory.addASTChild(currentAST, id_AST);
				match(IDENT);
				{
				_loop33:
				do {
					if ((LA(1)==LBRACK)) {
						lb = LT(1);
						lb_AST = astFactory.create(lb);
						astFactory.makeASTRoot(currentAST, lb_AST);
						match(LBRACK);
						if ( inputState.guessing==0 ) {
							lb_AST.setType(ARRAY_DECLARATOR);
						}
						match(RBRACK);
					}
					else {
						break _loop33;
					}
					
				} while (true);
				}
				varInitializer();
				v_AST = (AST)returnAST;
				astFactory.addASTChild(currentAST, returnAST);
				{
				if ((LA(1)==COMMA)) {
					match(COMMA);
				}
				else if ((LA(1)==SEMI||LA(1)==IDENT)) {
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
			}
			else {
				if ( _cnt35>=1 ) { break _loop35; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt35++;
		} while (true);
		}
		variableDeclarator_AST = (AST)currentAST.root;
		returnAST = variableDeclarator_AST;
	}
	
	public final void varInitializer() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST varInitializer_AST = null;
		
		{
		if ((LA(1)==ASSIGN)) {
			AST tmp96_AST = null;
			tmp96_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp96_AST);
			match(ASSIGN);
			initializer();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((LA(1)==SEMI||LA(1)==IDENT||LA(1)==COMMA)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		varInitializer_AST = (AST)currentAST.root;
		returnAST = varInitializer_AST;
	}
	
	public final void explicitConstructorInvocation() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST explicitConstructorInvocation_AST = null;
		Token  t = null;
		AST t_AST = null;
		AST a1_AST = null;
		Token  s = null;
		AST s_AST = null;
		AST a2_AST = null;
		
		if ((LA(1)==LITERAL_this)) {
			t = LT(1);
			t_AST = astFactory.create(t);
			match(LITERAL_this);
			AST tmp97_AST = null;
			tmp97_AST = astFactory.create(LT(1));
			match(LPAREN);
			argList();
			a1_AST = (AST)returnAST;
			AST tmp98_AST = null;
			tmp98_AST = astFactory.create(LT(1));
			match(RPAREN);
			AST tmp99_AST = null;
			tmp99_AST = astFactory.create(LT(1));
			match(SEMI);
			if ( inputState.guessing==0 ) {
				explicitConstructorInvocation_AST = (AST)currentAST.root;
				explicitConstructorInvocation_AST = (AST)astFactory.make( (new ASTArray(3)).add(astFactory.create(CTOR_CALL,"CTOR_CALL")).add(t_AST).add(a1_AST));
				currentAST.root = explicitConstructorInvocation_AST;
				currentAST.child = explicitConstructorInvocation_AST!=null &&explicitConstructorInvocation_AST.getFirstChild()!=null ?
					explicitConstructorInvocation_AST.getFirstChild() : explicitConstructorInvocation_AST;
				currentAST.advanceChildToEnd();
			}
		}
		else if ((LA(1)==LITERAL_super)) {
			s = LT(1);
			s_AST = astFactory.create(s);
			match(LITERAL_super);
			AST tmp100_AST = null;
			tmp100_AST = astFactory.create(LT(1));
			match(LPAREN);
			argList();
			a2_AST = (AST)returnAST;
			AST tmp101_AST = null;
			tmp101_AST = astFactory.create(LT(1));
			match(RPAREN);
			AST tmp102_AST = null;
			tmp102_AST = astFactory.create(LT(1));
			match(SEMI);
			if ( inputState.guessing==0 ) {
				explicitConstructorInvocation_AST = (AST)currentAST.root;
				explicitConstructorInvocation_AST = (AST)astFactory.make( (new ASTArray(3)).add(astFactory.create(SUPER_CTOR_CALL,"SUPER_CTOR_CALL")).add(s_AST).add(a2_AST));
				currentAST.root = explicitConstructorInvocation_AST;
				currentAST.child = explicitConstructorInvocation_AST!=null &&explicitConstructorInvocation_AST.getFirstChild()!=null ?
					explicitConstructorInvocation_AST.getFirstChild() : explicitConstructorInvocation_AST;
				currentAST.advanceChildToEnd();
			}
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = explicitConstructorInvocation_AST;
	}
	
	public final void classDefinition(
		AST modifiers
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST classDefinition_AST = null;
		Token  i = null;
		AST i_AST = null;
		AST sc_AST = null;
		AST ic_AST = null;
		AST cb_AST = null;
		
		match(LITERAL_class);
		i = LT(1);
		i_AST = astFactory.create(i);
		match(IDENT);
		superClassClause();
		sc_AST = (AST)returnAST;
		implementsClause();
		ic_AST = (AST)returnAST;
		classBlock();
		cb_AST = (AST)returnAST;
		if ( inputState.guessing==0 ) {
			classDefinition_AST = (AST)currentAST.root;
			classDefinition_AST = (AST)astFactory.make( (new ASTArray(6)).add(astFactory.create(CLASS_DEF,"CLASS_DEF")).add(modifiers).add(i_AST).add(sc_AST).add(ic_AST).add(cb_AST));
			PdePreprocessor.advClassName = i.getText();
			currentAST.root = classDefinition_AST;
			currentAST.child = classDefinition_AST!=null &&classDefinition_AST.getFirstChild()!=null ?
				classDefinition_AST.getFirstChild() : classDefinition_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = classDefinition_AST;
	}
	
	public final void superClassClause() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST superClassClause_AST = null;
		AST id_AST = null;
		
		{
		if ((LA(1)==LITERAL_extends)) {
			match(LITERAL_extends);
			identifier();
			id_AST = (AST)returnAST;
		}
		else if ((LA(1)==LCURLY||LA(1)==LITERAL_implements)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			superClassClause_AST = (AST)currentAST.root;
			superClassClause_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(EXTENDS_CLAUSE,"EXTENDS_CLAUSE")).add(id_AST));
			currentAST.root = superClassClause_AST;
			currentAST.child = superClassClause_AST!=null &&superClassClause_AST.getFirstChild()!=null ?
				superClassClause_AST.getFirstChild() : superClassClause_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = superClassClause_AST;
	}
	
	public final void implementsClause() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST implementsClause_AST = null;
		Token  i = null;
		AST i_AST = null;
		
		{
		if ((LA(1)==LITERAL_implements)) {
			i = LT(1);
			i_AST = astFactory.create(i);
			match(LITERAL_implements);
			identifier();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop82:
			do {
				if ((LA(1)==COMMA)) {
					match(COMMA);
					identifier();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop82;
				}
				
			} while (true);
			}
		}
		else if ((LA(1)==LCURLY)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			implementsClause_AST = (AST)currentAST.root;
			implementsClause_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(IMPLEMENTS_CLAUSE,"IMPLEMENTS_CLAUSE")).add(implementsClause_AST));
			currentAST.root = implementsClause_AST;
			currentAST.child = implementsClause_AST!=null &&implementsClause_AST.getFirstChild()!=null ?
				implementsClause_AST.getFirstChild() : implementsClause_AST;
			currentAST.advanceChildToEnd();
		}
		implementsClause_AST = (AST)currentAST.root;
		returnAST = implementsClause_AST;
	}
	
	public final void classBlock() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST classBlock_AST = null;
		
		match(LCURLY);
		{
		_loop74:
		do {
			if ((_tokenSet_7.member(LA(1)))) {
				field();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((LA(1)==SEMI)) {
				match(SEMI);
			}
			else {
				break _loop74;
			}
			
		} while (true);
		}
		match(RCURLY);
		if ( inputState.guessing==0 ) {
			classBlock_AST = (AST)currentAST.root;
			classBlock_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(OBJBLOCK,"OBJBLOCK")).add(classBlock_AST));
			currentAST.root = classBlock_AST;
			currentAST.child = classBlock_AST!=null &&classBlock_AST.getFirstChild()!=null ?
				classBlock_AST.getFirstChild() : classBlock_AST;
			currentAST.advanceChildToEnd();
		}
		classBlock_AST = (AST)currentAST.root;
		returnAST = classBlock_AST;
	}
	
	public final void field() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST field_AST = null;
		AST mods_AST = null;
		AST h_AST = null;
		AST s_AST = null;
		AST cd_AST = null;
		AST id_AST = null;
		AST t_AST = null;
		AST param_AST = null;
		AST rt_AST = null;
		AST tc_AST = null;
		AST s2_AST = null;
		AST v_AST = null;
		AST s3_AST = null;
		AST s4_AST = null;
		
		if ((_tokenSet_23.member(LA(1))) && (_tokenSet_24.member(LA(2)))) {
			modifiers();
			mods_AST = (AST)returnAST;
			{
			if ((LA(1)==IDENT) && (LA(2)==LPAREN)) {
				ctorHead();
				h_AST = (AST)returnAST;
				constructorBody();
				s_AST = (AST)returnAST;
				if ( inputState.guessing==0 ) {
					field_AST = (AST)currentAST.root;
					field_AST = (AST)astFactory.make( (new ASTArray(4)).add(astFactory.create(CTOR_DEF,"CTOR_DEF")).add(mods_AST).add(h_AST).add(s_AST));
					currentAST.root = field_AST;
					currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
						field_AST.getFirstChild() : field_AST;
					currentAST.advanceChildToEnd();
				}
			}
			else if ((LA(1)==LITERAL_class)) {
				classDefinition(mods_AST);
				cd_AST = (AST)returnAST;
				if ( inputState.guessing==0 ) {
					field_AST = (AST)currentAST.root;
					field_AST = cd_AST;
					currentAST.root = field_AST;
					currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
						field_AST.getFirstChild() : field_AST;
					currentAST.advanceChildToEnd();
				}
			}
			else if ((LA(1)==LITERAL_interface)) {
				interfaceDefinition(mods_AST);
				id_AST = (AST)returnAST;
				if ( inputState.guessing==0 ) {
					field_AST = (AST)currentAST.root;
					field_AST = id_AST;
					currentAST.root = field_AST;
					currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
						field_AST.getFirstChild() : field_AST;
					currentAST.advanceChildToEnd();
				}
			}
			else if ((_tokenSet_25.member(LA(1))) && (LA(2)==LBRACK||LA(2)==IDENT||LA(2)==DOT)) {
				typeSpec(false);
				t_AST = (AST)returnAST;
				{
				if ((LA(1)==IDENT) && (LA(2)==LPAREN)) {
					AST tmp109_AST = null;
					tmp109_AST = astFactory.create(LT(1));
					match(IDENT);
					match(LPAREN);
					parameterDeclarationList();
					param_AST = (AST)returnAST;
					match(RPAREN);
					declaratorBrackets(t_AST);
					rt_AST = (AST)returnAST;
					{
					if ((LA(1)==LITERAL_throws)) {
						throwsClause();
						tc_AST = (AST)returnAST;
					}
					else if ((LA(1)==SEMI||LA(1)==LCURLY)) {
					}
					else {
						throw new NoViableAltException(LT(1), getFilename());
					}
					
					}
					{
					if ((LA(1)==LCURLY)) {
						compoundStatement();
						s2_AST = (AST)returnAST;
					}
					else if ((LA(1)==SEMI)) {
						AST tmp112_AST = null;
						tmp112_AST = astFactory.create(LT(1));
						match(SEMI);
					}
					else {
						throw new NoViableAltException(LT(1), getFilename());
					}
					
					}
					if ( inputState.guessing==0 ) {
						field_AST = (AST)currentAST.root;
						field_AST = (AST)astFactory.make( (new ASTArray(7)).add(astFactory.create(METHOD_DEF,"METHOD_DEF")).add(mods_AST).add((AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(TYPE,"TYPE")).add(rt_AST))).add(tmp109_AST).add(param_AST).add(tc_AST).add(s2_AST));
						currentAST.root = field_AST;
						currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
							field_AST.getFirstChild() : field_AST;
						currentAST.advanceChildToEnd();
					}
				}
				else if ((LA(1)==IDENT) && (LA(2)==SEMI||LA(2)==LBRACK||LA(2)==IDENT||LA(2)==COMMA||LA(2)==ASSIGN)) {
					variableDefinitions(mods_AST,t_AST);
					v_AST = (AST)returnAST;
					AST tmp113_AST = null;
					tmp113_AST = astFactory.create(LT(1));
					match(SEMI);
					if ( inputState.guessing==0 ) {
						field_AST = (AST)currentAST.root;
						field_AST = v_AST;
						currentAST.root = field_AST;
						currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
							field_AST.getFirstChild() : field_AST;
						currentAST.advanceChildToEnd();
					}
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else if ((LA(1)==LITERAL_static) && (LA(2)==LCURLY)) {
			match(LITERAL_static);
			compoundStatement();
			s3_AST = (AST)returnAST;
			if ( inputState.guessing==0 ) {
				field_AST = (AST)currentAST.root;
				field_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(STATIC_INIT,"STATIC_INIT")).add(s3_AST));
				currentAST.root = field_AST;
				currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
					field_AST.getFirstChild() : field_AST;
				currentAST.advanceChildToEnd();
			}
		}
		else if ((LA(1)==LCURLY)) {
			compoundStatement();
			s4_AST = (AST)returnAST;
			if ( inputState.guessing==0 ) {
				field_AST = (AST)currentAST.root;
				field_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(INSTANCE_INIT,"INSTANCE_INIT")).add(s4_AST));
				currentAST.root = field_AST;
				currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
					field_AST.getFirstChild() : field_AST;
				currentAST.advanceChildToEnd();
			}
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = field_AST;
	}
	
	public final void packageDefinition() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST packageDefinition_AST = null;
		Token  p = null;
		AST p_AST = null;
		
		try {      // for error handling
			p = LT(1);
			p_AST = astFactory.create(p);
			astFactory.makeASTRoot(currentAST, p_AST);
			match(LITERAL_package);
			if ( inputState.guessing==0 ) {
				p_AST.setType(PACKAGE_DEF);
			}
			identifier();
			astFactory.addASTChild(currentAST, returnAST);
			match(SEMI);
			packageDefinition_AST = (AST)currentAST.root;
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				consume();
				consumeUntil(_tokenSet_9);
			} else {
			  throw ex;
			}
		}
		returnAST = packageDefinition_AST;
	}
	
	public final void importDefinition() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST importDefinition_AST = null;
		Token  i = null;
		AST i_AST = null;
		
		try {      // for error handling
			i = LT(1);
			i_AST = astFactory.create(i);
			astFactory.makeASTRoot(currentAST, i_AST);
			match(LITERAL_import);
			if ( inputState.guessing==0 ) {
				i_AST.setType(IMPORT);
			}
			identifierStar();
			astFactory.addASTChild(currentAST, returnAST);
			match(SEMI);
			importDefinition_AST = (AST)currentAST.root;
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				consume();
				consumeUntil(_tokenSet_9);
			} else {
			  throw ex;
			}
		}
		returnAST = importDefinition_AST;
	}
	
	public final void typeDefinition() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST typeDefinition_AST = null;
		AST m_AST = null;
		
		try {      // for error handling
			if ((_tokenSet_26.member(LA(1)))) {
				modifiers();
				m_AST = (AST)returnAST;
				{
				if ((LA(1)==LITERAL_class)) {
					classDefinition(m_AST);
					astFactory.addASTChild(currentAST, returnAST);
				}
				else if ((LA(1)==LITERAL_interface)) {
					interfaceDefinition(m_AST);
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
				typeDefinition_AST = (AST)currentAST.root;
			}
			else if ((LA(1)==SEMI)) {
				match(SEMI);
				typeDefinition_AST = (AST)currentAST.root;
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				consume();
				consumeUntil(_tokenSet_27);
			} else {
			  throw ex;
			}
		}
		returnAST = typeDefinition_AST;
	}
	
	public final void identifier() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST identifier_AST = null;
		
		AST tmp118_AST = null;
		tmp118_AST = astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp118_AST);
		match(IDENT);
		{
		_loop60:
		do {
			if ((LA(1)==DOT)) {
				AST tmp119_AST = null;
				tmp119_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp119_AST);
				match(DOT);
				AST tmp120_AST = null;
				tmp120_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp120_AST);
				match(IDENT);
			}
			else {
				break _loop60;
			}
			
		} while (true);
		}
		identifier_AST = (AST)currentAST.root;
		returnAST = identifier_AST;
	}
	
	public final void identifierStar() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST identifierStar_AST = null;
		
		AST tmp121_AST = null;
		tmp121_AST = astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp121_AST);
		match(IDENT);
		{
		_loop63:
		do {
			if ((LA(1)==DOT) && (LA(2)==IDENT)) {
				AST tmp122_AST = null;
				tmp122_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp122_AST);
				match(DOT);
				AST tmp123_AST = null;
				tmp123_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp123_AST);
				match(IDENT);
			}
			else {
				break _loop63;
			}
			
		} while (true);
		}
		{
		if ((LA(1)==DOT)) {
			AST tmp124_AST = null;
			tmp124_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp124_AST);
			match(DOT);
			AST tmp125_AST = null;
			tmp125_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp125_AST);
			match(STAR);
		}
		else if ((LA(1)==SEMI)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		identifierStar_AST = (AST)currentAST.root;
		returnAST = identifierStar_AST;
	}
	
	public final void modifiers() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST modifiers_AST = null;
		
		{
		_loop67:
		do {
			if ((_tokenSet_28.member(LA(1)))) {
				modifier();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop67;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			modifiers_AST = (AST)currentAST.root;
			modifiers_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(MODIFIERS,"MODIFIERS")).add(modifiers_AST));
			currentAST.root = modifiers_AST;
			currentAST.child = modifiers_AST!=null &&modifiers_AST.getFirstChild()!=null ?
				modifiers_AST.getFirstChild() : modifiers_AST;
			currentAST.advanceChildToEnd();
		}
		modifiers_AST = (AST)currentAST.root;
		returnAST = modifiers_AST;
	}
	
	public final void interfaceDefinition(
		AST modifiers
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST interfaceDefinition_AST = null;
		AST ie_AST = null;
		AST cb_AST = null;
		
		match(LITERAL_interface);
		AST tmp127_AST = null;
		tmp127_AST = astFactory.create(LT(1));
		match(IDENT);
		interfaceExtends();
		ie_AST = (AST)returnAST;
		classBlock();
		cb_AST = (AST)returnAST;
		if ( inputState.guessing==0 ) {
			interfaceDefinition_AST = (AST)currentAST.root;
			interfaceDefinition_AST = (AST)astFactory.make( (new ASTArray(5)).add(astFactory.create(INTERFACE_DEF,"INTERFACE_DEF")).add(modifiers).add(tmp127_AST).add(ie_AST).add(cb_AST));
			currentAST.root = interfaceDefinition_AST;
			currentAST.child = interfaceDefinition_AST!=null &&interfaceDefinition_AST.getFirstChild()!=null ?
				interfaceDefinition_AST.getFirstChild() : interfaceDefinition_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = interfaceDefinition_AST;
	}
	
	public final void declaration() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST declaration_AST = null;
		AST m_AST = null;
		AST t_AST = null;
		AST v_AST = null;
		
		modifiers();
		m_AST = (AST)returnAST;
		typeSpec(false);
		t_AST = (AST)returnAST;
		variableDefinitions(m_AST,t_AST);
		v_AST = (AST)returnAST;
		if ( inputState.guessing==0 ) {
			declaration_AST = (AST)currentAST.root;
			declaration_AST = v_AST;
			currentAST.root = declaration_AST;
			currentAST.child = declaration_AST!=null &&declaration_AST.getFirstChild()!=null ?
				declaration_AST.getFirstChild() : declaration_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = declaration_AST;
	}
	
	public final void typeSpec(
		boolean addImagNode
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST typeSpec_AST = null;
		
		if ((LA(1)==IDENT)) {
			classTypeSpec(addImagNode);
			astFactory.addASTChild(currentAST, returnAST);
			typeSpec_AST = (AST)currentAST.root;
		}
		else if ((_tokenSet_21.member(LA(1)))) {
			builtInTypeSpec(addImagNode);
			astFactory.addASTChild(currentAST, returnAST);
			typeSpec_AST = (AST)currentAST.root;
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = typeSpec_AST;
	}
	
	public final void classTypeSpec(
		boolean addImagNode
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST classTypeSpec_AST = null;
		Token  lb = null;
		AST lb_AST = null;
		
		identifier();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop53:
		do {
			if ((LA(1)==LBRACK)) {
				lb = LT(1);
				lb_AST = astFactory.create(lb);
				astFactory.makeASTRoot(currentAST, lb_AST);
				match(LBRACK);
				if ( inputState.guessing==0 ) {
					lb_AST.setType(ARRAY_DECLARATOR);
				}
				match(RBRACK);
			}
			else {
				break _loop53;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			classTypeSpec_AST = (AST)currentAST.root;
			
						if ( addImagNode ) {
							classTypeSpec_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(TYPE,"TYPE")).add(classTypeSpec_AST));
						}
					
			currentAST.root = classTypeSpec_AST;
			currentAST.child = classTypeSpec_AST!=null &&classTypeSpec_AST.getFirstChild()!=null ?
				classTypeSpec_AST.getFirstChild() : classTypeSpec_AST;
			currentAST.advanceChildToEnd();
		}
		classTypeSpec_AST = (AST)currentAST.root;
		returnAST = classTypeSpec_AST;
	}
	
	public final void builtInTypeSpec(
		boolean addImagNode
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST builtInTypeSpec_AST = null;
		Token  lb = null;
		AST lb_AST = null;
		
		builtInType();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop56:
		do {
			if ((LA(1)==LBRACK)) {
				lb = LT(1);
				lb_AST = astFactory.create(lb);
				astFactory.makeASTRoot(currentAST, lb_AST);
				match(LBRACK);
				if ( inputState.guessing==0 ) {
					lb_AST.setType(ARRAY_DECLARATOR);
				}
				match(RBRACK);
			}
			else {
				break _loop56;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			builtInTypeSpec_AST = (AST)currentAST.root;
			
						if ( addImagNode ) {
							builtInTypeSpec_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(TYPE,"TYPE")).add(builtInTypeSpec_AST));
						}
					
			currentAST.root = builtInTypeSpec_AST;
			currentAST.child = builtInTypeSpec_AST!=null &&builtInTypeSpec_AST.getFirstChild()!=null ?
				builtInTypeSpec_AST.getFirstChild() : builtInTypeSpec_AST;
			currentAST.advanceChildToEnd();
		}
		builtInTypeSpec_AST = (AST)currentAST.root;
		returnAST = builtInTypeSpec_AST;
	}
	
	public final void type() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST type_AST = null;
		
		if ((LA(1)==IDENT)) {
			identifier();
			astFactory.addASTChild(currentAST, returnAST);
			type_AST = (AST)currentAST.root;
		}
		else if ((_tokenSet_21.member(LA(1)))) {
			builtInType();
			astFactory.addASTChild(currentAST, returnAST);
			type_AST = (AST)currentAST.root;
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = type_AST;
	}
	
	public final void modifier() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST modifier_AST = null;
		
		switch ( LA(1)) {
		case LITERAL_private:
		{
			AST tmp130_AST = null;
			tmp130_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp130_AST);
			match(LITERAL_private);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_public:
		{
			AST tmp131_AST = null;
			tmp131_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp131_AST);
			match(LITERAL_public);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_protected:
		{
			AST tmp132_AST = null;
			tmp132_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp132_AST);
			match(LITERAL_protected);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_static:
		{
			AST tmp133_AST = null;
			tmp133_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp133_AST);
			match(LITERAL_static);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_transient:
		{
			AST tmp134_AST = null;
			tmp134_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp134_AST);
			match(LITERAL_transient);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case FINAL:
		{
			AST tmp135_AST = null;
			tmp135_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp135_AST);
			match(FINAL);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case ABSTRACT:
		{
			AST tmp136_AST = null;
			tmp136_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp136_AST);
			match(ABSTRACT);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_native:
		{
			AST tmp137_AST = null;
			tmp137_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp137_AST);
			match(LITERAL_native);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_threadsafe:
		{
			AST tmp138_AST = null;
			tmp138_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp138_AST);
			match(LITERAL_threadsafe);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_synchronized:
		{
			AST tmp139_AST = null;
			tmp139_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp139_AST);
			match(LITERAL_synchronized);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_volatile:
		{
			AST tmp140_AST = null;
			tmp140_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp140_AST);
			match(LITERAL_volatile);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case STRICTFP:
		{
			AST tmp141_AST = null;
			tmp141_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp141_AST);
			match(STRICTFP);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = modifier_AST;
	}
	
	public final void interfaceExtends() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST interfaceExtends_AST = null;
		Token  e = null;
		AST e_AST = null;
		
		{
		if ((LA(1)==LITERAL_extends)) {
			e = LT(1);
			e_AST = astFactory.create(e);
			match(LITERAL_extends);
			identifier();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop78:
			do {
				if ((LA(1)==COMMA)) {
					match(COMMA);
					identifier();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop78;
				}
				
			} while (true);
			}
		}
		else if ((LA(1)==LCURLY)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			interfaceExtends_AST = (AST)currentAST.root;
			interfaceExtends_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(EXTENDS_CLAUSE,"EXTENDS_CLAUSE")).add(interfaceExtends_AST));
			currentAST.root = interfaceExtends_AST;
			currentAST.child = interfaceExtends_AST!=null &&interfaceExtends_AST.getFirstChild()!=null ?
				interfaceExtends_AST.getFirstChild() : interfaceExtends_AST;
			currentAST.advanceChildToEnd();
		}
		interfaceExtends_AST = (AST)currentAST.root;
		returnAST = interfaceExtends_AST;
	}
	
	public final void ctorHead() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST ctorHead_AST = null;
		
		AST tmp143_AST = null;
		tmp143_AST = astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp143_AST);
		match(IDENT);
		match(LPAREN);
		parameterDeclarationList();
		astFactory.addASTChild(currentAST, returnAST);
		match(RPAREN);
		{
		if ((LA(1)==LITERAL_throws)) {
			throwsClause();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((LA(1)==LCURLY)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		ctorHead_AST = (AST)currentAST.root;
		returnAST = ctorHead_AST;
	}
	
	public final void constructorBody() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST constructorBody_AST = null;
		Token  lc = null;
		AST lc_AST = null;
		
		lc = LT(1);
		lc_AST = astFactory.create(lc);
		astFactory.makeASTRoot(currentAST, lc_AST);
		match(LCURLY);
		if ( inputState.guessing==0 ) {
			lc_AST.setType(SLIST);
		}
		{
		if ((LA(1)==LITERAL_this||LA(1)==LITERAL_super) && (LA(2)==LPAREN)) {
			explicitConstructorInvocation();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_29.member(LA(1))) && (_tokenSet_30.member(LA(2)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		{
		_loop91:
		do {
			if ((_tokenSet_8.member(LA(1)))) {
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop91;
			}
			
		} while (true);
		}
		match(RCURLY);
		constructorBody_AST = (AST)currentAST.root;
		returnAST = constructorBody_AST;
	}
	
	public final void parameterDeclarationList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST parameterDeclarationList_AST = null;
		
		{
		if ((_tokenSet_31.member(LA(1)))) {
			parameterDeclaration();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop111:
			do {
				if ((LA(1)==COMMA)) {
					match(COMMA);
					parameterDeclaration();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop111;
				}
				
			} while (true);
			}
		}
		else if ((LA(1)==RPAREN)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			parameterDeclarationList_AST = (AST)currentAST.root;
			parameterDeclarationList_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(PARAMETERS,"PARAMETERS")).add(parameterDeclarationList_AST));
			currentAST.root = parameterDeclarationList_AST;
			currentAST.child = parameterDeclarationList_AST!=null &&parameterDeclarationList_AST.getFirstChild()!=null ?
				parameterDeclarationList_AST.getFirstChild() : parameterDeclarationList_AST;
			currentAST.advanceChildToEnd();
		}
		parameterDeclarationList_AST = (AST)currentAST.root;
		returnAST = parameterDeclarationList_AST;
	}
	
	public final void declaratorBrackets(
		AST typ
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST declaratorBrackets_AST = null;
		Token  lb = null;
		AST lb_AST = null;
		
		if ( inputState.guessing==0 ) {
			declaratorBrackets_AST = (AST)currentAST.root;
			declaratorBrackets_AST=typ;
			currentAST.root = declaratorBrackets_AST;
			currentAST.child = declaratorBrackets_AST!=null &&declaratorBrackets_AST.getFirstChild()!=null ?
				declaratorBrackets_AST.getFirstChild() : declaratorBrackets_AST;
			currentAST.advanceChildToEnd();
		}
		{
		_loop94:
		do {
			if ((LA(1)==LBRACK)) {
				lb = LT(1);
				lb_AST = astFactory.create(lb);
				astFactory.makeASTRoot(currentAST, lb_AST);
				match(LBRACK);
				if ( inputState.guessing==0 ) {
					lb_AST.setType(ARRAY_DECLARATOR);
				}
				match(RBRACK);
			}
			else {
				break _loop94;
			}
			
		} while (true);
		}
		declaratorBrackets_AST = (AST)currentAST.root;
		returnAST = declaratorBrackets_AST;
	}
	
	public final void throwsClause() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST throwsClause_AST = null;
		
		AST tmp149_AST = null;
		tmp149_AST = astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp149_AST);
		match(LITERAL_throws);
		identifier();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop107:
		do {
			if ((LA(1)==COMMA)) {
				match(COMMA);
				identifier();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop107;
			}
			
		} while (true);
		}
		throwsClause_AST = (AST)currentAST.root;
		returnAST = throwsClause_AST;
	}
	
	public final void compoundStatement() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST compoundStatement_AST = null;
		Token  lc = null;
		AST lc_AST = null;
		
		lc = LT(1);
		lc_AST = astFactory.create(lc);
		astFactory.makeASTRoot(currentAST, lc_AST);
		match(LCURLY);
		if ( inputState.guessing==0 ) {
			lc_AST.setType(SLIST);
		}
		{
		_loop117:
		do {
			if ((_tokenSet_8.member(LA(1)))) {
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop117;
			}
			
		} while (true);
		}
		match(RCURLY);
		compoundStatement_AST = (AST)currentAST.root;
		returnAST = compoundStatement_AST;
	}
	
	public final void initializer() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST initializer_AST = null;
		
		if ((_tokenSet_13.member(LA(1)))) {
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			initializer_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LCURLY)) {
			arrayInitializer();
			astFactory.addASTChild(currentAST, returnAST);
			initializer_AST = (AST)currentAST.root;
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = initializer_AST;
	}
	
	public final void arrayInitializer() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST arrayInitializer_AST = null;
		Token  lc = null;
		AST lc_AST = null;
		
		lc = LT(1);
		lc_AST = astFactory.create(lc);
		astFactory.makeASTRoot(currentAST, lc_AST);
		match(LCURLY);
		if ( inputState.guessing==0 ) {
			lc_AST.setType(ARRAY_INIT);
		}
		{
		if ((_tokenSet_32.member(LA(1)))) {
			initializer();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop100:
			do {
				if ((LA(1)==COMMA) && (_tokenSet_32.member(LA(2)))) {
					match(COMMA);
					initializer();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop100;
				}
				
			} while (true);
			}
			{
			if ((LA(1)==COMMA)) {
				match(COMMA);
			}
			else if ((LA(1)==RCURLY)) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else if ((LA(1)==RCURLY)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		match(RCURLY);
		arrayInitializer_AST = (AST)currentAST.root;
		returnAST = arrayInitializer_AST;
	}
	
	public final void parameterDeclaration() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST parameterDeclaration_AST = null;
		AST pm_AST = null;
		AST t_AST = null;
		Token  id = null;
		AST id_AST = null;
		AST pd_AST = null;
		
		parameterModifier();
		pm_AST = (AST)returnAST;
		typeSpec(false);
		t_AST = (AST)returnAST;
		id = LT(1);
		id_AST = astFactory.create(id);
		match(IDENT);
		declaratorBrackets(t_AST);
		pd_AST = (AST)returnAST;
		if ( inputState.guessing==0 ) {
			parameterDeclaration_AST = (AST)currentAST.root;
			parameterDeclaration_AST = (AST)astFactory.make( (new ASTArray(4)).add(astFactory.create(PARAMETER_DEF,"PARAMETER_DEF")).add(pm_AST).add((AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(TYPE,"TYPE")).add(pd_AST))).add(id_AST));
			currentAST.root = parameterDeclaration_AST;
			currentAST.child = parameterDeclaration_AST!=null &&parameterDeclaration_AST.getFirstChild()!=null ?
				parameterDeclaration_AST.getFirstChild() : parameterDeclaration_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = parameterDeclaration_AST;
	}
	
	public final void parameterModifier() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST parameterModifier_AST = null;
		Token  f = null;
		AST f_AST = null;
		
		{
		if ((LA(1)==FINAL)) {
			f = LT(1);
			f_AST = astFactory.create(f);
			astFactory.addASTChild(currentAST, f_AST);
			match(FINAL);
		}
		else if ((_tokenSet_25.member(LA(1)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			parameterModifier_AST = (AST)currentAST.root;
			parameterModifier_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(MODIFIERS,"MODIFIERS")).add(f_AST));
			currentAST.root = parameterModifier_AST;
			currentAST.child = parameterModifier_AST!=null &&parameterModifier_AST.getFirstChild()!=null ?
				parameterModifier_AST.getFirstChild() : parameterModifier_AST;
			currentAST.advanceChildToEnd();
		}
		parameterModifier_AST = (AST)currentAST.root;
		returnAST = parameterModifier_AST;
	}
	
	public final void forInit() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST forInit_AST = null;
		
		{
		boolean synPredMatched139 = false;
		if (((_tokenSet_14.member(LA(1))) && (_tokenSet_15.member(LA(2))))) {
			int _m139 = mark();
			synPredMatched139 = true;
			inputState.guessing++;
			try {
				{
				declaration();
				}
			}
			catch (RecognitionException pe) {
				synPredMatched139 = false;
			}
			rewind(_m139);
			inputState.guessing--;
		}
		if ( synPredMatched139 ) {
			declaration();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_13.member(LA(1))) && (_tokenSet_33.member(LA(2)))) {
			expressionList();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((LA(1)==SEMI)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			forInit_AST = (AST)currentAST.root;
			forInit_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(FOR_INIT,"FOR_INIT")).add(forInit_AST));
			currentAST.root = forInit_AST;
			currentAST.child = forInit_AST!=null &&forInit_AST.getFirstChild()!=null ?
				forInit_AST.getFirstChild() : forInit_AST;
			currentAST.advanceChildToEnd();
		}
		forInit_AST = (AST)currentAST.root;
		returnAST = forInit_AST;
	}
	
	public final void forCond() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST forCond_AST = null;
		
		{
		if ((_tokenSet_13.member(LA(1)))) {
			expression();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((LA(1)==SEMI)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			forCond_AST = (AST)currentAST.root;
			forCond_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(FOR_CONDITION,"FOR_CONDITION")).add(forCond_AST));
			currentAST.root = forCond_AST;
			currentAST.child = forCond_AST!=null &&forCond_AST.getFirstChild()!=null ?
				forCond_AST.getFirstChild() : forCond_AST;
			currentAST.advanceChildToEnd();
		}
		forCond_AST = (AST)currentAST.root;
		returnAST = forCond_AST;
	}
	
	public final void forIter() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST forIter_AST = null;
		
		{
		if ((_tokenSet_13.member(LA(1)))) {
			expressionList();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((LA(1)==RPAREN)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			forIter_AST = (AST)currentAST.root;
			forIter_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(FOR_ITERATOR,"FOR_ITERATOR")).add(forIter_AST));
			currentAST.root = forIter_AST;
			currentAST.child = forIter_AST!=null &&forIter_AST.getFirstChild()!=null ?
				forIter_AST.getFirstChild() : forIter_AST;
			currentAST.advanceChildToEnd();
		}
		forIter_AST = (AST)currentAST.root;
		returnAST = forIter_AST;
	}
	
	public final void casesGroup() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST casesGroup_AST = null;
		
		{
		int _cnt130=0;
		_loop130:
		do {
			if ((LA(1)==LITERAL_case||LA(1)==LITERAL_default) && (_tokenSet_34.member(LA(2)))) {
				aCase();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				if ( _cnt130>=1 ) { break _loop130; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt130++;
		} while (true);
		}
		caseSList();
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			casesGroup_AST = (AST)currentAST.root;
			casesGroup_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(CASE_GROUP,"CASE_GROUP")).add(casesGroup_AST));
			currentAST.root = casesGroup_AST;
			currentAST.child = casesGroup_AST!=null &&casesGroup_AST.getFirstChild()!=null ?
				casesGroup_AST.getFirstChild() : casesGroup_AST;
			currentAST.advanceChildToEnd();
		}
		casesGroup_AST = (AST)currentAST.root;
		returnAST = casesGroup_AST;
	}
	
	public final void tryBlock() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST tryBlock_AST = null;
		
		AST tmp155_AST = null;
		tmp155_AST = astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp155_AST);
		match(LITERAL_try);
		compoundStatement();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop146:
		do {
			if ((LA(1)==LITERAL_catch)) {
				handler();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop146;
			}
			
		} while (true);
		}
		{
		if ((LA(1)==LITERAL_finally)) {
			finallyClause();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_11.member(LA(1)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		tryBlock_AST = (AST)currentAST.root;
		returnAST = tryBlock_AST;
	}
	
	public final void aCase() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST aCase_AST = null;
		
		{
		if ((LA(1)==LITERAL_case)) {
			AST tmp156_AST = null;
			tmp156_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp156_AST);
			match(LITERAL_case);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((LA(1)==LITERAL_default)) {
			AST tmp157_AST = null;
			tmp157_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp157_AST);
			match(LITERAL_default);
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		match(COLON);
		aCase_AST = (AST)currentAST.root;
		returnAST = aCase_AST;
	}
	
	public final void caseSList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST caseSList_AST = null;
		
		{
		_loop135:
		do {
			if ((_tokenSet_8.member(LA(1)))) {
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop135;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			caseSList_AST = (AST)currentAST.root;
			caseSList_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(SLIST,"SLIST")).add(caseSList_AST));
			currentAST.root = caseSList_AST;
			currentAST.child = caseSList_AST!=null &&caseSList_AST.getFirstChild()!=null ?
				caseSList_AST.getFirstChild() : caseSList_AST;
			currentAST.advanceChildToEnd();
		}
		caseSList_AST = (AST)currentAST.root;
		returnAST = caseSList_AST;
	}
	
	public final void expressionList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST expressionList_AST = null;
		
		expression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop153:
		do {
			if ((LA(1)==COMMA)) {
				match(COMMA);
				expression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop153;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			expressionList_AST = (AST)currentAST.root;
			expressionList_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(ELIST,"ELIST")).add(expressionList_AST));
			currentAST.root = expressionList_AST;
			currentAST.child = expressionList_AST!=null &&expressionList_AST.getFirstChild()!=null ?
				expressionList_AST.getFirstChild() : expressionList_AST;
			currentAST.advanceChildToEnd();
		}
		expressionList_AST = (AST)currentAST.root;
		returnAST = expressionList_AST;
	}
	
	public final void handler() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST handler_AST = null;
		
		AST tmp160_AST = null;
		tmp160_AST = astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp160_AST);
		match(LITERAL_catch);
		match(LPAREN);
		parameterDeclaration();
		astFactory.addASTChild(currentAST, returnAST);
		match(RPAREN);
		compoundStatement();
		astFactory.addASTChild(currentAST, returnAST);
		handler_AST = (AST)currentAST.root;
		returnAST = handler_AST;
	}
	
	public final void finallyClause() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST finallyClause_AST = null;
		
		AST tmp163_AST = null;
		tmp163_AST = astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp163_AST);
		match(LITERAL_finally);
		compoundStatement();
		astFactory.addASTChild(currentAST, returnAST);
		finallyClause_AST = (AST)currentAST.root;
		returnAST = finallyClause_AST;
	}
	
	public final void conditionalExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST conditionalExpression_AST = null;
		
		logicalOrExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		if ((LA(1)==QUESTION)) {
			AST tmp164_AST = null;
			tmp164_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp164_AST);
			match(QUESTION);
			assignmentExpression();
			astFactory.addASTChild(currentAST, returnAST);
			match(COLON);
			conditionalExpression();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_35.member(LA(1)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		conditionalExpression_AST = (AST)currentAST.root;
		returnAST = conditionalExpression_AST;
	}
	
	public final void logicalOrExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST logicalOrExpression_AST = null;
		
		logicalAndExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop161:
		do {
			if ((LA(1)==LOR)) {
				AST tmp166_AST = null;
				tmp166_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp166_AST);
				match(LOR);
				logicalAndExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop161;
			}
			
		} while (true);
		}
		logicalOrExpression_AST = (AST)currentAST.root;
		returnAST = logicalOrExpression_AST;
	}
	
	public final void logicalAndExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST logicalAndExpression_AST = null;
		
		inclusiveOrExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop164:
		do {
			if ((LA(1)==LAND)) {
				AST tmp167_AST = null;
				tmp167_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp167_AST);
				match(LAND);
				inclusiveOrExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop164;
			}
			
		} while (true);
		}
		logicalAndExpression_AST = (AST)currentAST.root;
		returnAST = logicalAndExpression_AST;
	}
	
	public final void inclusiveOrExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST inclusiveOrExpression_AST = null;
		
		exclusiveOrExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop167:
		do {
			if ((LA(1)==BOR)) {
				AST tmp168_AST = null;
				tmp168_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp168_AST);
				match(BOR);
				exclusiveOrExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop167;
			}
			
		} while (true);
		}
		inclusiveOrExpression_AST = (AST)currentAST.root;
		returnAST = inclusiveOrExpression_AST;
	}
	
	public final void exclusiveOrExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST exclusiveOrExpression_AST = null;
		
		andExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop170:
		do {
			if ((LA(1)==BXOR)) {
				AST tmp169_AST = null;
				tmp169_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp169_AST);
				match(BXOR);
				andExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop170;
			}
			
		} while (true);
		}
		exclusiveOrExpression_AST = (AST)currentAST.root;
		returnAST = exclusiveOrExpression_AST;
	}
	
	public final void andExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST andExpression_AST = null;
		
		equalityExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop173:
		do {
			if ((LA(1)==BAND)) {
				AST tmp170_AST = null;
				tmp170_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp170_AST);
				match(BAND);
				equalityExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop173;
			}
			
		} while (true);
		}
		andExpression_AST = (AST)currentAST.root;
		returnAST = andExpression_AST;
	}
	
	public final void equalityExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST equalityExpression_AST = null;
		
		relationalExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop177:
		do {
			if ((LA(1)==NOT_EQUAL||LA(1)==EQUAL)) {
				{
				if ((LA(1)==NOT_EQUAL)) {
					AST tmp171_AST = null;
					tmp171_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp171_AST);
					match(NOT_EQUAL);
				}
				else if ((LA(1)==EQUAL)) {
					AST tmp172_AST = null;
					tmp172_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp172_AST);
					match(EQUAL);
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
				relationalExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop177;
			}
			
		} while (true);
		}
		equalityExpression_AST = (AST)currentAST.root;
		returnAST = equalityExpression_AST;
	}
	
	public final void relationalExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST relationalExpression_AST = null;
		
		shiftExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		if ((_tokenSet_36.member(LA(1)))) {
			{
			_loop182:
			do {
				if (((LA(1) >= LT && LA(1) <= GE))) {
					{
					if ((LA(1)==LT)) {
						AST tmp173_AST = null;
						tmp173_AST = astFactory.create(LT(1));
						astFactory.makeASTRoot(currentAST, tmp173_AST);
						match(LT);
					}
					else if ((LA(1)==GT)) {
						AST tmp174_AST = null;
						tmp174_AST = astFactory.create(LT(1));
						astFactory.makeASTRoot(currentAST, tmp174_AST);
						match(GT);
					}
					else if ((LA(1)==LE)) {
						AST tmp175_AST = null;
						tmp175_AST = astFactory.create(LT(1));
						astFactory.makeASTRoot(currentAST, tmp175_AST);
						match(LE);
					}
					else if ((LA(1)==GE)) {
						AST tmp176_AST = null;
						tmp176_AST = astFactory.create(LT(1));
						astFactory.makeASTRoot(currentAST, tmp176_AST);
						match(GE);
					}
					else {
						throw new NoViableAltException(LT(1), getFilename());
					}
					
					}
					shiftExpression();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop182;
				}
				
			} while (true);
			}
		}
		else if ((LA(1)==LITERAL_instanceof)) {
			AST tmp177_AST = null;
			tmp177_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp177_AST);
			match(LITERAL_instanceof);
			typeSpec(true);
			astFactory.addASTChild(currentAST, returnAST);
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		relationalExpression_AST = (AST)currentAST.root;
		returnAST = relationalExpression_AST;
	}
	
	public final void shiftExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST shiftExpression_AST = null;
		
		additiveExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop186:
		do {
			if (((LA(1) >= SL && LA(1) <= BSR))) {
				{
				if ((LA(1)==SL)) {
					AST tmp178_AST = null;
					tmp178_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp178_AST);
					match(SL);
				}
				else if ((LA(1)==SR)) {
					AST tmp179_AST = null;
					tmp179_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp179_AST);
					match(SR);
				}
				else if ((LA(1)==BSR)) {
					AST tmp180_AST = null;
					tmp180_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp180_AST);
					match(BSR);
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
				additiveExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop186;
			}
			
		} while (true);
		}
		shiftExpression_AST = (AST)currentAST.root;
		returnAST = shiftExpression_AST;
	}
	
	public final void additiveExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST additiveExpression_AST = null;
		
		multiplicativeExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop190:
		do {
			if ((LA(1)==PLUS||LA(1)==MINUS)) {
				{
				if ((LA(1)==PLUS)) {
					AST tmp181_AST = null;
					tmp181_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp181_AST);
					match(PLUS);
				}
				else if ((LA(1)==MINUS)) {
					AST tmp182_AST = null;
					tmp182_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp182_AST);
					match(MINUS);
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
				multiplicativeExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop190;
			}
			
		} while (true);
		}
		additiveExpression_AST = (AST)currentAST.root;
		returnAST = additiveExpression_AST;
	}
	
	public final void multiplicativeExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST multiplicativeExpression_AST = null;
		
		unaryExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop194:
		do {
			if ((LA(1)==STAR||LA(1)==DIV||LA(1)==MOD)) {
				{
				if ((LA(1)==STAR)) {
					AST tmp183_AST = null;
					tmp183_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp183_AST);
					match(STAR);
				}
				else if ((LA(1)==DIV)) {
					AST tmp184_AST = null;
					tmp184_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp184_AST);
					match(DIV);
				}
				else if ((LA(1)==MOD)) {
					AST tmp185_AST = null;
					tmp185_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp185_AST);
					match(MOD);
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
				unaryExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop194;
			}
			
		} while (true);
		}
		multiplicativeExpression_AST = (AST)currentAST.root;
		returnAST = multiplicativeExpression_AST;
	}
	
	public final void unaryExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST unaryExpression_AST = null;
		
		if ((LA(1)==INC)) {
			AST tmp186_AST = null;
			tmp186_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp186_AST);
			match(INC);
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==DEC)) {
			AST tmp187_AST = null;
			tmp187_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp187_AST);
			match(DEC);
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==MINUS)) {
			AST tmp188_AST = null;
			tmp188_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp188_AST);
			match(MINUS);
			if ( inputState.guessing==0 ) {
				tmp188_AST.setType(UNARY_MINUS);
			}
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpression_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==PLUS)) {
			AST tmp189_AST = null;
			tmp189_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp189_AST);
			match(PLUS);
			if ( inputState.guessing==0 ) {
				tmp189_AST.setType(UNARY_PLUS);
			}
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpression_AST = (AST)currentAST.root;
		}
		else if ((_tokenSet_37.member(LA(1)))) {
			unaryExpressionNotPlusMinus();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpression_AST = (AST)currentAST.root;
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = unaryExpression_AST;
	}
	
	public final void unaryExpressionNotPlusMinus() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST unaryExpressionNotPlusMinus_AST = null;
		Token  lpb = null;
		AST lpb_AST = null;
		Token  lp = null;
		AST lp_AST = null;
		
		if ((LA(1)==BNOT)) {
			AST tmp190_AST = null;
			tmp190_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp190_AST);
			match(BNOT);
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpressionNotPlusMinus_AST = (AST)currentAST.root;
		}
		else if ((LA(1)==LNOT)) {
			AST tmp191_AST = null;
			tmp191_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp191_AST);
			match(LNOT);
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpressionNotPlusMinus_AST = (AST)currentAST.root;
		}
		else if ((_tokenSet_38.member(LA(1)))) {
			{
			if ((LA(1)==LPAREN) && (_tokenSet_21.member(LA(2)))) {
				lpb = LT(1);
				lpb_AST = astFactory.create(lpb);
				astFactory.makeASTRoot(currentAST, lpb_AST);
				match(LPAREN);
				if ( inputState.guessing==0 ) {
					lpb_AST.setType(TYPECAST);
				}
				builtInTypeSpec(true);
				astFactory.addASTChild(currentAST, returnAST);
				match(RPAREN);
				unaryExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				boolean synPredMatched199 = false;
				if (((LA(1)==LPAREN) && (LA(2)==IDENT))) {
					int _m199 = mark();
					synPredMatched199 = true;
					inputState.guessing++;
					try {
						{
						match(LPAREN);
						classTypeSpec(true);
						match(RPAREN);
						unaryExpressionNotPlusMinus();
						}
					}
					catch (RecognitionException pe) {
						synPredMatched199 = false;
					}
					rewind(_m199);
					inputState.guessing--;
				}
				if ( synPredMatched199 ) {
					lp = LT(1);
					lp_AST = astFactory.create(lp);
					astFactory.makeASTRoot(currentAST, lp_AST);
					match(LPAREN);
					if ( inputState.guessing==0 ) {
						lp_AST.setType(TYPECAST);
					}
					classTypeSpec(true);
					astFactory.addASTChild(currentAST, returnAST);
					match(RPAREN);
					unaryExpressionNotPlusMinus();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else if ((_tokenSet_38.member(LA(1))) && (_tokenSet_39.member(LA(2)))) {
					postfixExpression();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				unaryExpressionNotPlusMinus_AST = (AST)currentAST.root;
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			returnAST = unaryExpressionNotPlusMinus_AST;
		}
		
	public final void postfixExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST postfixExpression_AST = null;
		Token  lp = null;
		AST lp_AST = null;
		Token  lp3 = null;
		AST lp3_AST = null;
		Token  lps = null;
		AST lps_AST = null;
		Token  lb = null;
		AST lb_AST = null;
		Token  in = null;
		AST in_AST = null;
		Token  de = null;
		AST de_AST = null;
		
		primaryExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop205:
		do {
			if ((LA(1)==DOT) && (LA(2)==IDENT)) {
				AST tmp194_AST = null;
				tmp194_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp194_AST);
				match(DOT);
				AST tmp195_AST = null;
				tmp195_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp195_AST);
				match(IDENT);
				{
				if ((LA(1)==LPAREN)) {
					lp = LT(1);
					lp_AST = astFactory.create(lp);
					astFactory.makeASTRoot(currentAST, lp_AST);
					match(LPAREN);
					if ( inputState.guessing==0 ) {
						lp_AST.setType(METHOD_CALL);
					}
					argList();
					astFactory.addASTChild(currentAST, returnAST);
					match(RPAREN);
				}
				else if ((_tokenSet_19.member(LA(1)))) {
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
			}
			else if ((LA(1)==DOT) && (LA(2)==LITERAL_this)) {
				AST tmp197_AST = null;
				tmp197_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp197_AST);
				match(DOT);
				AST tmp198_AST = null;
				tmp198_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp198_AST);
				match(LITERAL_this);
			}
			else if ((LA(1)==DOT) && (LA(2)==LITERAL_super)) {
				AST tmp199_AST = null;
				tmp199_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp199_AST);
				match(DOT);
				AST tmp200_AST = null;
				tmp200_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp200_AST);
				match(LITERAL_super);
				{
				if ((LA(1)==LPAREN)) {
					lp3 = LT(1);
					lp3_AST = astFactory.create(lp3);
					astFactory.makeASTRoot(currentAST, lp3_AST);
					match(LPAREN);
					argList();
					astFactory.addASTChild(currentAST, returnAST);
					match(RPAREN);
					if ( inputState.guessing==0 ) {
						lp3_AST.setType(SUPER_CTOR_CALL);
					}
				}
				else if ((LA(1)==DOT)) {
					AST tmp202_AST = null;
					tmp202_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp202_AST);
					match(DOT);
					AST tmp203_AST = null;
					tmp203_AST = astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp203_AST);
					match(IDENT);
					{
					if ((LA(1)==LPAREN)) {
						lps = LT(1);
						lps_AST = astFactory.create(lps);
						astFactory.makeASTRoot(currentAST, lps_AST);
						match(LPAREN);
						if ( inputState.guessing==0 ) {
							lps_AST.setType(METHOD_CALL);
						}
						argList();
						astFactory.addASTChild(currentAST, returnAST);
						match(RPAREN);
					}
					else if ((_tokenSet_19.member(LA(1)))) {
					}
					else {
						throw new NoViableAltException(LT(1), getFilename());
					}
					
					}
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
			}
			else if ((LA(1)==DOT) && (LA(2)==LITERAL_new)) {
				AST tmp205_AST = null;
				tmp205_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp205_AST);
				match(DOT);
				newExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((LA(1)==LBRACK)) {
				lb = LT(1);
				lb_AST = astFactory.create(lb);
				astFactory.makeASTRoot(currentAST, lb_AST);
				match(LBRACK);
				if ( inputState.guessing==0 ) {
					lb_AST.setType(INDEX_OP);
				}
				expression();
				astFactory.addASTChild(currentAST, returnAST);
				match(RBRACK);
			}
			else {
				break _loop205;
			}
			
		} while (true);
		}
		{
		if ((LA(1)==INC)) {
			in = LT(1);
			in_AST = astFactory.create(in);
			astFactory.makeASTRoot(currentAST, in_AST);
			match(INC);
			if ( inputState.guessing==0 ) {
				in_AST.setType(POST_INC);
			}
		}
		else if ((LA(1)==DEC)) {
			de = LT(1);
			de_AST = astFactory.create(de);
			astFactory.makeASTRoot(currentAST, de_AST);
			match(DEC);
			if ( inputState.guessing==0 ) {
				de_AST.setType(POST_DEC);
			}
		}
		else if ((_tokenSet_40.member(LA(1)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		postfixExpression_AST = (AST)currentAST.root;
		returnAST = postfixExpression_AST;
	}
	
	public final void newArrayDeclarator() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST newArrayDeclarator_AST = null;
		Token  lb = null;
		AST lb_AST = null;
		
		{
		int _cnt223=0;
		_loop223:
		do {
			if ((LA(1)==LBRACK) && (_tokenSet_41.member(LA(2)))) {
				lb = LT(1);
				lb_AST = astFactory.create(lb);
				astFactory.makeASTRoot(currentAST, lb_AST);
				match(LBRACK);
				if ( inputState.guessing==0 ) {
					lb_AST.setType(ARRAY_DECLARATOR);
				}
				{
				if ((_tokenSet_13.member(LA(1)))) {
					expression();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else if ((LA(1)==RBRACK)) {
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
				match(RBRACK);
			}
			else {
				if ( _cnt223>=1 ) { break _loop223; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt223++;
		} while (true);
		}
		newArrayDeclarator_AST = (AST)currentAST.root;
		returnAST = newArrayDeclarator_AST;
	}
	
	
	public static final String[] _tokenNames = {
		"<0>",
		"EOF",
		"<2>",
		"NULL_TREE_LOOKAHEAD",
		"BLOCK",
		"MODIFIERS",
		"OBJBLOCK",
		"SLIST",
		"CTOR_DEF",
		"METHOD_DEF",
		"VARIABLE_DEF",
		"INSTANCE_INIT",
		"STATIC_INIT",
		"TYPE",
		"CLASS_DEF",
		"INTERFACE_DEF",
		"PACKAGE_DEF",
		"ARRAY_DECLARATOR",
		"EXTENDS_CLAUSE",
		"IMPLEMENTS_CLAUSE",
		"PARAMETERS",
		"PARAMETER_DEF",
		"LABELED_STAT",
		"TYPECAST",
		"INDEX_OP",
		"POST_INC",
		"POST_DEC",
		"METHOD_CALL",
		"EXPR",
		"ARRAY_INIT",
		"IMPORT",
		"UNARY_MINUS",
		"UNARY_PLUS",
		"CASE_GROUP",
		"ELIST",
		"FOR_INIT",
		"FOR_CONDITION",
		"FOR_ITERATOR",
		"EMPTY_STAT",
		"\"final\"",
		"\"abstract\"",
		"\"strictfp\"",
		"SUPER_CTOR_CALL",
		"CTOR_CALL",
		"\"package\"",
		"SEMI",
		"\"import\"",
		"LBRACK",
		"RBRACK",
		"\"void\"",
		"\"boolean\"",
		"\"byte\"",
		"\"char\"",
		"\"short\"",
		"\"int\"",
		"\"float\"",
		"\"long\"",
		"\"double\"",
		"IDENT",
		"DOT",
		"STAR",
		"\"private\"",
		"\"public\"",
		"\"protected\"",
		"\"static\"",
		"\"transient\"",
		"\"native\"",
		"\"threadsafe\"",
		"\"synchronized\"",
		"\"volatile\"",
		"\"class\"",
		"\"extends\"",
		"\"interface\"",
		"LCURLY",
		"RCURLY",
		"COMMA",
		"\"implements\"",
		"LPAREN",
		"RPAREN",
		"\"this\"",
		"\"super\"",
		"ASSIGN",
		"\"throws\"",
		"COLON",
		"\"if\"",
		"\"else\"",
		"\"for\"",
		"\"while\"",
		"\"do\"",
		"\"break\"",
		"\"continue\"",
		"\"return\"",
		"\"switch\"",
		"\"throw\"",
		"\"assert\"",
		"\"case\"",
		"\"default\"",
		"\"try\"",
		"\"finally\"",
		"\"catch\"",
		"PLUS_ASSIGN",
		"MINUS_ASSIGN",
		"STAR_ASSIGN",
		"DIV_ASSIGN",
		"MOD_ASSIGN",
		"SR_ASSIGN",
		"BSR_ASSIGN",
		"SL_ASSIGN",
		"BAND_ASSIGN",
		"BXOR_ASSIGN",
		"BOR_ASSIGN",
		"QUESTION",
		"LOR",
		"LAND",
		"BOR",
		"BXOR",
		"BAND",
		"NOT_EQUAL",
		"EQUAL",
		"LT",
		"GT",
		"LE",
		"GE",
		"\"instanceof\"",
		"SL",
		"SR",
		"BSR",
		"PLUS",
		"MINUS",
		"DIV",
		"MOD",
		"INC",
		"DEC",
		"BNOT",
		"LNOT",
		"\"true\"",
		"\"false\"",
		"\"null\"",
		"\"new\"",
		"NUM_INT",
		"CHAR_LITERAL",
		"STRING_LITERAL",
		"NUM_FLOAT",
		"NUM_LONG",
		"NUM_DOUBLE",
		"WS",
		"SL_COMMENT",
		"ML_COMMENT",
		"ESC",
		"HEX_DIGIT",
		"VOCAB",
		"EXPONENT",
		"FLOAT_SUFFIX",
		"CONSTRUCTOR_CAST",
		"EMPTY_FIELD",
		"WEBCOLOR_LITERAL",
		"\"color\""
	};
	
	protected void buildTokenTypeASTClassMap() {
		tokenTypeToASTClassMap=null;
	};
	
	private static final long[] mk_tokenSet_0() {
		long[] data = { -2305716015620685822L, 383L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_0 = new BitSet(mk_tokenSet_0());
	private static final long[] mk_tokenSet_1() {
		long[] data = { -2017573600399196158L, 383L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_1 = new BitSet(mk_tokenSet_1());
	private static final long[] mk_tokenSet_2() {
		long[] data = { -1729906174200905728L, 895L, 268435456L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_2 = new BitSet(mk_tokenSet_2());
	private static final long[] mk_tokenSet_3() {
		long[] data = { -1153304684409126910L, -9223372026120394881L, 402784249L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_3 = new BitSet(mk_tokenSet_3());
	private static final long[] mk_tokenSet_4() {
		long[] data = { -1153304684409126912L, -9223372026120394881L, 402784249L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_4 = new BitSet(mk_tokenSet_4());
	private static final long[] mk_tokenSet_5() {
		long[] data = { -1729906174200905726L, -9223372026120396161L, 402784249L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_5 = new BitSet(mk_tokenSet_5());
	private static final long[] mk_tokenSet_6() {
		long[] data = { -383179802279934L, -57984440705L, 402784255L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_6 = new BitSet(mk_tokenSet_6());
	private static final long[] mk_tokenSet_7() {
		long[] data = { -1729941358572994560L, 895L, 268435456L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_7 = new BitSet(mk_tokenSet_7());
	private static final long[] mk_tokenSet_8() {
		long[] data = { -1729906174200905728L, -9223372026120396161L, 402784249L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_8 = new BitSet(mk_tokenSet_8());
	private static final long[] mk_tokenSet_9() {
		long[] data = { -2305733607806730238L, 383L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_9 = new BitSet(mk_tokenSet_9());
	private static final long[] mk_tokenSet_10() {
		long[] data = { -2305803976550907904L, 383L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_10 = new BitSet(mk_tokenSet_10());
	private static final long[] mk_tokenSet_11() {
		long[] data = { -1729906174200905726L, -9223372019675847041L, 402784249L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_11 = new BitSet(mk_tokenSet_11());
	private static final long[] mk_tokenSet_12() {
		long[] data = { -383179802279934L, -284801L, 402784255L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_12 = new BitSet(mk_tokenSet_12());
	private static final long[] mk_tokenSet_13() {
		long[] data = { 575897802350002176L, -9223372036854669312L, 402784249L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_13 = new BitSet(mk_tokenSet_13());
	private static final long[] mk_tokenSet_14() {
		long[] data = { -1729941358572994560L, 63L, 268435456L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_14 = new BitSet(mk_tokenSet_14());
	private static final long[] mk_tokenSet_15() {
		long[] data = { -1153339868781215744L, 63L, 268435456L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_15 = new BitSet(mk_tokenSet_15());
	private static final long[] mk_tokenSet_16() {
		long[] data = { 2305455981120716800L, -68719239168L, 402784255L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_16 = new BitSet(mk_tokenSet_16());
	private static final long[] mk_tokenSet_17() {
		long[] data = { -2305839160922996736L, 127L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_17 = new BitSet(mk_tokenSet_17());
	private static final long[] mk_tokenSet_18() {
		long[] data = { -2017608784771284992L, 127L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_18 = new BitSet(mk_tokenSet_18());
	private static final long[] mk_tokenSet_19() {
		long[] data = { 2018070029899137024L, -68718801920L, 31L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_19 = new BitSet(mk_tokenSet_19());
	private static final long[] mk_tokenSet_20() {
		long[] data = { -101704825569278L, -51539873921L, 402784255L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_20 = new BitSet(mk_tokenSet_20());
	private static final long[] mk_tokenSet_21() {
		long[] data = { 287667426198290432L, 0L, 268435456L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_21 = new BitSet(mk_tokenSet_21());
	private static final long[] mk_tokenSet_22() {
		long[] data = { 0L, 140668769009664L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_22 = new BitSet(mk_tokenSet_22());
	private static final long[] mk_tokenSet_23() {
		long[] data = { -1729941358572994560L, 383L, 268435456L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_23 = new BitSet(mk_tokenSet_23());
	private static final long[] mk_tokenSet_24() {
		long[] data = { -1153339868781215744L, 8575L, 268435456L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_24 = new BitSet(mk_tokenSet_24());
	private static final long[] mk_tokenSet_25() {
		long[] data = { 575897802350002176L, 0L, 268435456L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_25 = new BitSet(mk_tokenSet_25());
	private static final long[] mk_tokenSet_26() {
		long[] data = { -2305839160922996736L, 383L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_26 = new BitSet(mk_tokenSet_26());
	private static final long[] mk_tokenSet_27() {
		long[] data = { -2305803976550907902L, 383L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_27 = new BitSet(mk_tokenSet_27());
	private static final long[] mk_tokenSet_28() {
		long[] data = { -2305839160922996736L, 63L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_28 = new BitSet(mk_tokenSet_28());
	private static final long[] mk_tokenSet_29() {
		long[] data = { -1729906174200905728L, -9223372026120395137L, 402784249L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_29 = new BitSet(mk_tokenSet_29());
	private static final long[] mk_tokenSet_30() {
		long[] data = { -383179802279934L, -57984440449L, 402784255L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_30 = new BitSet(mk_tokenSet_30());
	private static final long[] mk_tokenSet_31() {
		long[] data = { 575898352105816064L, 0L, 268435456L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_31 = new BitSet(mk_tokenSet_31());
	private static final long[] mk_tokenSet_32() {
		long[] data = { 575897802350002176L, -9223372036854668800L, 402784249L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_32 = new BitSet(mk_tokenSet_32());
	private static final long[] mk_tokenSet_33() {
		long[] data = { 2305455981120716800L, -68719237120L, 402784255L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_33 = new BitSet(mk_tokenSet_33());
	private static final long[] mk_tokenSet_34() {
		long[] data = { 575897802350002176L, -9223372036854145024L, 402784249L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_34 = new BitSet(mk_tokenSet_34());
	private static final long[] mk_tokenSet_35() {
		long[] data = { 288547035500511232L, 140668769553408L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_35 = new BitSet(mk_tokenSet_35());
	private static final long[] mk_tokenSet_36() {
		long[] data = { 288547035500511232L, 576460683584621568L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_36 = new BitSet(mk_tokenSet_36());
	private static final long[] mk_tokenSet_37() {
		long[] data = { 575897802350002176L, 106496L, 402784224L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_37 = new BitSet(mk_tokenSet_37());
	private static final long[] mk_tokenSet_38() {
		long[] data = { 575897802350002176L, 106496L, 402784128L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_38 = new BitSet(mk_tokenSet_38());
	private static final long[] mk_tokenSet_39() {
		long[] data = { 2305737456097427456L, -68718695424L, 402784255L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_39 = new BitSet(mk_tokenSet_39());
	private static final long[] mk_tokenSet_40() {
		long[] data = { 1441468540107358208L, -68718801920L, 7L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_40 = new BitSet(mk_tokenSet_40());
	private static final long[] mk_tokenSet_41() {
		long[] data = { 576179277326712832L, -9223372036854669312L, 402784249L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_41 = new BitSet(mk_tokenSet_41());
	
	}
