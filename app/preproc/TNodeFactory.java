package processing.app.preproc;

import antlr.Token;
import antlr.ASTFactory;
import antlr.collections.AST;

/** This class extends ASTFactory to build instances
 of class TNode */
public class TNodeFactory extends ASTFactory {

  /** Create a new ampty AST node */
  public AST create() { 
    return new TNode(); 
  }

        /** Create a new AST node from type and text */
        public AST create(int ttype, String text) {
                AST ast = new TNode();
                ast.setType(ttype);
                ast.setText(text);
                return ast;
        }

        /** Create a new AST node from an existing AST node */
        public AST create(AST ast) {
                AST newast = new TNode();
                newast.setType(ast.getType());
                newast.setText(ast.getText());
                return newast;
        }

  
}
