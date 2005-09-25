package processing.app.preproc;


import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;



public class CSymbolTable {

  /** holds list of scopes */
  private Vector scopeStack;

  /** table where all defined names are mapped to TNode tree nodes */
  private Hashtable symTable;

  public CSymbolTable()  {
    scopeStack = new Vector(10);
    symTable = new Hashtable(533);
  }


  /** push a new scope onto the scope stack.
    */
  public void pushScope(String s) {
      //System.out.println("push scope:" + s);
    scopeStack.addElement(s);
  }

  /** pop the last scope off the scope stack.
    */
  public void popScope() {
      //System.out.println("pop scope");
    int size = scopeStack.size();
    if(size > 0)
      scopeStack.removeElementAt(size - 1);
  }

  /** return the current scope as a string 
   */
  public String currentScopeAsString() {
      StringBuffer buf = new StringBuffer(100);
      boolean first = true;
      Enumeration e = scopeStack.elements();
      while(e.hasMoreElements()) {
        if(first) 
          first = false;
        else
          buf.append("::");
        buf.append(e.nextElement().toString());
      }
      return buf.toString();
  }

  /** given a name for a type, append it with the 
    current scope.
    */
  public String addCurrentScopeToName(String name) {
    String currScope = currentScopeAsString();
    return addScopeToName(currScope, name);
  }

  /** given a name for a type, append it with the 
    given scope.  MBZ
    */
  public String addScopeToName(String scope, String name) {
    if(scope == null || scope.length() > 0)
      return scope + "::" + name;
    else
      return name;
  }

  /** remove one level of scope from name MBZ*/
  public String removeOneLevelScope(String scopeName) {
    int index = scopeName.lastIndexOf("::");
    if (index > 0) {
      return scopeName.substring(0,index);
    }
    if (scopeName.length() > 0) {
        return "";
    }
    return null;
  }
  
  /** add a node to the table with it's key as
    the current scope and the name */
  public TNode add(String name, TNode node) {
    return (TNode)symTable.put(addCurrentScopeToName(name),node);
  }


  /** lookup a fully scoped name in the symbol table */
  public TNode lookupScopedName(String scopedName) {
    return (TNode)symTable.get(scopedName);
  }

  /** lookup an unscoped name in the table by prepending
    the current scope.
    MBZ -- if not found, pop scopes and look again
    */
  public TNode lookupNameInCurrentScope(String name) {
    String scope = currentScopeAsString();
    String scopedName;
    TNode tnode = null;

    //System.out.println( "\n"+ this.toString() );

    while (tnode == null && scope != null) {
      scopedName = addScopeToName(scope, name);
      //System.out.println("lookup trying " + scopedName);
      tnode = (TNode)symTable.get(scopedName);
      scope = removeOneLevelScope(scope);
    }
    return tnode;
  }

  /** convert this table to a string */
  public String toString() {
    StringBuffer buff = new StringBuffer(300);
    buff.append("CSymbolTable { \nCurrentScope: " + currentScopeAsString() + 
                "\nDefinedSymbols:\n");
    Enumeration ke = symTable.keys();
    Enumeration ve = symTable.elements();
    while(ke.hasMoreElements()) {
      buff.append(ke.nextElement().toString() + " (" + 
                  TNode.getNameForType(((TNode)ve.nextElement()).getType()) + ")\n");
    }
    buff.append("}\n");
    return buff.toString();
  }

};
