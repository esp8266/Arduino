// $ANTLR 2.7.2: "WTreeParser.g" -> "WTreeParser.java"$

package processing.app.preproc;

import java.io.*;

import antlr.CommonAST;
import antlr.DumpASTVisitor;

import antlr.TreeParser;
import antlr.Token;
import antlr.collections.AST;
import antlr.RecognitionException;
import antlr.ANTLRException;
import antlr.NoViableAltException;
import antlr.MismatchedTokenException;
import antlr.SemanticException;
import antlr.collections.impl.BitSet;
import antlr.ASTPair;
import antlr.collections.impl.ASTArray;


public class WTreeParser extends antlr.TreeParser       implements WTreeParserTokenTypes
 {

        int traceDepth = 0;
        public void reportError(RecognitionException ex) {
          if ( ex != null)   {
                System.err.println("ANTLR Tree Parsing RecognitionException Error: " + ex.getClass().getName() + " " + ex );
                ex.printStackTrace(System.err);
          }
        }
        public void reportError(NoViableAltException ex) {
                System.err.println("ANTLR Tree Parsing NoViableAltException Error: " + ex.toString());
                TNode.printTree( ex.node );
                ex.printStackTrace(System.err);
        }
        public void reportError(MismatchedTokenException ex) {
          if ( ex != null)   {
                TNode.printTree( ex.node );
                System.err.println("ANTLR Tree Parsing MismatchedTokenException Error: " + ex );
                ex.printStackTrace(System.err);
          }
        }
        public void reportError(String s) {
                System.err.println("ANTLR Error from String: " + s);
        }
        public void reportWarning(String s) {
                System.err.println("ANTLR Warning from String: " + s);
        }
        protected void match(AST t, int ttype) throws MismatchedTokenException {
                //System.out.println("match("+ttype+"); cursor is "+t);
                super.match(t, ttype);
        }
        public void match(AST t, BitSet b) throws MismatchedTokenException {
                //System.out.println("match("+b+"); cursor is "+t);
                super.match(t, b);
        }
        protected void matchNot(AST t, int ttype) throws MismatchedTokenException {
                //System.out.println("matchNot("+ttype+"); cursor is "+t);
                super.matchNot(t, ttype);
                }
        public void traceIn(String rname, AST t) {
          traceDepth += 1;
          for (int x=0; x<traceDepth; x++) System.out.print(" ");
          super.traceIn(rname, t);   
        }
        public void traceOut(String rname, AST t) {
          for (int x=0; x<traceDepth; x++) System.out.print(" ");
          super.traceOut(rname, t);
          traceDepth -= 1;
        }


public WTreeParser() {
	tokenNames = _tokenNames;
}

	public final void translationUnit(AST _t) throws RecognitionException {
		
		TNode translationUnit_AST_in = (TNode)_t;
		
		{
		if (_t==null) _t=ASTNULL;
		switch ( _t.getType()) {
		case LITERAL_asm:
		case SEMI:
		case NDeclaration:
		case NFunctionDef:
		case NTypeMissing:
		{
			externalList(_t);
			_t = _retTree;
			break;
		}
		case 3:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(_t);
		}
		}
		}
		_retTree = _t;
	}
	
	public final void externalList(AST _t) throws RecognitionException {
		
		TNode externalList_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt5=0;
			_loop5:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_tokenSet_0.member(_t.getType()))) {
					externalDef(_t);
					_t = _retTree;
				}
				else {
					if ( _cnt5>=1 ) { break _loop5; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt5++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void externalDef(AST _t) throws RecognitionException {
		
		TNode externalDef_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case NDeclaration:
			{
				declaration(_t);
				_t = _retTree;
				break;
			}
			case NFunctionDef:
			{
				functionDef(_t);
				_t = _retTree;
				break;
			}
			case LITERAL_asm:
			{
				asm_expr(_t);
				_t = _retTree;
				break;
			}
			case SEMI:
			{
				TNode tmp1_AST_in = (TNode)_t;
				match(_t,SEMI);
				_t = _t.getNextSibling();
				break;
			}
			case NTypeMissing:
			{
				typelessDeclaration(_t);
				_t = _retTree;
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void declaration(AST _t) throws RecognitionException {
		
		TNode declaration_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t15 = _t;
			TNode tmp2_AST_in = (TNode)_t;
			match(_t,NDeclaration);
			_t = _t.getFirstChild();
			declSpecifiers(_t);
			_t = _retTree;
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case NInitDecl:
			{
				initDeclList(_t);
				_t = _retTree;
				break;
			}
			case SEMI:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			{
			int _cnt18=0;
			_loop18:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==SEMI)) {
					TNode tmp3_AST_in = (TNode)_t;
					match(_t,SEMI);
					_t = _t.getNextSibling();
				}
				else {
					if ( _cnt18>=1 ) { break _loop18; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt18++;
			} while (true);
			}
			_t = __t15;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void functionDef(AST _t) throws RecognitionException {
		
		TNode functionDef_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t125 = _t;
			TNode tmp4_AST_in = (TNode)_t;
			match(_t,NFunctionDef);
			_t = _t.getFirstChild();
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LITERAL_volatile:
			case LITERAL_struct:
			case LITERAL_union:
			case LITERAL_enum:
			case LITERAL_extern:
			case LITERAL_static:
			case LITERAL_const:
			case LITERAL_void:
			case LITERAL_char:
			case LITERAL_short:
			case LITERAL_int:
			case LITERAL_long:
			case LITERAL_float:
			case LITERAL_double:
			case LITERAL_signed:
			case LITERAL_unsigned:
			case NTypedefName:
			case LITERAL_inline:
			case LITERAL_byte:
			case LITERAL_boolean:
			case LITERAL_Servo:
			case LITERAL_Wire:
			case LITERAL_typeof:
			case LITERAL___complex:
			{
				functionDeclSpecifiers(_t);
				_t = _retTree;
				break;
			}
			case NDeclarator:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			declarator(_t);
			_t = _retTree;
			{
			_loop128:
			do {
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case NDeclaration:
				{
					declaration(_t);
					_t = _retTree;
					break;
				}
				case VARARGS:
				{
					TNode tmp5_AST_in = (TNode)_t;
					match(_t,VARARGS);
					_t = _t.getNextSibling();
					break;
				}
				default:
				{
					break _loop128;
				}
				}
			} while (true);
			}
			compoundStatement(_t);
			_t = _retTree;
			_t = __t125;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void asm_expr(AST _t) throws RecognitionException {
		
		TNode asm_expr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t10 = _t;
			TNode tmp6_AST_in = (TNode)_t;
			match(_t,LITERAL_asm);
			_t = _t.getFirstChild();
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LITERAL_volatile:
			{
				TNode tmp7_AST_in = (TNode)_t;
				match(_t,LITERAL_volatile);
				_t = _t.getNextSibling();
				break;
			}
			case LCURLY:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			TNode tmp8_AST_in = (TNode)_t;
			match(_t,LCURLY);
			_t = _t.getNextSibling();
			expr(_t);
			_t = _retTree;
			TNode tmp9_AST_in = (TNode)_t;
			match(_t,RCURLY);
			_t = _t.getNextSibling();
			{
			int _cnt13=0;
			_loop13:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==SEMI)) {
					TNode tmp10_AST_in = (TNode)_t;
					match(_t,SEMI);
					_t = _t.getNextSibling();
				}
				else {
					if ( _cnt13>=1 ) { break _loop13; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt13++;
			} while (true);
			}
			_t = __t10;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void typelessDeclaration(AST _t) throws RecognitionException {
		
		TNode typelessDeclaration_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t8 = _t;
			TNode tmp11_AST_in = (TNode)_t;
			match(_t,NTypeMissing);
			_t = _t.getFirstChild();
			initDeclList(_t);
			_t = _retTree;
			TNode tmp12_AST_in = (TNode)_t;
			match(_t,SEMI);
			_t = _t.getNextSibling();
			_t = __t8;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void initDeclList(AST _t) throws RecognitionException {
		
		TNode initDeclList_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt78=0;
			_loop78:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==NInitDecl)) {
					initDecl(_t);
					_t = _retTree;
				}
				else {
					if ( _cnt78>=1 ) { break _loop78; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt78++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void expr(AST _t) throws RecognitionException {
		
		TNode expr_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case ASSIGN:
			case DIV_ASSIGN:
			case PLUS_ASSIGN:
			case MINUS_ASSIGN:
			case STAR_ASSIGN:
			case MOD_ASSIGN:
			case RSHIFT_ASSIGN:
			case LSHIFT_ASSIGN:
			case BAND_ASSIGN:
			case BOR_ASSIGN:
			case BXOR_ASSIGN:
			{
				assignExpr(_t);
				_t = _retTree;
				break;
			}
			case QUESTION:
			{
				conditionalExpr(_t);
				_t = _retTree;
				break;
			}
			case LOR:
			{
				logicalOrExpr(_t);
				_t = _retTree;
				break;
			}
			case LAND:
			{
				logicalAndExpr(_t);
				_t = _retTree;
				break;
			}
			case BOR:
			{
				inclusiveOrExpr(_t);
				_t = _retTree;
				break;
			}
			case BXOR:
			{
				exclusiveOrExpr(_t);
				_t = _retTree;
				break;
			}
			case BAND:
			{
				bitAndExpr(_t);
				_t = _retTree;
				break;
			}
			case EQUAL:
			case NOT_EQUAL:
			{
				equalityExpr(_t);
				_t = _retTree;
				break;
			}
			case LT:
			case LTE:
			case GT:
			case GTE:
			{
				relationalExpr(_t);
				_t = _retTree;
				break;
			}
			case LSHIFT:
			case RSHIFT:
			{
				shiftExpr(_t);
				_t = _retTree;
				break;
			}
			case PLUS:
			case MINUS:
			{
				additiveExpr(_t);
				_t = _retTree;
				break;
			}
			case STAR:
			case DIV:
			case MOD:
			{
				multExpr(_t);
				_t = _retTree;
				break;
			}
			case NCast:
			{
				castExpr(_t);
				_t = _retTree;
				break;
			}
			case INC:
			case DEC:
			case LITERAL_sizeof:
			case NUnaryExpr:
			case LITERAL___alignof:
			{
				unaryExpr(_t);
				_t = _retTree;
				break;
			}
			case NPostfixExpr:
			{
				postfixExpr(_t);
				_t = _retTree;
				break;
			}
			case ID:
			case CharLiteral:
			case NExpressionGroup:
			case NStringSeq:
			case Number:
			{
				primaryExpr(_t);
				_t = _retTree;
				break;
			}
			case NCommaExpr:
			{
				commaExpr(_t);
				_t = _retTree;
				break;
			}
			case NEmptyExpression:
			{
				emptyExpr(_t);
				_t = _retTree;
				break;
			}
			case LPAREN:
			{
				compoundStatementExpr(_t);
				_t = _retTree;
				break;
			}
			case NInitializer:
			case NLcurlyInitializer:
			{
				initializer(_t);
				_t = _retTree;
				break;
			}
			case NRangeExpr:
			{
				rangeExpr(_t);
				_t = _retTree;
				break;
			}
			case NGnuAsmExpr:
			{
				gnuAsmExpr(_t);
				_t = _retTree;
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void declSpecifiers(AST _t) throws RecognitionException {
		
		TNode declSpecifiers_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt21=0;
			_loop21:
			do {
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case LITERAL_typedef:
				case LITERAL_auto:
				case LITERAL_register:
				case LITERAL_extern:
				case LITERAL_static:
				case LITERAL_inline:
				{
					storageClassSpecifier(_t);
					_t = _retTree;
					break;
				}
				case LITERAL_volatile:
				case LITERAL_const:
				{
					typeQualifier(_t);
					_t = _retTree;
					break;
				}
				case LITERAL_struct:
				case LITERAL_union:
				case LITERAL_enum:
				case LITERAL_void:
				case LITERAL_char:
				case LITERAL_short:
				case LITERAL_int:
				case LITERAL_long:
				case LITERAL_float:
				case LITERAL_double:
				case LITERAL_signed:
				case LITERAL_unsigned:
				case NTypedefName:
				case LITERAL_byte:
				case LITERAL_boolean:
				case LITERAL_Servo:
				case LITERAL_Wire:
				case LITERAL_typeof:
				case LITERAL___complex:
				{
					typeSpecifier(_t);
					_t = _retTree;
					break;
				}
				default:
				{
					if ( _cnt21>=1 ) { break _loop21; } else {throw new NoViableAltException(_t);}
				}
				}
				_cnt21++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void storageClassSpecifier(AST _t) throws RecognitionException {
		
		TNode storageClassSpecifier_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LITERAL_auto:
			{
				TNode tmp13_AST_in = (TNode)_t;
				match(_t,LITERAL_auto);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_register:
			{
				TNode tmp14_AST_in = (TNode)_t;
				match(_t,LITERAL_register);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_typedef:
			{
				TNode tmp15_AST_in = (TNode)_t;
				match(_t,LITERAL_typedef);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_extern:
			case LITERAL_static:
			case LITERAL_inline:
			{
				functionStorageClassSpecifier(_t);
				_t = _retTree;
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void typeQualifier(AST _t) throws RecognitionException {
		
		TNode typeQualifier_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LITERAL_const:
			{
				TNode tmp16_AST_in = (TNode)_t;
				match(_t,LITERAL_const);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_volatile:
			{
				TNode tmp17_AST_in = (TNode)_t;
				match(_t,LITERAL_volatile);
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void typeSpecifier(AST _t) throws RecognitionException {
		
		TNode typeSpecifier_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LITERAL_void:
			{
				TNode tmp18_AST_in = (TNode)_t;
				match(_t,LITERAL_void);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_char:
			{
				TNode tmp19_AST_in = (TNode)_t;
				match(_t,LITERAL_char);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_short:
			{
				TNode tmp20_AST_in = (TNode)_t;
				match(_t,LITERAL_short);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_int:
			{
				TNode tmp21_AST_in = (TNode)_t;
				match(_t,LITERAL_int);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_long:
			{
				TNode tmp22_AST_in = (TNode)_t;
				match(_t,LITERAL_long);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_float:
			{
				TNode tmp23_AST_in = (TNode)_t;
				match(_t,LITERAL_float);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_double:
			{
				TNode tmp24_AST_in = (TNode)_t;
				match(_t,LITERAL_double);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_signed:
			{
				TNode tmp25_AST_in = (TNode)_t;
				match(_t,LITERAL_signed);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_unsigned:
			{
				TNode tmp26_AST_in = (TNode)_t;
				match(_t,LITERAL_unsigned);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_byte:
			{
				TNode tmp27_AST_in = (TNode)_t;
				match(_t,LITERAL_byte);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_boolean:
			{
				TNode tmp28_AST_in = (TNode)_t;
				match(_t,LITERAL_boolean);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_Servo:
			{
				TNode tmp29_AST_in = (TNode)_t;
				match(_t,LITERAL_Servo);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_Wire:
			{
				TNode tmp30_AST_in = (TNode)_t;
				match(_t,LITERAL_Wire);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_struct:
			{
				structSpecifier(_t);
				_t = _retTree;
				{
				_loop27:
				do {
					if (_t==null) _t=ASTNULL;
					if ((_t.getType()==NAsmAttribute||_t.getType()==LITERAL___attribute)) {
						attributeDecl(_t);
						_t = _retTree;
					}
					else {
						break _loop27;
					}
					
				} while (true);
				}
				break;
			}
			case LITERAL_union:
			{
				unionSpecifier(_t);
				_t = _retTree;
				{
				_loop29:
				do {
					if (_t==null) _t=ASTNULL;
					if ((_t.getType()==NAsmAttribute||_t.getType()==LITERAL___attribute)) {
						attributeDecl(_t);
						_t = _retTree;
					}
					else {
						break _loop29;
					}
					
				} while (true);
				}
				break;
			}
			case LITERAL_enum:
			{
				enumSpecifier(_t);
				_t = _retTree;
				break;
			}
			case NTypedefName:
			{
				typedefName(_t);
				_t = _retTree;
				break;
			}
			case LITERAL_typeof:
			{
				AST __t30 = _t;
				TNode tmp31_AST_in = (TNode)_t;
				match(_t,LITERAL_typeof);
				_t = _t.getFirstChild();
				TNode tmp32_AST_in = (TNode)_t;
				match(_t,LPAREN);
				_t = _t.getNextSibling();
				{
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case LITERAL_volatile:
				case LITERAL_struct:
				case LITERAL_union:
				case LITERAL_enum:
				case LITERAL_const:
				case LITERAL_void:
				case LITERAL_char:
				case LITERAL_short:
				case LITERAL_int:
				case LITERAL_long:
				case LITERAL_float:
				case LITERAL_double:
				case LITERAL_signed:
				case LITERAL_unsigned:
				case NTypedefName:
				case LITERAL_byte:
				case LITERAL_boolean:
				case LITERAL_Servo:
				case LITERAL_Wire:
				case LITERAL_typeof:
				case LITERAL___complex:
				{
					typeName(_t);
					_t = _retTree;
					break;
				}
				case ID:
				case ASSIGN:
				case STAR:
				case LPAREN:
				case DIV_ASSIGN:
				case PLUS_ASSIGN:
				case MINUS_ASSIGN:
				case STAR_ASSIGN:
				case MOD_ASSIGN:
				case RSHIFT_ASSIGN:
				case LSHIFT_ASSIGN:
				case BAND_ASSIGN:
				case BOR_ASSIGN:
				case BXOR_ASSIGN:
				case QUESTION:
				case LOR:
				case LAND:
				case BOR:
				case BXOR:
				case BAND:
				case EQUAL:
				case NOT_EQUAL:
				case LT:
				case LTE:
				case GT:
				case GTE:
				case LSHIFT:
				case RSHIFT:
				case PLUS:
				case MINUS:
				case DIV:
				case MOD:
				case INC:
				case DEC:
				case LITERAL_sizeof:
				case CharLiteral:
				case NCast:
				case NExpressionGroup:
				case NInitializer:
				case NEmptyExpression:
				case NCommaExpr:
				case NUnaryExpr:
				case NPostfixExpr:
				case NRangeExpr:
				case NStringSeq:
				case NLcurlyInitializer:
				case NGnuAsmExpr:
				case Number:
				case LITERAL___alignof:
				{
					expr(_t);
					_t = _retTree;
					break;
				}
				default:
				{
					throw new NoViableAltException(_t);
				}
				}
				}
				TNode tmp33_AST_in = (TNode)_t;
				match(_t,RPAREN);
				_t = _t.getNextSibling();
				_t = __t30;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL___complex:
			{
				TNode tmp34_AST_in = (TNode)_t;
				match(_t,LITERAL___complex);
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void functionStorageClassSpecifier(AST _t) throws RecognitionException {
		
		TNode functionStorageClassSpecifier_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LITERAL_extern:
			{
				TNode tmp35_AST_in = (TNode)_t;
				match(_t,LITERAL_extern);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_static:
			{
				TNode tmp36_AST_in = (TNode)_t;
				match(_t,LITERAL_static);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_inline:
			{
				TNode tmp37_AST_in = (TNode)_t;
				match(_t,LITERAL_inline);
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void structSpecifier(AST _t) throws RecognitionException {
		
		TNode structSpecifier_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t37 = _t;
			TNode tmp38_AST_in = (TNode)_t;
			match(_t,LITERAL_struct);
			_t = _t.getFirstChild();
			structOrUnionBody(_t);
			_t = _retTree;
			_t = __t37;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void attributeDecl(AST _t) throws RecognitionException {
		
		TNode attributeDecl_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LITERAL___attribute:
			{
				AST __t72 = _t;
				TNode tmp39_AST_in = (TNode)_t;
				match(_t,LITERAL___attribute);
				_t = _t.getFirstChild();
				{
				_loop74:
				do {
					if (_t==null) _t=ASTNULL;
					if (((_t.getType() >= LITERAL_typedef && _t.getType() <= LITERAL___imag))) {
						TNode tmp40_AST_in = (TNode)_t;
						if ( _t==null ) throw new MismatchedTokenException();
						_t = _t.getNextSibling();
					}
					else {
						break _loop74;
					}
					
				} while (true);
				}
				_t = __t72;
				_t = _t.getNextSibling();
				break;
			}
			case NAsmAttribute:
			{
				AST __t75 = _t;
				TNode tmp41_AST_in = (TNode)_t;
				match(_t,NAsmAttribute);
				_t = _t.getFirstChild();
				TNode tmp42_AST_in = (TNode)_t;
				match(_t,LPAREN);
				_t = _t.getNextSibling();
				expr(_t);
				_t = _retTree;
				TNode tmp43_AST_in = (TNode)_t;
				match(_t,RPAREN);
				_t = _t.getNextSibling();
				_t = __t75;
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void unionSpecifier(AST _t) throws RecognitionException {
		
		TNode unionSpecifier_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t39 = _t;
			TNode tmp44_AST_in = (TNode)_t;
			match(_t,LITERAL_union);
			_t = _t.getFirstChild();
			structOrUnionBody(_t);
			_t = _retTree;
			_t = __t39;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void enumSpecifier(AST _t) throws RecognitionException {
		
		TNode enumSpecifier_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t63 = _t;
			TNode tmp45_AST_in = (TNode)_t;
			match(_t,LITERAL_enum);
			_t = _t.getFirstChild();
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case ID:
			{
				TNode tmp46_AST_in = (TNode)_t;
				match(_t,ID);
				_t = _t.getNextSibling();
				break;
			}
			case 3:
			case LCURLY:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LCURLY:
			{
				TNode tmp47_AST_in = (TNode)_t;
				match(_t,LCURLY);
				_t = _t.getNextSibling();
				enumList(_t);
				_t = _retTree;
				TNode tmp48_AST_in = (TNode)_t;
				match(_t,RCURLY);
				_t = _t.getNextSibling();
				break;
			}
			case 3:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			_t = __t63;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void typedefName(AST _t) throws RecognitionException {
		
		TNode typedefName_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t35 = _t;
			TNode tmp49_AST_in = (TNode)_t;
			match(_t,NTypedefName);
			_t = _t.getFirstChild();
			TNode tmp50_AST_in = (TNode)_t;
			match(_t,ID);
			_t = _t.getNextSibling();
			_t = __t35;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void typeName(AST _t) throws RecognitionException {
		
		TNode typeName_AST_in = (TNode)_t;
		
		try {      // for error handling
			specifierQualifierList(_t);
			_t = _retTree;
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case NNonemptyAbstractDeclarator:
			{
				nonemptyAbstractDeclarator(_t);
				_t = _retTree;
				break;
			}
			case RPAREN:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void structOrUnionBody(AST _t) throws RecognitionException {
		
		TNode structOrUnionBody_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			boolean synPredMatched43 = false;
			if (((_t.getType()==ID))) {
				AST __t43 = _t;
				synPredMatched43 = true;
				inputState.guessing++;
				try {
					{
					TNode tmp51_AST_in = (TNode)_t;
					match(_t,ID);
					_t = _t.getNextSibling();
					TNode tmp52_AST_in = (TNode)_t;
					match(_t,LCURLY);
					_t = _t.getNextSibling();
					}
				}
				catch (RecognitionException pe) {
					synPredMatched43 = false;
				}
				_t = __t43;
				inputState.guessing--;
			}
			if ( synPredMatched43 ) {
				TNode tmp53_AST_in = (TNode)_t;
				match(_t,ID);
				_t = _t.getNextSibling();
				TNode tmp54_AST_in = (TNode)_t;
				match(_t,LCURLY);
				_t = _t.getNextSibling();
				{
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case LITERAL_volatile:
				case LITERAL_struct:
				case LITERAL_union:
				case LITERAL_enum:
				case LITERAL_const:
				case LITERAL_void:
				case LITERAL_char:
				case LITERAL_short:
				case LITERAL_int:
				case LITERAL_long:
				case LITERAL_float:
				case LITERAL_double:
				case LITERAL_signed:
				case LITERAL_unsigned:
				case NTypedefName:
				case LITERAL_byte:
				case LITERAL_boolean:
				case LITERAL_Servo:
				case LITERAL_Wire:
				case LITERAL_typeof:
				case LITERAL___complex:
				{
					structDeclarationList(_t);
					_t = _retTree;
					break;
				}
				case RCURLY:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(_t);
				}
				}
				}
				TNode tmp55_AST_in = (TNode)_t;
				match(_t,RCURLY);
				_t = _t.getNextSibling();
			}
			else if ((_t.getType()==LCURLY)) {
				TNode tmp56_AST_in = (TNode)_t;
				match(_t,LCURLY);
				_t = _t.getNextSibling();
				{
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case LITERAL_volatile:
				case LITERAL_struct:
				case LITERAL_union:
				case LITERAL_enum:
				case LITERAL_const:
				case LITERAL_void:
				case LITERAL_char:
				case LITERAL_short:
				case LITERAL_int:
				case LITERAL_long:
				case LITERAL_float:
				case LITERAL_double:
				case LITERAL_signed:
				case LITERAL_unsigned:
				case NTypedefName:
				case LITERAL_byte:
				case LITERAL_boolean:
				case LITERAL_Servo:
				case LITERAL_Wire:
				case LITERAL_typeof:
				case LITERAL___complex:
				{
					structDeclarationList(_t);
					_t = _retTree;
					break;
				}
				case RCURLY:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(_t);
				}
				}
				}
				TNode tmp57_AST_in = (TNode)_t;
				match(_t,RCURLY);
				_t = _t.getNextSibling();
			}
			else if ((_t.getType()==ID)) {
				TNode tmp58_AST_in = (TNode)_t;
				match(_t,ID);
				_t = _t.getNextSibling();
			}
			else {
				throw new NoViableAltException(_t);
			}
			
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void structDeclarationList(AST _t) throws RecognitionException {
		
		TNode structDeclarationList_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt48=0;
			_loop48:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_tokenSet_1.member(_t.getType()))) {
					structDeclaration(_t);
					_t = _retTree;
				}
				else {
					if ( _cnt48>=1 ) { break _loop48; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt48++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void structDeclaration(AST _t) throws RecognitionException {
		
		TNode structDeclaration_AST_in = (TNode)_t;
		
		try {      // for error handling
			specifierQualifierList(_t);
			_t = _retTree;
			structDeclaratorList(_t);
			_t = _retTree;
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void specifierQualifierList(AST _t) throws RecognitionException {
		
		TNode specifierQualifierList_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt52=0;
			_loop52:
			do {
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case LITERAL_struct:
				case LITERAL_union:
				case LITERAL_enum:
				case LITERAL_void:
				case LITERAL_char:
				case LITERAL_short:
				case LITERAL_int:
				case LITERAL_long:
				case LITERAL_float:
				case LITERAL_double:
				case LITERAL_signed:
				case LITERAL_unsigned:
				case NTypedefName:
				case LITERAL_byte:
				case LITERAL_boolean:
				case LITERAL_Servo:
				case LITERAL_Wire:
				case LITERAL_typeof:
				case LITERAL___complex:
				{
					typeSpecifier(_t);
					_t = _retTree;
					break;
				}
				case LITERAL_volatile:
				case LITERAL_const:
				{
					typeQualifier(_t);
					_t = _retTree;
					break;
				}
				default:
				{
					if ( _cnt52>=1 ) { break _loop52; } else {throw new NoViableAltException(_t);}
				}
				}
				_cnt52++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void structDeclaratorList(AST _t) throws RecognitionException {
		
		TNode structDeclaratorList_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt55=0;
			_loop55:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==NStructDeclarator)) {
					structDeclarator(_t);
					_t = _retTree;
				}
				else {
					if ( _cnt55>=1 ) { break _loop55; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt55++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void structDeclarator(AST _t) throws RecognitionException {
		
		TNode structDeclarator_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t57 = _t;
			TNode tmp59_AST_in = (TNode)_t;
			match(_t,NStructDeclarator);
			_t = _t.getFirstChild();
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case NDeclarator:
			{
				declarator(_t);
				_t = _retTree;
				break;
			}
			case 3:
			case COLON:
			case NAsmAttribute:
			case LITERAL___attribute:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case COLON:
			{
				TNode tmp60_AST_in = (TNode)_t;
				match(_t,COLON);
				_t = _t.getNextSibling();
				expr(_t);
				_t = _retTree;
				break;
			}
			case 3:
			case NAsmAttribute:
			case LITERAL___attribute:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			{
			_loop61:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==NAsmAttribute||_t.getType()==LITERAL___attribute)) {
					attributeDecl(_t);
					_t = _retTree;
				}
				else {
					break _loop61;
				}
				
			} while (true);
			}
			_t = __t57;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void declarator(AST _t) throws RecognitionException {
		
		TNode declarator_AST_in = (TNode)_t;
		TNode id = null;
		
		try {      // for error handling
			AST __t107 = _t;
			TNode tmp61_AST_in = (TNode)_t;
			match(_t,NDeclarator);
			_t = _t.getFirstChild();
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case NPointerGroup:
			{
				pointerGroup(_t);
				_t = _retTree;
				break;
			}
			case ID:
			case LPAREN:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case ID:
			{
				id = (TNode)_t;
				match(_t,ID);
				_t = _t.getNextSibling();
				break;
			}
			case LPAREN:
			{
				TNode tmp62_AST_in = (TNode)_t;
				match(_t,LPAREN);
				_t = _t.getNextSibling();
				declarator(_t);
				_t = _retTree;
				TNode tmp63_AST_in = (TNode)_t;
				match(_t,RPAREN);
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			{
			_loop115:
			do {
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case NParameterTypeList:
				{
					AST __t111 = _t;
					TNode tmp64_AST_in = (TNode)_t;
					match(_t,NParameterTypeList);
					_t = _t.getFirstChild();
					{
					if (_t==null) _t=ASTNULL;
					switch ( _t.getType()) {
					case NParameterDeclaration:
					{
						parameterTypeList(_t);
						_t = _retTree;
						break;
					}
					case ID:
					case RPAREN:
					{
						{
						if (_t==null) _t=ASTNULL;
						switch ( _t.getType()) {
						case ID:
						{
							idList(_t);
							_t = _retTree;
							break;
						}
						case RPAREN:
						{
							break;
						}
						default:
						{
							throw new NoViableAltException(_t);
						}
						}
						}
						break;
					}
					default:
					{
						throw new NoViableAltException(_t);
					}
					}
					}
					TNode tmp65_AST_in = (TNode)_t;
					match(_t,RPAREN);
					_t = _t.getNextSibling();
					_t = __t111;
					_t = _t.getNextSibling();
					break;
				}
				case LBRACKET:
				{
					TNode tmp66_AST_in = (TNode)_t;
					match(_t,LBRACKET);
					_t = _t.getNextSibling();
					{
					if (_t==null) _t=ASTNULL;
					switch ( _t.getType()) {
					case ID:
					case ASSIGN:
					case STAR:
					case LPAREN:
					case DIV_ASSIGN:
					case PLUS_ASSIGN:
					case MINUS_ASSIGN:
					case STAR_ASSIGN:
					case MOD_ASSIGN:
					case RSHIFT_ASSIGN:
					case LSHIFT_ASSIGN:
					case BAND_ASSIGN:
					case BOR_ASSIGN:
					case BXOR_ASSIGN:
					case QUESTION:
					case LOR:
					case LAND:
					case BOR:
					case BXOR:
					case BAND:
					case EQUAL:
					case NOT_EQUAL:
					case LT:
					case LTE:
					case GT:
					case GTE:
					case LSHIFT:
					case RSHIFT:
					case PLUS:
					case MINUS:
					case DIV:
					case MOD:
					case INC:
					case DEC:
					case LITERAL_sizeof:
					case CharLiteral:
					case NCast:
					case NExpressionGroup:
					case NInitializer:
					case NEmptyExpression:
					case NCommaExpr:
					case NUnaryExpr:
					case NPostfixExpr:
					case NRangeExpr:
					case NStringSeq:
					case NLcurlyInitializer:
					case NGnuAsmExpr:
					case Number:
					case LITERAL___alignof:
					{
						expr(_t);
						_t = _retTree;
						break;
					}
					case RBRACKET:
					{
						break;
					}
					default:
					{
						throw new NoViableAltException(_t);
					}
					}
					}
					TNode tmp67_AST_in = (TNode)_t;
					match(_t,RBRACKET);
					_t = _t.getNextSibling();
					break;
				}
				default:
				{
					break _loop115;
				}
				}
			} while (true);
			}
			_t = __t107;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void enumList(AST _t) throws RecognitionException {
		
		TNode enumList_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt68=0;
			_loop68:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==ID)) {
					enumerator(_t);
					_t = _retTree;
				}
				else {
					if ( _cnt68>=1 ) { break _loop68; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt68++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void enumerator(AST _t) throws RecognitionException {
		
		TNode enumerator_AST_in = (TNode)_t;
		
		try {      // for error handling
			TNode tmp68_AST_in = (TNode)_t;
			match(_t,ID);
			_t = _t.getNextSibling();
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case ASSIGN:
			{
				TNode tmp69_AST_in = (TNode)_t;
				match(_t,ASSIGN);
				_t = _t.getNextSibling();
				expr(_t);
				_t = _retTree;
				break;
			}
			case RCURLY:
			case ID:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void initDecl(AST _t) throws RecognitionException {
		
		TNode initDecl_AST_in = (TNode)_t;
		String declName = "";
		
		try {      // for error handling
			AST __t80 = _t;
			TNode tmp70_AST_in = (TNode)_t;
			match(_t,NInitDecl);
			_t = _t.getFirstChild();
			declarator(_t);
			_t = _retTree;
			{
			_loop82:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==NAsmAttribute||_t.getType()==LITERAL___attribute)) {
					attributeDecl(_t);
					_t = _retTree;
				}
				else {
					break _loop82;
				}
				
			} while (true);
			}
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case ASSIGN:
			{
				TNode tmp71_AST_in = (TNode)_t;
				match(_t,ASSIGN);
				_t = _t.getNextSibling();
				initializer(_t);
				_t = _retTree;
				break;
			}
			case COLON:
			{
				TNode tmp72_AST_in = (TNode)_t;
				match(_t,COLON);
				_t = _t.getNextSibling();
				expr(_t);
				_t = _retTree;
				break;
			}
			case 3:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			_t = __t80;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void initializer(AST _t) throws RecognitionException {
		
		TNode initializer_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case NInitializer:
			{
				AST __t94 = _t;
				TNode tmp73_AST_in = (TNode)_t;
				match(_t,NInitializer);
				_t = _t.getFirstChild();
				{
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case NInitializerElementLabel:
				{
					initializerElementLabel(_t);
					_t = _retTree;
					break;
				}
				case ID:
				case ASSIGN:
				case STAR:
				case LPAREN:
				case DIV_ASSIGN:
				case PLUS_ASSIGN:
				case MINUS_ASSIGN:
				case STAR_ASSIGN:
				case MOD_ASSIGN:
				case RSHIFT_ASSIGN:
				case LSHIFT_ASSIGN:
				case BAND_ASSIGN:
				case BOR_ASSIGN:
				case BXOR_ASSIGN:
				case QUESTION:
				case LOR:
				case LAND:
				case BOR:
				case BXOR:
				case BAND:
				case EQUAL:
				case NOT_EQUAL:
				case LT:
				case LTE:
				case GT:
				case GTE:
				case LSHIFT:
				case RSHIFT:
				case PLUS:
				case MINUS:
				case DIV:
				case MOD:
				case INC:
				case DEC:
				case LITERAL_sizeof:
				case CharLiteral:
				case NCast:
				case NExpressionGroup:
				case NInitializer:
				case NEmptyExpression:
				case NCommaExpr:
				case NUnaryExpr:
				case NPostfixExpr:
				case NRangeExpr:
				case NStringSeq:
				case NLcurlyInitializer:
				case NGnuAsmExpr:
				case Number:
				case LITERAL___alignof:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(_t);
				}
				}
				}
				expr(_t);
				_t = _retTree;
				_t = __t94;
				_t = _t.getNextSibling();
				break;
			}
			case NLcurlyInitializer:
			{
				lcurlyInitializer(_t);
				_t = _retTree;
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void pointerGroup(AST _t) throws RecognitionException {
		
		TNode pointerGroup_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t85 = _t;
			TNode tmp74_AST_in = (TNode)_t;
			match(_t,NPointerGroup);
			_t = _t.getFirstChild();
			{
			int _cnt89=0;
			_loop89:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==STAR)) {
					TNode tmp75_AST_in = (TNode)_t;
					match(_t,STAR);
					_t = _t.getNextSibling();
					{
					_loop88:
					do {
						if (_t==null) _t=ASTNULL;
						if ((_t.getType()==LITERAL_volatile||_t.getType()==LITERAL_const)) {
							typeQualifier(_t);
							_t = _retTree;
						}
						else {
							break _loop88;
						}
						
					} while (true);
					}
				}
				else {
					if ( _cnt89>=1 ) { break _loop89; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt89++;
			} while (true);
			}
			_t = __t85;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void idList(AST _t) throws RecognitionException {
		
		TNode idList_AST_in = (TNode)_t;
		
		try {      // for error handling
			TNode tmp76_AST_in = (TNode)_t;
			match(_t,ID);
			_t = _t.getNextSibling();
			{
			_loop92:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==COMMA)) {
					TNode tmp77_AST_in = (TNode)_t;
					match(_t,COMMA);
					_t = _t.getNextSibling();
					TNode tmp78_AST_in = (TNode)_t;
					match(_t,ID);
					_t = _t.getNextSibling();
				}
				else {
					break _loop92;
				}
				
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void initializerElementLabel(AST _t) throws RecognitionException {
		
		TNode initializerElementLabel_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t97 = _t;
			TNode tmp79_AST_in = (TNode)_t;
			match(_t,NInitializerElementLabel);
			_t = _t.getFirstChild();
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LBRACKET:
			{
				{
				TNode tmp80_AST_in = (TNode)_t;
				match(_t,LBRACKET);
				_t = _t.getNextSibling();
				expr(_t);
				_t = _retTree;
				TNode tmp81_AST_in = (TNode)_t;
				match(_t,RBRACKET);
				_t = _t.getNextSibling();
				{
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case ASSIGN:
				{
					TNode tmp82_AST_in = (TNode)_t;
					match(_t,ASSIGN);
					_t = _t.getNextSibling();
					break;
				}
				case 3:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(_t);
				}
				}
				}
				}
				break;
			}
			case ID:
			{
				TNode tmp83_AST_in = (TNode)_t;
				match(_t,ID);
				_t = _t.getNextSibling();
				TNode tmp84_AST_in = (TNode)_t;
				match(_t,COLON);
				_t = _t.getNextSibling();
				break;
			}
			case DOT:
			{
				TNode tmp85_AST_in = (TNode)_t;
				match(_t,DOT);
				_t = _t.getNextSibling();
				TNode tmp86_AST_in = (TNode)_t;
				match(_t,ID);
				_t = _t.getNextSibling();
				TNode tmp87_AST_in = (TNode)_t;
				match(_t,ASSIGN);
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			_t = __t97;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void lcurlyInitializer(AST _t) throws RecognitionException {
		
		TNode lcurlyInitializer_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t102 = _t;
			TNode tmp88_AST_in = (TNode)_t;
			match(_t,NLcurlyInitializer);
			_t = _t.getFirstChild();
			initializerList(_t);
			_t = _retTree;
			TNode tmp89_AST_in = (TNode)_t;
			match(_t,RCURLY);
			_t = _t.getNextSibling();
			_t = __t102;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void initializerList(AST _t) throws RecognitionException {
		
		TNode initializerList_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			_loop105:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==NInitializer||_t.getType()==NLcurlyInitializer)) {
					initializer(_t);
					_t = _retTree;
				}
				else {
					break _loop105;
				}
				
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void parameterTypeList(AST _t) throws RecognitionException {
		
		TNode parameterTypeList_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt119=0;
			_loop119:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==NParameterDeclaration)) {
					parameterDeclaration(_t);
					_t = _retTree;
					{
					if (_t==null) _t=ASTNULL;
					switch ( _t.getType()) {
					case COMMA:
					{
						TNode tmp90_AST_in = (TNode)_t;
						match(_t,COMMA);
						_t = _t.getNextSibling();
						break;
					}
					case SEMI:
					{
						TNode tmp91_AST_in = (TNode)_t;
						match(_t,SEMI);
						_t = _t.getNextSibling();
						break;
					}
					case RPAREN:
					case VARARGS:
					case NParameterDeclaration:
					{
						break;
					}
					default:
					{
						throw new NoViableAltException(_t);
					}
					}
					}
				}
				else {
					if ( _cnt119>=1 ) { break _loop119; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt119++;
			} while (true);
			}
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case VARARGS:
			{
				TNode tmp92_AST_in = (TNode)_t;
				match(_t,VARARGS);
				_t = _t.getNextSibling();
				break;
			}
			case RPAREN:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void parameterDeclaration(AST _t) throws RecognitionException {
		
		TNode parameterDeclaration_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t122 = _t;
			TNode tmp93_AST_in = (TNode)_t;
			match(_t,NParameterDeclaration);
			_t = _t.getFirstChild();
			declSpecifiers(_t);
			_t = _retTree;
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case NDeclarator:
			{
				declarator(_t);
				_t = _retTree;
				break;
			}
			case NNonemptyAbstractDeclarator:
			{
				nonemptyAbstractDeclarator(_t);
				_t = _retTree;
				break;
			}
			case 3:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			_t = __t122;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void nonemptyAbstractDeclarator(AST _t) throws RecognitionException {
		
		TNode nonemptyAbstractDeclarator_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t237 = _t;
			TNode tmp94_AST_in = (TNode)_t;
			match(_t,NNonemptyAbstractDeclarator);
			_t = _t.getFirstChild();
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case NPointerGroup:
			{
				pointerGroup(_t);
				_t = _retTree;
				{
				_loop244:
				do {
					if (_t==null) _t=ASTNULL;
					switch ( _t.getType()) {
					case LPAREN:
					{
						{
						TNode tmp95_AST_in = (TNode)_t;
						match(_t,LPAREN);
						_t = _t.getNextSibling();
						{
						if (_t==null) _t=ASTNULL;
						switch ( _t.getType()) {
						case NNonemptyAbstractDeclarator:
						{
							nonemptyAbstractDeclarator(_t);
							_t = _retTree;
							break;
						}
						case NParameterDeclaration:
						{
							parameterTypeList(_t);
							_t = _retTree;
							break;
						}
						case RPAREN:
						{
							break;
						}
						default:
						{
							throw new NoViableAltException(_t);
						}
						}
						}
						TNode tmp96_AST_in = (TNode)_t;
						match(_t,RPAREN);
						_t = _t.getNextSibling();
						}
						break;
					}
					case LBRACKET:
					{
						{
						TNode tmp97_AST_in = (TNode)_t;
						match(_t,LBRACKET);
						_t = _t.getNextSibling();
						{
						if (_t==null) _t=ASTNULL;
						switch ( _t.getType()) {
						case ID:
						case ASSIGN:
						case STAR:
						case LPAREN:
						case DIV_ASSIGN:
						case PLUS_ASSIGN:
						case MINUS_ASSIGN:
						case STAR_ASSIGN:
						case MOD_ASSIGN:
						case RSHIFT_ASSIGN:
						case LSHIFT_ASSIGN:
						case BAND_ASSIGN:
						case BOR_ASSIGN:
						case BXOR_ASSIGN:
						case QUESTION:
						case LOR:
						case LAND:
						case BOR:
						case BXOR:
						case BAND:
						case EQUAL:
						case NOT_EQUAL:
						case LT:
						case LTE:
						case GT:
						case GTE:
						case LSHIFT:
						case RSHIFT:
						case PLUS:
						case MINUS:
						case DIV:
						case MOD:
						case INC:
						case DEC:
						case LITERAL_sizeof:
						case CharLiteral:
						case NCast:
						case NExpressionGroup:
						case NInitializer:
						case NEmptyExpression:
						case NCommaExpr:
						case NUnaryExpr:
						case NPostfixExpr:
						case NRangeExpr:
						case NStringSeq:
						case NLcurlyInitializer:
						case NGnuAsmExpr:
						case Number:
						case LITERAL___alignof:
						{
							expr(_t);
							_t = _retTree;
							break;
						}
						case RBRACKET:
						{
							break;
						}
						default:
						{
							throw new NoViableAltException(_t);
						}
						}
						}
						TNode tmp98_AST_in = (TNode)_t;
						match(_t,RBRACKET);
						_t = _t.getNextSibling();
						}
						break;
					}
					default:
					{
						break _loop244;
					}
					}
				} while (true);
				}
				break;
			}
			case LPAREN:
			case LBRACKET:
			{
				{
				int _cnt250=0;
				_loop250:
				do {
					if (_t==null) _t=ASTNULL;
					switch ( _t.getType()) {
					case LPAREN:
					{
						{
						TNode tmp99_AST_in = (TNode)_t;
						match(_t,LPAREN);
						_t = _t.getNextSibling();
						{
						if (_t==null) _t=ASTNULL;
						switch ( _t.getType()) {
						case NNonemptyAbstractDeclarator:
						{
							nonemptyAbstractDeclarator(_t);
							_t = _retTree;
							break;
						}
						case NParameterDeclaration:
						{
							parameterTypeList(_t);
							_t = _retTree;
							break;
						}
						case RPAREN:
						{
							break;
						}
						default:
						{
							throw new NoViableAltException(_t);
						}
						}
						}
						TNode tmp100_AST_in = (TNode)_t;
						match(_t,RPAREN);
						_t = _t.getNextSibling();
						}
						break;
					}
					case LBRACKET:
					{
						{
						TNode tmp101_AST_in = (TNode)_t;
						match(_t,LBRACKET);
						_t = _t.getNextSibling();
						{
						if (_t==null) _t=ASTNULL;
						switch ( _t.getType()) {
						case ID:
						case ASSIGN:
						case STAR:
						case LPAREN:
						case DIV_ASSIGN:
						case PLUS_ASSIGN:
						case MINUS_ASSIGN:
						case STAR_ASSIGN:
						case MOD_ASSIGN:
						case RSHIFT_ASSIGN:
						case LSHIFT_ASSIGN:
						case BAND_ASSIGN:
						case BOR_ASSIGN:
						case BXOR_ASSIGN:
						case QUESTION:
						case LOR:
						case LAND:
						case BOR:
						case BXOR:
						case BAND:
						case EQUAL:
						case NOT_EQUAL:
						case LT:
						case LTE:
						case GT:
						case GTE:
						case LSHIFT:
						case RSHIFT:
						case PLUS:
						case MINUS:
						case DIV:
						case MOD:
						case INC:
						case DEC:
						case LITERAL_sizeof:
						case CharLiteral:
						case NCast:
						case NExpressionGroup:
						case NInitializer:
						case NEmptyExpression:
						case NCommaExpr:
						case NUnaryExpr:
						case NPostfixExpr:
						case NRangeExpr:
						case NStringSeq:
						case NLcurlyInitializer:
						case NGnuAsmExpr:
						case Number:
						case LITERAL___alignof:
						{
							expr(_t);
							_t = _retTree;
							break;
						}
						case RBRACKET:
						{
							break;
						}
						default:
						{
							throw new NoViableAltException(_t);
						}
						}
						}
						TNode tmp102_AST_in = (TNode)_t;
						match(_t,RBRACKET);
						_t = _t.getNextSibling();
						}
						break;
					}
					default:
					{
						if ( _cnt250>=1 ) { break _loop250; } else {throw new NoViableAltException(_t);}
					}
					}
					_cnt250++;
				} while (true);
				}
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			_t = __t237;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void functionDeclSpecifiers(AST _t) throws RecognitionException {
		
		TNode functionDeclSpecifiers_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt131=0;
			_loop131:
			do {
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case LITERAL_extern:
				case LITERAL_static:
				case LITERAL_inline:
				{
					functionStorageClassSpecifier(_t);
					_t = _retTree;
					break;
				}
				case LITERAL_volatile:
				case LITERAL_const:
				{
					typeQualifier(_t);
					_t = _retTree;
					break;
				}
				case LITERAL_struct:
				case LITERAL_union:
				case LITERAL_enum:
				case LITERAL_void:
				case LITERAL_char:
				case LITERAL_short:
				case LITERAL_int:
				case LITERAL_long:
				case LITERAL_float:
				case LITERAL_double:
				case LITERAL_signed:
				case LITERAL_unsigned:
				case NTypedefName:
				case LITERAL_byte:
				case LITERAL_boolean:
				case LITERAL_Servo:
				case LITERAL_Wire:
				case LITERAL_typeof:
				case LITERAL___complex:
				{
					typeSpecifier(_t);
					_t = _retTree;
					break;
				}
				default:
				{
					if ( _cnt131>=1 ) { break _loop131; } else {throw new NoViableAltException(_t);}
				}
				}
				_cnt131++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void compoundStatement(AST _t) throws RecognitionException {
		
		TNode compoundStatement_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t140 = _t;
			TNode tmp103_AST_in = (TNode)_t;
			match(_t,NCompoundStatement);
			_t = _t.getFirstChild();
			{
			_loop142:
			do {
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case NDeclaration:
				case LITERAL___label__:
				{
					declarationList(_t);
					_t = _retTree;
					break;
				}
				case NFunctionDef:
				{
					functionDef(_t);
					_t = _retTree;
					break;
				}
				default:
				{
					break _loop142;
				}
				}
			} while (true);
			}
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case SEMI:
			case LITERAL_while:
			case LITERAL_do:
			case LITERAL_for:
			case LITERAL_goto:
			case LITERAL_continue:
			case LITERAL_break:
			case LITERAL_return:
			case LITERAL_case:
			case LITERAL_default:
			case LITERAL_if:
			case LITERAL_switch:
			case NStatementExpr:
			case NCompoundStatement:
			case NLabel:
			{
				statementList(_t);
				_t = _retTree;
				break;
			}
			case RCURLY:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			TNode tmp104_AST_in = (TNode)_t;
			match(_t,RCURLY);
			_t = _t.getNextSibling();
			_t = __t140;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void declarationList(AST _t) throws RecognitionException {
		
		TNode declarationList_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt134=0;
			_loop134:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==LITERAL___label__)) {
					localLabelDecl(_t);
					_t = _retTree;
				}
				else if ((_t.getType()==NDeclaration)) {
					declaration(_t);
					_t = _retTree;
				}
				else {
					if ( _cnt134>=1 ) { break _loop134; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt134++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void localLabelDecl(AST _t) throws RecognitionException {
		
		TNode localLabelDecl_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t136 = _t;
			TNode tmp105_AST_in = (TNode)_t;
			match(_t,LITERAL___label__);
			_t = _t.getFirstChild();
			{
			int _cnt138=0;
			_loop138:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==ID)) {
					TNode tmp106_AST_in = (TNode)_t;
					match(_t,ID);
					_t = _t.getNextSibling();
				}
				else {
					if ( _cnt138>=1 ) { break _loop138; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt138++;
			} while (true);
			}
			_t = __t136;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void statementList(AST _t) throws RecognitionException {
		
		TNode statementList_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt146=0;
			_loop146:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_tokenSet_2.member(_t.getType()))) {
					statement(_t);
					_t = _retTree;
				}
				else {
					if ( _cnt146>=1 ) { break _loop146; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt146++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void statement(AST _t) throws RecognitionException {
		
		TNode statement_AST_in = (TNode)_t;
		
		try {      // for error handling
			statementBody(_t);
			_t = _retTree;
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void statementBody(AST _t) throws RecognitionException {
		
		TNode statementBody_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case SEMI:
			{
				TNode tmp107_AST_in = (TNode)_t;
				match(_t,SEMI);
				_t = _t.getNextSibling();
				break;
			}
			case NCompoundStatement:
			{
				compoundStatement(_t);
				_t = _retTree;
				break;
			}
			case NStatementExpr:
			{
				AST __t149 = _t;
				TNode tmp108_AST_in = (TNode)_t;
				match(_t,NStatementExpr);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				_t = __t149;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_while:
			{
				AST __t150 = _t;
				TNode tmp109_AST_in = (TNode)_t;
				match(_t,LITERAL_while);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				statement(_t);
				_t = _retTree;
				_t = __t150;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_do:
			{
				AST __t151 = _t;
				TNode tmp110_AST_in = (TNode)_t;
				match(_t,LITERAL_do);
				_t = _t.getFirstChild();
				statement(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t151;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_for:
			{
				AST __t152 = _t;
				TNode tmp111_AST_in = (TNode)_t;
				match(_t,LITERAL_for);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				statement(_t);
				_t = _retTree;
				_t = __t152;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_goto:
			{
				AST __t153 = _t;
				TNode tmp112_AST_in = (TNode)_t;
				match(_t,LITERAL_goto);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				_t = __t153;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_continue:
			{
				TNode tmp113_AST_in = (TNode)_t;
				match(_t,LITERAL_continue);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_break:
			{
				TNode tmp114_AST_in = (TNode)_t;
				match(_t,LITERAL_break);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_return:
			{
				AST __t154 = _t;
				TNode tmp115_AST_in = (TNode)_t;
				match(_t,LITERAL_return);
				_t = _t.getFirstChild();
				{
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case ID:
				case ASSIGN:
				case STAR:
				case LPAREN:
				case DIV_ASSIGN:
				case PLUS_ASSIGN:
				case MINUS_ASSIGN:
				case STAR_ASSIGN:
				case MOD_ASSIGN:
				case RSHIFT_ASSIGN:
				case LSHIFT_ASSIGN:
				case BAND_ASSIGN:
				case BOR_ASSIGN:
				case BXOR_ASSIGN:
				case QUESTION:
				case LOR:
				case LAND:
				case BOR:
				case BXOR:
				case BAND:
				case EQUAL:
				case NOT_EQUAL:
				case LT:
				case LTE:
				case GT:
				case GTE:
				case LSHIFT:
				case RSHIFT:
				case PLUS:
				case MINUS:
				case DIV:
				case MOD:
				case INC:
				case DEC:
				case LITERAL_sizeof:
				case CharLiteral:
				case NCast:
				case NExpressionGroup:
				case NInitializer:
				case NEmptyExpression:
				case NCommaExpr:
				case NUnaryExpr:
				case NPostfixExpr:
				case NRangeExpr:
				case NStringSeq:
				case NLcurlyInitializer:
				case NGnuAsmExpr:
				case Number:
				case LITERAL___alignof:
				{
					expr(_t);
					_t = _retTree;
					break;
				}
				case 3:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(_t);
				}
				}
				}
				_t = __t154;
				_t = _t.getNextSibling();
				break;
			}
			case NLabel:
			{
				AST __t156 = _t;
				TNode tmp116_AST_in = (TNode)_t;
				match(_t,NLabel);
				_t = _t.getFirstChild();
				TNode tmp117_AST_in = (TNode)_t;
				match(_t,ID);
				_t = _t.getNextSibling();
				{
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case SEMI:
				case LITERAL_while:
				case LITERAL_do:
				case LITERAL_for:
				case LITERAL_goto:
				case LITERAL_continue:
				case LITERAL_break:
				case LITERAL_return:
				case LITERAL_case:
				case LITERAL_default:
				case LITERAL_if:
				case LITERAL_switch:
				case NStatementExpr:
				case NCompoundStatement:
				case NLabel:
				{
					statement(_t);
					_t = _retTree;
					break;
				}
				case 3:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(_t);
				}
				}
				}
				_t = __t156;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_case:
			{
				AST __t158 = _t;
				TNode tmp118_AST_in = (TNode)_t;
				match(_t,LITERAL_case);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				{
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case SEMI:
				case LITERAL_while:
				case LITERAL_do:
				case LITERAL_for:
				case LITERAL_goto:
				case LITERAL_continue:
				case LITERAL_break:
				case LITERAL_return:
				case LITERAL_case:
				case LITERAL_default:
				case LITERAL_if:
				case LITERAL_switch:
				case NStatementExpr:
				case NCompoundStatement:
				case NLabel:
				{
					statement(_t);
					_t = _retTree;
					break;
				}
				case 3:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(_t);
				}
				}
				}
				_t = __t158;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_default:
			{
				AST __t160 = _t;
				TNode tmp119_AST_in = (TNode)_t;
				match(_t,LITERAL_default);
				_t = _t.getFirstChild();
				{
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case SEMI:
				case LITERAL_while:
				case LITERAL_do:
				case LITERAL_for:
				case LITERAL_goto:
				case LITERAL_continue:
				case LITERAL_break:
				case LITERAL_return:
				case LITERAL_case:
				case LITERAL_default:
				case LITERAL_if:
				case LITERAL_switch:
				case NStatementExpr:
				case NCompoundStatement:
				case NLabel:
				{
					statement(_t);
					_t = _retTree;
					break;
				}
				case 3:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(_t);
				}
				}
				}
				_t = __t160;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_if:
			{
				AST __t162 = _t;
				TNode tmp120_AST_in = (TNode)_t;
				match(_t,LITERAL_if);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				statement(_t);
				_t = _retTree;
				{
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case LITERAL_else:
				{
					TNode tmp121_AST_in = (TNode)_t;
					match(_t,LITERAL_else);
					_t = _t.getNextSibling();
					statement(_t);
					_t = _retTree;
					break;
				}
				case 3:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(_t);
				}
				}
				}
				_t = __t162;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_switch:
			{
				AST __t164 = _t;
				TNode tmp122_AST_in = (TNode)_t;
				match(_t,LITERAL_switch);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				statement(_t);
				_t = _retTree;
				_t = __t164;
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void assignExpr(AST _t) throws RecognitionException {
		
		TNode assignExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case ASSIGN:
			{
				AST __t190 = _t;
				TNode tmp123_AST_in = (TNode)_t;
				match(_t,ASSIGN);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t190;
				_t = _t.getNextSibling();
				break;
			}
			case DIV_ASSIGN:
			{
				AST __t191 = _t;
				TNode tmp124_AST_in = (TNode)_t;
				match(_t,DIV_ASSIGN);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t191;
				_t = _t.getNextSibling();
				break;
			}
			case PLUS_ASSIGN:
			{
				AST __t192 = _t;
				TNode tmp125_AST_in = (TNode)_t;
				match(_t,PLUS_ASSIGN);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t192;
				_t = _t.getNextSibling();
				break;
			}
			case MINUS_ASSIGN:
			{
				AST __t193 = _t;
				TNode tmp126_AST_in = (TNode)_t;
				match(_t,MINUS_ASSIGN);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t193;
				_t = _t.getNextSibling();
				break;
			}
			case STAR_ASSIGN:
			{
				AST __t194 = _t;
				TNode tmp127_AST_in = (TNode)_t;
				match(_t,STAR_ASSIGN);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t194;
				_t = _t.getNextSibling();
				break;
			}
			case MOD_ASSIGN:
			{
				AST __t195 = _t;
				TNode tmp128_AST_in = (TNode)_t;
				match(_t,MOD_ASSIGN);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t195;
				_t = _t.getNextSibling();
				break;
			}
			case RSHIFT_ASSIGN:
			{
				AST __t196 = _t;
				TNode tmp129_AST_in = (TNode)_t;
				match(_t,RSHIFT_ASSIGN);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t196;
				_t = _t.getNextSibling();
				break;
			}
			case LSHIFT_ASSIGN:
			{
				AST __t197 = _t;
				TNode tmp130_AST_in = (TNode)_t;
				match(_t,LSHIFT_ASSIGN);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t197;
				_t = _t.getNextSibling();
				break;
			}
			case BAND_ASSIGN:
			{
				AST __t198 = _t;
				TNode tmp131_AST_in = (TNode)_t;
				match(_t,BAND_ASSIGN);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t198;
				_t = _t.getNextSibling();
				break;
			}
			case BOR_ASSIGN:
			{
				AST __t199 = _t;
				TNode tmp132_AST_in = (TNode)_t;
				match(_t,BOR_ASSIGN);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t199;
				_t = _t.getNextSibling();
				break;
			}
			case BXOR_ASSIGN:
			{
				AST __t200 = _t;
				TNode tmp133_AST_in = (TNode)_t;
				match(_t,BXOR_ASSIGN);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t200;
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void conditionalExpr(AST _t) throws RecognitionException {
		
		TNode conditionalExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t202 = _t;
			TNode tmp134_AST_in = (TNode)_t;
			match(_t,QUESTION);
			_t = _t.getFirstChild();
			expr(_t);
			_t = _retTree;
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case ID:
			case ASSIGN:
			case STAR:
			case LPAREN:
			case DIV_ASSIGN:
			case PLUS_ASSIGN:
			case MINUS_ASSIGN:
			case STAR_ASSIGN:
			case MOD_ASSIGN:
			case RSHIFT_ASSIGN:
			case LSHIFT_ASSIGN:
			case BAND_ASSIGN:
			case BOR_ASSIGN:
			case BXOR_ASSIGN:
			case QUESTION:
			case LOR:
			case LAND:
			case BOR:
			case BXOR:
			case BAND:
			case EQUAL:
			case NOT_EQUAL:
			case LT:
			case LTE:
			case GT:
			case GTE:
			case LSHIFT:
			case RSHIFT:
			case PLUS:
			case MINUS:
			case DIV:
			case MOD:
			case INC:
			case DEC:
			case LITERAL_sizeof:
			case CharLiteral:
			case NCast:
			case NExpressionGroup:
			case NInitializer:
			case NEmptyExpression:
			case NCommaExpr:
			case NUnaryExpr:
			case NPostfixExpr:
			case NRangeExpr:
			case NStringSeq:
			case NLcurlyInitializer:
			case NGnuAsmExpr:
			case Number:
			case LITERAL___alignof:
			{
				expr(_t);
				_t = _retTree;
				break;
			}
			case COLON:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			TNode tmp135_AST_in = (TNode)_t;
			match(_t,COLON);
			_t = _t.getNextSibling();
			expr(_t);
			_t = _retTree;
			_t = __t202;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void logicalOrExpr(AST _t) throws RecognitionException {
		
		TNode logicalOrExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t205 = _t;
			TNode tmp136_AST_in = (TNode)_t;
			match(_t,LOR);
			_t = _t.getFirstChild();
			expr(_t);
			_t = _retTree;
			expr(_t);
			_t = _retTree;
			_t = __t205;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void logicalAndExpr(AST _t) throws RecognitionException {
		
		TNode logicalAndExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t207 = _t;
			TNode tmp137_AST_in = (TNode)_t;
			match(_t,LAND);
			_t = _t.getFirstChild();
			expr(_t);
			_t = _retTree;
			expr(_t);
			_t = _retTree;
			_t = __t207;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void inclusiveOrExpr(AST _t) throws RecognitionException {
		
		TNode inclusiveOrExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t209 = _t;
			TNode tmp138_AST_in = (TNode)_t;
			match(_t,BOR);
			_t = _t.getFirstChild();
			expr(_t);
			_t = _retTree;
			expr(_t);
			_t = _retTree;
			_t = __t209;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void exclusiveOrExpr(AST _t) throws RecognitionException {
		
		TNode exclusiveOrExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t211 = _t;
			TNode tmp139_AST_in = (TNode)_t;
			match(_t,BXOR);
			_t = _t.getFirstChild();
			expr(_t);
			_t = _retTree;
			expr(_t);
			_t = _retTree;
			_t = __t211;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void bitAndExpr(AST _t) throws RecognitionException {
		
		TNode bitAndExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t213 = _t;
			TNode tmp140_AST_in = (TNode)_t;
			match(_t,BAND);
			_t = _t.getFirstChild();
			expr(_t);
			_t = _retTree;
			expr(_t);
			_t = _retTree;
			_t = __t213;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void equalityExpr(AST _t) throws RecognitionException {
		
		TNode equalityExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case EQUAL:
			{
				AST __t215 = _t;
				TNode tmp141_AST_in = (TNode)_t;
				match(_t,EQUAL);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t215;
				_t = _t.getNextSibling();
				break;
			}
			case NOT_EQUAL:
			{
				AST __t216 = _t;
				TNode tmp142_AST_in = (TNode)_t;
				match(_t,NOT_EQUAL);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t216;
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void relationalExpr(AST _t) throws RecognitionException {
		
		TNode relationalExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LT:
			{
				AST __t218 = _t;
				TNode tmp143_AST_in = (TNode)_t;
				match(_t,LT);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t218;
				_t = _t.getNextSibling();
				break;
			}
			case LTE:
			{
				AST __t219 = _t;
				TNode tmp144_AST_in = (TNode)_t;
				match(_t,LTE);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t219;
				_t = _t.getNextSibling();
				break;
			}
			case GT:
			{
				AST __t220 = _t;
				TNode tmp145_AST_in = (TNode)_t;
				match(_t,GT);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t220;
				_t = _t.getNextSibling();
				break;
			}
			case GTE:
			{
				AST __t221 = _t;
				TNode tmp146_AST_in = (TNode)_t;
				match(_t,GTE);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t221;
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void shiftExpr(AST _t) throws RecognitionException {
		
		TNode shiftExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LSHIFT:
			{
				AST __t223 = _t;
				TNode tmp147_AST_in = (TNode)_t;
				match(_t,LSHIFT);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t223;
				_t = _t.getNextSibling();
				break;
			}
			case RSHIFT:
			{
				AST __t224 = _t;
				TNode tmp148_AST_in = (TNode)_t;
				match(_t,RSHIFT);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t224;
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void additiveExpr(AST _t) throws RecognitionException {
		
		TNode additiveExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case PLUS:
			{
				AST __t226 = _t;
				TNode tmp149_AST_in = (TNode)_t;
				match(_t,PLUS);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t226;
				_t = _t.getNextSibling();
				break;
			}
			case MINUS:
			{
				AST __t227 = _t;
				TNode tmp150_AST_in = (TNode)_t;
				match(_t,MINUS);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t227;
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void multExpr(AST _t) throws RecognitionException {
		
		TNode multExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case STAR:
			{
				AST __t229 = _t;
				TNode tmp151_AST_in = (TNode)_t;
				match(_t,STAR);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t229;
				_t = _t.getNextSibling();
				break;
			}
			case DIV:
			{
				AST __t230 = _t;
				TNode tmp152_AST_in = (TNode)_t;
				match(_t,DIV);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t230;
				_t = _t.getNextSibling();
				break;
			}
			case MOD:
			{
				AST __t231 = _t;
				TNode tmp153_AST_in = (TNode)_t;
				match(_t,MOD);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t231;
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void castExpr(AST _t) throws RecognitionException {
		
		TNode castExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t233 = _t;
			TNode tmp154_AST_in = (TNode)_t;
			match(_t,NCast);
			_t = _t.getFirstChild();
			typeName(_t);
			_t = _retTree;
			TNode tmp155_AST_in = (TNode)_t;
			match(_t,RPAREN);
			_t = _t.getNextSibling();
			expr(_t);
			_t = _retTree;
			_t = __t233;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void unaryExpr(AST _t) throws RecognitionException {
		
		TNode unaryExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case INC:
			{
				AST __t252 = _t;
				TNode tmp156_AST_in = (TNode)_t;
				match(_t,INC);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				_t = __t252;
				_t = _t.getNextSibling();
				break;
			}
			case DEC:
			{
				AST __t253 = _t;
				TNode tmp157_AST_in = (TNode)_t;
				match(_t,DEC);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				_t = __t253;
				_t = _t.getNextSibling();
				break;
			}
			case NUnaryExpr:
			{
				AST __t254 = _t;
				TNode tmp158_AST_in = (TNode)_t;
				match(_t,NUnaryExpr);
				_t = _t.getFirstChild();
				unaryOperator(_t);
				_t = _retTree;
				expr(_t);
				_t = _retTree;
				_t = __t254;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL_sizeof:
			{
				AST __t255 = _t;
				TNode tmp159_AST_in = (TNode)_t;
				match(_t,LITERAL_sizeof);
				_t = _t.getFirstChild();
				{
				boolean synPredMatched258 = false;
				if (((_t.getType()==LPAREN))) {
					AST __t258 = _t;
					synPredMatched258 = true;
					inputState.guessing++;
					try {
						{
						TNode tmp160_AST_in = (TNode)_t;
						match(_t,LPAREN);
						_t = _t.getNextSibling();
						typeName(_t);
						_t = _retTree;
						}
					}
					catch (RecognitionException pe) {
						synPredMatched258 = false;
					}
					_t = __t258;
					inputState.guessing--;
				}
				if ( synPredMatched258 ) {
					TNode tmp161_AST_in = (TNode)_t;
					match(_t,LPAREN);
					_t = _t.getNextSibling();
					typeName(_t);
					_t = _retTree;
					TNode tmp162_AST_in = (TNode)_t;
					match(_t,RPAREN);
					_t = _t.getNextSibling();
				}
				else if ((_tokenSet_3.member(_t.getType()))) {
					expr(_t);
					_t = _retTree;
				}
				else {
					throw new NoViableAltException(_t);
				}
				
				}
				_t = __t255;
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL___alignof:
			{
				AST __t259 = _t;
				TNode tmp163_AST_in = (TNode)_t;
				match(_t,LITERAL___alignof);
				_t = _t.getFirstChild();
				{
				boolean synPredMatched262 = false;
				if (((_t.getType()==LPAREN))) {
					AST __t262 = _t;
					synPredMatched262 = true;
					inputState.guessing++;
					try {
						{
						TNode tmp164_AST_in = (TNode)_t;
						match(_t,LPAREN);
						_t = _t.getNextSibling();
						typeName(_t);
						_t = _retTree;
						}
					}
					catch (RecognitionException pe) {
						synPredMatched262 = false;
					}
					_t = __t262;
					inputState.guessing--;
				}
				if ( synPredMatched262 ) {
					TNode tmp165_AST_in = (TNode)_t;
					match(_t,LPAREN);
					_t = _t.getNextSibling();
					typeName(_t);
					_t = _retTree;
					TNode tmp166_AST_in = (TNode)_t;
					match(_t,RPAREN);
					_t = _t.getNextSibling();
				}
				else if ((_tokenSet_3.member(_t.getType()))) {
					expr(_t);
					_t = _retTree;
				}
				else {
					throw new NoViableAltException(_t);
				}
				
				}
				_t = __t259;
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void postfixExpr(AST _t) throws RecognitionException {
		
		TNode postfixExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t265 = _t;
			TNode tmp167_AST_in = (TNode)_t;
			match(_t,NPostfixExpr);
			_t = _t.getFirstChild();
			primaryExpr(_t);
			_t = _retTree;
			{
			int _cnt269=0;
			_loop269:
			do {
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case PTR:
				{
					TNode tmp168_AST_in = (TNode)_t;
					match(_t,PTR);
					_t = _t.getNextSibling();
					TNode tmp169_AST_in = (TNode)_t;
					match(_t,ID);
					_t = _t.getNextSibling();
					break;
				}
				case DOT:
				{
					TNode tmp170_AST_in = (TNode)_t;
					match(_t,DOT);
					_t = _t.getNextSibling();
					TNode tmp171_AST_in = (TNode)_t;
					match(_t,ID);
					_t = _t.getNextSibling();
					break;
				}
				case NFunctionCallArgs:
				{
					AST __t267 = _t;
					TNode tmp172_AST_in = (TNode)_t;
					match(_t,NFunctionCallArgs);
					_t = _t.getFirstChild();
					{
					if (_t==null) _t=ASTNULL;
					switch ( _t.getType()) {
					case ID:
					case ASSIGN:
					case STAR:
					case LPAREN:
					case DIV_ASSIGN:
					case PLUS_ASSIGN:
					case MINUS_ASSIGN:
					case STAR_ASSIGN:
					case MOD_ASSIGN:
					case RSHIFT_ASSIGN:
					case LSHIFT_ASSIGN:
					case BAND_ASSIGN:
					case BOR_ASSIGN:
					case BXOR_ASSIGN:
					case QUESTION:
					case LOR:
					case LAND:
					case BOR:
					case BXOR:
					case BAND:
					case EQUAL:
					case NOT_EQUAL:
					case LT:
					case LTE:
					case GT:
					case GTE:
					case LSHIFT:
					case RSHIFT:
					case PLUS:
					case MINUS:
					case DIV:
					case MOD:
					case INC:
					case DEC:
					case LITERAL_sizeof:
					case CharLiteral:
					case NCast:
					case NExpressionGroup:
					case NInitializer:
					case NEmptyExpression:
					case NCommaExpr:
					case NUnaryExpr:
					case NPostfixExpr:
					case NRangeExpr:
					case NStringSeq:
					case NLcurlyInitializer:
					case NGnuAsmExpr:
					case Number:
					case LITERAL___alignof:
					{
						argExprList(_t);
						_t = _retTree;
						break;
					}
					case RPAREN:
					{
						break;
					}
					default:
					{
						throw new NoViableAltException(_t);
					}
					}
					}
					TNode tmp173_AST_in = (TNode)_t;
					match(_t,RPAREN);
					_t = _t.getNextSibling();
					_t = __t267;
					_t = _t.getNextSibling();
					break;
				}
				case LBRACKET:
				{
					TNode tmp174_AST_in = (TNode)_t;
					match(_t,LBRACKET);
					_t = _t.getNextSibling();
					expr(_t);
					_t = _retTree;
					TNode tmp175_AST_in = (TNode)_t;
					match(_t,RBRACKET);
					_t = _t.getNextSibling();
					break;
				}
				case INC:
				{
					TNode tmp176_AST_in = (TNode)_t;
					match(_t,INC);
					_t = _t.getNextSibling();
					break;
				}
				case DEC:
				{
					TNode tmp177_AST_in = (TNode)_t;
					match(_t,DEC);
					_t = _t.getNextSibling();
					break;
				}
				default:
				{
					if ( _cnt269>=1 ) { break _loop269; } else {throw new NoViableAltException(_t);}
				}
				}
				_cnt269++;
			} while (true);
			}
			_t = __t265;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void primaryExpr(AST _t) throws RecognitionException {
		
		TNode primaryExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case ID:
			{
				TNode tmp178_AST_in = (TNode)_t;
				match(_t,ID);
				_t = _t.getNextSibling();
				break;
			}
			case Number:
			{
				TNode tmp179_AST_in = (TNode)_t;
				match(_t,Number);
				_t = _t.getNextSibling();
				break;
			}
			case CharLiteral:
			{
				charConst(_t);
				_t = _retTree;
				break;
			}
			case NStringSeq:
			{
				stringConst(_t);
				_t = _retTree;
				break;
			}
			case NExpressionGroup:
			{
				AST __t271 = _t;
				TNode tmp180_AST_in = (TNode)_t;
				match(_t,NExpressionGroup);
				_t = _t.getFirstChild();
				expr(_t);
				_t = _retTree;
				_t = __t271;
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void commaExpr(AST _t) throws RecognitionException {
		
		TNode commaExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t167 = _t;
			TNode tmp181_AST_in = (TNode)_t;
			match(_t,NCommaExpr);
			_t = _t.getFirstChild();
			expr(_t);
			_t = _retTree;
			expr(_t);
			_t = _retTree;
			_t = __t167;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void emptyExpr(AST _t) throws RecognitionException {
		
		TNode emptyExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			TNode tmp182_AST_in = (TNode)_t;
			match(_t,NEmptyExpression);
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void compoundStatementExpr(AST _t) throws RecognitionException {
		
		TNode compoundStatementExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t170 = _t;
			TNode tmp183_AST_in = (TNode)_t;
			match(_t,LPAREN);
			_t = _t.getFirstChild();
			compoundStatement(_t);
			_t = _retTree;
			TNode tmp184_AST_in = (TNode)_t;
			match(_t,RPAREN);
			_t = _t.getNextSibling();
			_t = __t170;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void rangeExpr(AST _t) throws RecognitionException {
		
		TNode rangeExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t172 = _t;
			TNode tmp185_AST_in = (TNode)_t;
			match(_t,NRangeExpr);
			_t = _t.getFirstChild();
			expr(_t);
			_t = _retTree;
			TNode tmp186_AST_in = (TNode)_t;
			match(_t,VARARGS);
			_t = _t.getNextSibling();
			expr(_t);
			_t = _retTree;
			_t = __t172;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void gnuAsmExpr(AST _t) throws RecognitionException {
		
		TNode gnuAsmExpr_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t174 = _t;
			TNode tmp187_AST_in = (TNode)_t;
			match(_t,NGnuAsmExpr);
			_t = _t.getFirstChild();
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LITERAL_volatile:
			{
				TNode tmp188_AST_in = (TNode)_t;
				match(_t,LITERAL_volatile);
				_t = _t.getNextSibling();
				break;
			}
			case LPAREN:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			TNode tmp189_AST_in = (TNode)_t;
			match(_t,LPAREN);
			_t = _t.getNextSibling();
			stringConst(_t);
			_t = _retTree;
			{
			if (_t==null) _t=ASTNULL;
			if ((_t.getType()==COLON)) {
				TNode tmp190_AST_in = (TNode)_t;
				match(_t,COLON);
				_t = _t.getNextSibling();
				{
				if (_t==null) _t=ASTNULL;
				switch ( _t.getType()) {
				case NStringSeq:
				{
					strOptExprPair(_t);
					_t = _retTree;
					{
					_loop179:
					do {
						if (_t==null) _t=ASTNULL;
						if ((_t.getType()==COMMA)) {
							TNode tmp191_AST_in = (TNode)_t;
							match(_t,COMMA);
							_t = _t.getNextSibling();
							strOptExprPair(_t);
							_t = _retTree;
						}
						else {
							break _loop179;
						}
						
					} while (true);
					}
					break;
				}
				case COLON:
				case RPAREN:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(_t);
				}
				}
				}
				{
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==COLON)) {
					TNode tmp192_AST_in = (TNode)_t;
					match(_t,COLON);
					_t = _t.getNextSibling();
					{
					if (_t==null) _t=ASTNULL;
					switch ( _t.getType()) {
					case NStringSeq:
					{
						strOptExprPair(_t);
						_t = _retTree;
						{
						_loop183:
						do {
							if (_t==null) _t=ASTNULL;
							if ((_t.getType()==COMMA)) {
								TNode tmp193_AST_in = (TNode)_t;
								match(_t,COMMA);
								_t = _t.getNextSibling();
								strOptExprPair(_t);
								_t = _retTree;
							}
							else {
								break _loop183;
							}
							
						} while (true);
						}
						break;
					}
					case COLON:
					case RPAREN:
					{
						break;
					}
					default:
					{
						throw new NoViableAltException(_t);
					}
					}
					}
				}
				else if ((_t.getType()==COLON||_t.getType()==RPAREN)) {
				}
				else {
					throw new NoViableAltException(_t);
				}
				
				}
			}
			else if ((_t.getType()==COLON||_t.getType()==RPAREN)) {
			}
			else {
				throw new NoViableAltException(_t);
			}
			
			}
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case COLON:
			{
				TNode tmp194_AST_in = (TNode)_t;
				match(_t,COLON);
				_t = _t.getNextSibling();
				stringConst(_t);
				_t = _retTree;
				{
				_loop186:
				do {
					if (_t==null) _t=ASTNULL;
					if ((_t.getType()==COMMA)) {
						TNode tmp195_AST_in = (TNode)_t;
						match(_t,COMMA);
						_t = _t.getNextSibling();
						stringConst(_t);
						_t = _retTree;
					}
					else {
						break _loop186;
					}
					
				} while (true);
				}
				break;
			}
			case RPAREN:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
			TNode tmp196_AST_in = (TNode)_t;
			match(_t,RPAREN);
			_t = _t.getNextSibling();
			_t = __t174;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	protected final void stringConst(AST _t) throws RecognitionException {
		
		TNode stringConst_AST_in = (TNode)_t;
		
		try {      // for error handling
			AST __t277 = _t;
			TNode tmp197_AST_in = (TNode)_t;
			match(_t,NStringSeq);
			_t = _t.getFirstChild();
			{
			int _cnt279=0;
			_loop279:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_t.getType()==StringLiteral)) {
					TNode tmp198_AST_in = (TNode)_t;
					match(_t,StringLiteral);
					_t = _t.getNextSibling();
				}
				else {
					if ( _cnt279>=1 ) { break _loop279; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt279++;
			} while (true);
			}
			_t = __t277;
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void strOptExprPair(AST _t) throws RecognitionException {
		
		TNode strOptExprPair_AST_in = (TNode)_t;
		
		try {      // for error handling
			stringConst(_t);
			_t = _retTree;
			{
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case LPAREN:
			{
				TNode tmp199_AST_in = (TNode)_t;
				match(_t,LPAREN);
				_t = _t.getNextSibling();
				expr(_t);
				_t = _retTree;
				TNode tmp200_AST_in = (TNode)_t;
				match(_t,RPAREN);
				_t = _t.getNextSibling();
				break;
			}
			case COMMA:
			case COLON:
			case RPAREN:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void unaryOperator(AST _t) throws RecognitionException {
		
		TNode unaryOperator_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case BAND:
			{
				TNode tmp201_AST_in = (TNode)_t;
				match(_t,BAND);
				_t = _t.getNextSibling();
				break;
			}
			case STAR:
			{
				TNode tmp202_AST_in = (TNode)_t;
				match(_t,STAR);
				_t = _t.getNextSibling();
				break;
			}
			case PLUS:
			{
				TNode tmp203_AST_in = (TNode)_t;
				match(_t,PLUS);
				_t = _t.getNextSibling();
				break;
			}
			case MINUS:
			{
				TNode tmp204_AST_in = (TNode)_t;
				match(_t,MINUS);
				_t = _t.getNextSibling();
				break;
			}
			case BNOT:
			{
				TNode tmp205_AST_in = (TNode)_t;
				match(_t,BNOT);
				_t = _t.getNextSibling();
				break;
			}
			case LNOT:
			{
				TNode tmp206_AST_in = (TNode)_t;
				match(_t,LNOT);
				_t = _t.getNextSibling();
				break;
			}
			case LAND:
			{
				TNode tmp207_AST_in = (TNode)_t;
				match(_t,LAND);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL___real:
			{
				TNode tmp208_AST_in = (TNode)_t;
				match(_t,LITERAL___real);
				_t = _t.getNextSibling();
				break;
			}
			case LITERAL___imag:
			{
				TNode tmp209_AST_in = (TNode)_t;
				match(_t,LITERAL___imag);
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	public final void argExprList(AST _t) throws RecognitionException {
		
		TNode argExprList_AST_in = (TNode)_t;
		
		try {      // for error handling
			{
			int _cnt274=0;
			_loop274:
			do {
				if (_t==null) _t=ASTNULL;
				if ((_tokenSet_3.member(_t.getType()))) {
					expr(_t);
					_t = _retTree;
				}
				else {
					if ( _cnt274>=1 ) { break _loop274; } else {throw new NoViableAltException(_t);}
				}
				
				_cnt274++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	protected final void charConst(AST _t) throws RecognitionException {
		
		TNode charConst_AST_in = (TNode)_t;
		
		try {      // for error handling
			TNode tmp210_AST_in = (TNode)_t;
			match(_t,CharLiteral);
			_t = _t.getNextSibling();
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	protected final void intConst(AST _t) throws RecognitionException {
		
		TNode intConst_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case IntOctalConst:
			{
				TNode tmp211_AST_in = (TNode)_t;
				match(_t,IntOctalConst);
				_t = _t.getNextSibling();
				break;
			}
			case LongOctalConst:
			{
				TNode tmp212_AST_in = (TNode)_t;
				match(_t,LongOctalConst);
				_t = _t.getNextSibling();
				break;
			}
			case UnsignedOctalConst:
			{
				TNode tmp213_AST_in = (TNode)_t;
				match(_t,UnsignedOctalConst);
				_t = _t.getNextSibling();
				break;
			}
			case IntIntConst:
			{
				TNode tmp214_AST_in = (TNode)_t;
				match(_t,IntIntConst);
				_t = _t.getNextSibling();
				break;
			}
			case LongIntConst:
			{
				TNode tmp215_AST_in = (TNode)_t;
				match(_t,LongIntConst);
				_t = _t.getNextSibling();
				break;
			}
			case UnsignedIntConst:
			{
				TNode tmp216_AST_in = (TNode)_t;
				match(_t,UnsignedIntConst);
				_t = _t.getNextSibling();
				break;
			}
			case IntHexConst:
			{
				TNode tmp217_AST_in = (TNode)_t;
				match(_t,IntHexConst);
				_t = _t.getNextSibling();
				break;
			}
			case LongHexConst:
			{
				TNode tmp218_AST_in = (TNode)_t;
				match(_t,LongHexConst);
				_t = _t.getNextSibling();
				break;
			}
			case UnsignedHexConst:
			{
				TNode tmp219_AST_in = (TNode)_t;
				match(_t,UnsignedHexConst);
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	protected final void floatConst(AST _t) throws RecognitionException {
		
		TNode floatConst_AST_in = (TNode)_t;
		
		try {      // for error handling
			if (_t==null) _t=ASTNULL;
			switch ( _t.getType()) {
			case FloatDoubleConst:
			{
				TNode tmp220_AST_in = (TNode)_t;
				match(_t,FloatDoubleConst);
				_t = _t.getNextSibling();
				break;
			}
			case DoubleDoubleConst:
			{
				TNode tmp221_AST_in = (TNode)_t;
				match(_t,DoubleDoubleConst);
				_t = _t.getNextSibling();
				break;
			}
			case LongDoubleConst:
			{
				TNode tmp222_AST_in = (TNode)_t;
				match(_t,LongDoubleConst);
				_t = _t.getNextSibling();
				break;
			}
			default:
			{
				throw new NoViableAltException(_t);
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				if (_t!=null) {_t = _t.getNextSibling();}
			} else {
			  throw ex;
			}
		}
		_retTree = _t;
	}
	
	
	public static final String[] _tokenNames = {
		"<0>",
		"EOF",
		"<2>",
		"NULL_TREE_LOOKAHEAD",
		"\"typedef\"",
		"\"asm\"",
		"\"volatile\"",
		"LCURLY",
		"RCURLY",
		"SEMI",
		"\"struct\"",
		"\"union\"",
		"\"enum\"",
		"\"auto\"",
		"\"register\"",
		"\"extern\"",
		"\"static\"",
		"\"const\"",
		"\"void\"",
		"\"char\"",
		"\"short\"",
		"\"int\"",
		"\"long\"",
		"\"float\"",
		"\"double\"",
		"\"signed\"",
		"\"unsigned\"",
		"ID",
		"COMMA",
		"COLON",
		"ASSIGN",
		"STAR",
		"LPAREN",
		"RPAREN",
		"LBRACKET",
		"RBRACKET",
		"VARARGS",
		"\"while\"",
		"\"do\"",
		"\"for\"",
		"\"goto\"",
		"\"continue\"",
		"\"break\"",
		"\"return\"",
		"\"case\"",
		"\"default\"",
		"\"if\"",
		"\"else\"",
		"\"switch\"",
		"DIV_ASSIGN",
		"PLUS_ASSIGN",
		"MINUS_ASSIGN",
		"STAR_ASSIGN",
		"MOD_ASSIGN",
		"RSHIFT_ASSIGN",
		"LSHIFT_ASSIGN",
		"BAND_ASSIGN",
		"BOR_ASSIGN",
		"BXOR_ASSIGN",
		"QUESTION",
		"LOR",
		"LAND",
		"BOR",
		"BXOR",
		"BAND",
		"EQUAL",
		"NOT_EQUAL",
		"LT",
		"LTE",
		"GT",
		"GTE",
		"LSHIFT",
		"RSHIFT",
		"PLUS",
		"MINUS",
		"DIV",
		"MOD",
		"INC",
		"DEC",
		"\"sizeof\"",
		"BNOT",
		"LNOT",
		"PTR",
		"DOT",
		"CharLiteral",
		"StringLiteral",
		"IntOctalConst",
		"LongOctalConst",
		"UnsignedOctalConst",
		"IntIntConst",
		"LongIntConst",
		"UnsignedIntConst",
		"IntHexConst",
		"LongHexConst",
		"UnsignedHexConst",
		"FloatDoubleConst",
		"DoubleDoubleConst",
		"LongDoubleConst",
		"NTypedefName",
		"NInitDecl",
		"NDeclarator",
		"NStructDeclarator",
		"NDeclaration",
		"NCast",
		"NPointerGroup",
		"NExpressionGroup",
		"NFunctionCallArgs",
		"NNonemptyAbstractDeclarator",
		"NInitializer",
		"NStatementExpr",
		"NEmptyExpression",
		"NParameterTypeList",
		"NFunctionDef",
		"NCompoundStatement",
		"NParameterDeclaration",
		"NCommaExpr",
		"NUnaryExpr",
		"NLabel",
		"NPostfixExpr",
		"NRangeExpr",
		"NStringSeq",
		"NInitializerElementLabel",
		"NLcurlyInitializer",
		"NAsmAttribute",
		"NGnuAsmExpr",
		"NTypeMissing",
		"Vocabulary",
		"Whitespace",
		"Comment",
		"CPPComment",
		"a line directive",
		"Space",
		"LineDirective",
		"BadStringLiteral",
		"Escape",
		"Digit",
		"LongSuffix",
		"UnsignedSuffix",
		"FloatSuffix",
		"Exponent",
		"Number",
		"\"__label__\"",
		"\"inline\"",
		"\"byte\"",
		"\"boolean\"",
		"\"Servo\"",
		"\"Wire\"",
		"\"typeof\"",
		"\"__complex\"",
		"\"__attribute\"",
		"\"__alignof\"",
		"\"__real\"",
		"\"__imag\""
	};
	
	private static final long[] mk_tokenSet_0() {
		long[] data = { 544L, 2306124759068311552L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_0 = new BitSet(mk_tokenSet_0());
	private static final long[] mk_tokenSet_1() {
		long[] data = { 134093888L, 17179869184L, 2064384L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_1 = new BitSet(mk_tokenSet_1());
	private static final long[] mk_tokenSet_2() {
		long[] data = { 422075026113024L, 9605333580251136L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_2 = new BitSet(mk_tokenSet_2());
	private static final long[] mk_tokenSet_3() {
		long[] data = { -562942303010816L, 1574098779476393983L, 4198400L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_3 = new BitSet(mk_tokenSet_3());
	}
	
