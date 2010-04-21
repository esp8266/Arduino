/* XMLElement.java                                                 NanoXML/Java
 *
 * This file is part of NanoXML 2 for Java.
 * Copyright (C) 2000-2002 Marc De Scheemaecker, All Rights Reserved.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software in
 *     a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 */

package processing.xml;

import java.io.*;
import java.util.*;

import processing.core.PApplet;


/**
 * XMLElement is a representation of an XML object. The object is able to parse XML code. The methods described here are the most basic. More are documented in the Developer's Reference.
 * <br><br>
 * The encoding parameter inside XML files is ignored, only UTF-8 (or plain ASCII) are parsed properly.
 * =advanced
 * XMLElement is an XML element. This is the base class used for the 
 * Processing XML library, representing a single node of an XML tree.
 * 
 * This code is based on a modified version of NanoXML by Marc De Scheemaecker.
 *
 * @author Marc De Scheemaecker
 * @author processing.org
 * 
 * @webref data:composite
 * @usage Web &amp; Application
 * @instanceName xml any variable of type XMLElement
 */
public class XMLElement implements Serializable {

    /**
     * No line number defined.
     */
    public static final int NO_LINE = -1;


    /**
     * The parent element.
     */
    private XMLElement parent;


    /**
     * The attributes of the element.
     */
    private Vector<XMLAttribute> attributes;


    /**
     * The child elements.
     */
    private Vector<XMLElement> children;


    /**
     * The name of the element.
     */
    private String name;


    /**
     * The full name of the element.
     */
    private String fullName;


    /**
     * The namespace URI.
     */
    private String namespace;


    /**
     * The content of the element.
     */
    private String content;


    /**
     * The system ID of the source data where this element is located.
     */
    private String systemID;


    /**
     * The line in the source data where this element starts.
     */
    private int lineNr;


    /**
     * Creates an empty element to be used for #PCDATA content.
     * @nowebref
     */
    public XMLElement() {
        this(null, null, null, NO_LINE);
    }


    protected void set(String fullName,
                           String namespace,
                           String systemID,
                           int lineNr) {
        this.fullName = fullName;
        if (namespace == null) {
                this.name = fullName;
        } else {
                int index = fullName.indexOf(':');
                if (index >= 0) {
                        this.name = fullName.substring(index + 1);
                } else {
                        this.name = fullName;
                }
        }
        this.namespace = namespace;
        this.lineNr = lineNr;
        this.systemID = systemID;
    }


    /**
     * Creates an empty element.
     *
     * @param fullName the name of the element.
     */
//    public XMLElement(String fullName) {
//        this(fullName, null, null, NO_LINE);
//    }


    /**
     * Creates an empty element.
     *
     * @param fullName the name of the element.
     * @param systemID the system ID of the XML data where the element starts.
     * @param lineNr   the line in the XML data where the element starts.
     */
//    public XMLElement(String fullName,
//                      String systemID,
//                      int    lineNr) {
//        this(fullName, null, systemID, lineNr);
//    }


    /**
     * Creates an empty element.
     *
     * @param fullName  the full name of the element
     * @param namespace the namespace URI.
     */
//    public XMLElement(String fullName,
//                      String namespace) {
//        this(fullName, namespace, null, NO_LINE);
//    }


    /**
     * Creates an empty element.
     *
     * @param fullName  the full name of the element
     * @param namespace the namespace URI.
     * @param systemID  the system ID of the XML data where the element starts.
     * @param lineNr    the line in the XML data where the element starts.
     * @nowebref
     */
    public XMLElement(String fullName,
                      String namespace,
                      String systemID,
                      int    lineNr) {
        this.attributes = new Vector<XMLAttribute>();
        this.children = new Vector<XMLElement>(8);
        this.fullName = fullName;
        if (namespace == null) {
            this.name = fullName;
        } else {
            int index = fullName.indexOf(':');
            if (index >= 0) {
                this.name = fullName.substring(index + 1);
            } else {
                this.name = fullName;
            }
        }
        this.namespace = namespace;
        this.content = null;
        this.lineNr = lineNr;
        this.systemID = systemID;
        this.parent = null;
    }


    /**
     * Begin parsing XML data passed in from a PApplet. This code
     * wraps exception handling, for more advanced exception handling,
     * use the constructor that takes a Reader or InputStream.
     * @author processing.org
     * @param filename name of the XML file to load
     * @param parent typically use "this"
     */
    public XMLElement(PApplet parent, String filename) {
        this();
        parseFromReader(parent.createReader(filename));
    }

    /**
     * @nowebref
     */
    public XMLElement(Reader r) {
        this();
        parseFromReader(r);
    }

    /**
     * @nowebref
     */
    public XMLElement(String xml) {
        this();
        parseFromReader(new StringReader(xml));
    }


    protected void parseFromReader(Reader r) {
        try {
            StdXMLParser parser = new StdXMLParser();
            parser.setBuilder(new StdXMLBuilder(this));
            parser.setValidator(new XMLValidator());
            parser.setReader(new StdXMLReader(r));
            //System.out.println(parser.parse().getName());
            /*XMLElement xm = (XMLElement)*/ parser.parse();
            //System.out.println("xm name is " + xm.getName());
            //System.out.println(xm + " " + this);
            //parser.parse();
        } catch (XMLException e) {
            e.printStackTrace();
        }
    }


//    static public XMLElement parse(Reader r) {
//        try {
//              StdXMLParser parser = new StdXMLParser();
//              parser.setBuilder(new StdXMLBuilder());
//              parser.setValidator(new XMLValidator());
//              parser.setReader(new StdXMLReader(r));
//              return (XMLElement) parser.parse();
//              } catch (XMLException e) {
//                      e.printStackTrace();
//                      return null;
//              }
//    }


    /**
     * Creates an element to be used for #PCDATA content.
     */
    public XMLElement createPCDataElement() {
        return new XMLElement();
    }


    /**
     * Creates an empty element.
     *
     * @param fullName the name of the element.
     */
//    public XMLElement createElement(String fullName) {
//        return new XMLElement(fullName);
//    }


    /**
     * Creates an empty element.
     *
     * @param fullName the name of the element.
     * @param systemID the system ID of the XML data where the element starts.
     * @param lineNr   the line in the XML data where the element starts.
     */
//    public XMLElement createElement(String fullName,
//                                    String systemID,
//                                    int lineNr) {
//        //return new XMLElement(fullName, systemID, lineNr);
//      return new XMLElement(fullName, null, systemID, lineNr);
//    }


    /**
     * Creates an empty element.
     *
     * @param fullName  the full name of the element
     * @param namespace the namespace URI.
     */
    public XMLElement createElement(String fullName,
                                    String namespace) {
        //return new XMLElement(fullName, namespace);
        return new XMLElement(fullName, namespace, null, NO_LINE);
    }


    /**
     * Creates an empty element.
     *
     * @param fullName  the full name of the element
     * @param namespace the namespace URI.
     * @param systemID  the system ID of the XML data where the element starts.
     * @param lineNr    the line in the XML data where the element starts.
     */
    public XMLElement createElement(String fullName,
                                     String namespace,
                                     String systemID,
                                     int    lineNr) {
        return new XMLElement(fullName, namespace, systemID, lineNr);
    }


    /**
     * Cleans up the object when it's destroyed.
     */
    protected void finalize() throws Throwable {
        this.attributes.clear();
        this.attributes = null;
        this.children = null;
        this.fullName = null;
        this.name = null;
        this.namespace = null;
        this.content = null;
        this.systemID = null;
        this.parent = null;
        super.finalize();
    }


    /**
     * Returns the parent element. This method returns null for the root
     * element.
     */
    public XMLElement getParent() {
        return this.parent;
    }


    /**
     * Returns the full name (i.e. the name including an eventual namespace
     * prefix) of the element.
     *
     * @webref
     * @brief  	Returns the name of the element.
     * @return the name, or null if the element only contains #PCDATA.
     */
    public String getName() {
        return this.fullName;
    }


    /**
     * Returns the name of the element.
     *
     * @return the name, or null if the element only contains #PCDATA.
     */
    public String getLocalName() {
        return this.name;
    }


    /**
     * Returns the namespace of the element.
     *
     * @return the namespace, or null if no namespace is associated with the
     *         element.
     */
    public String getNamespace() {
        return this.namespace;
    }


    /**
     * Sets the full name. This method also sets the short name and clears the
     * namespace URI.
     *
     * @param name the non-null name.
     */
    public void setName(String name) {
        this.name = name;
        this.fullName = name;
        this.namespace = null;
    }


    /**
     * Sets the name.
     *
     * @param fullName  the non-null full name.
     * @param namespace the namespace URI, which may be null.
     */
    public void setName(String fullName, String namespace) {
        int index = fullName.indexOf(':');
        if ((namespace == null) || (index < 0)) {
            this.name = fullName;
        } else {
            this.name = fullName.substring(index + 1);
        }
        this.fullName = fullName;
        this.namespace = namespace;
    }


    /**
     * Adds a child element.
     *
     * @param child the non-null child to add.
     */
    public void addChild(XMLElement child) {
        if (child == null) {
            throw new IllegalArgumentException("child must not be null");
        }
        if ((child.getLocalName() == null) && (! this.children.isEmpty())) {
            XMLElement lastChild = (XMLElement) this.children.lastElement();

            if (lastChild.getLocalName() == null) {
                lastChild.setContent(lastChild.getContent()
                                     + child.getContent());
                return;
            }
        }
        ((XMLElement)child).parent = this;
        this.children.addElement(child);
    }


    /**
     * Inserts a child element.
     *
     * @param child the non-null child to add.
     * @param index where to put the child.
     */
    public void insertChild(XMLElement child, int index) {
        if (child == null) {
            throw new IllegalArgumentException("child must not be null");
        }
        if ((child.getLocalName() == null) && (! this.children.isEmpty())) {
            XMLElement lastChild = (XMLElement) this.children.lastElement();
            if (lastChild.getLocalName() == null) {
                lastChild.setContent(lastChild.getContent()
                                     + child.getContent());
                return;
            }
        }
        ((XMLElement) child).parent = this;
        this.children.insertElementAt(child, index);
    }


    /**
     * Removes a child element.
     *
     * @param child the non-null child to remove.
     */
    public void removeChild(XMLElement child) {
        if (child == null) {
            throw new IllegalArgumentException("child must not be null");
        }
        this.children.removeElement(child);
    }


    /**
     * Removes the child located at a certain index.
     *
     * @param index the index of the child, where the first child has index 0.
     */
    public void removeChildAtIndex(int index) {
        this.children.removeElementAt(index);
    }


    /**
     * Returns an enumeration of all child elements.
     *
     * @return the non-null enumeration
     */
    public Enumeration<XMLElement> enumerateChildren() {
        return this.children.elements();
    }


    /**
     * Returns whether the element is a leaf element.
     *
     * @return true if the element has no children.
     */
    public boolean isLeaf() {
        return this.children.isEmpty();
    }


    /**
     * Returns whether the element has children.
     *
     * @return true if the element has children.
     */
    public boolean hasChildren() {
        return (! this.children.isEmpty());
    }


    /**
     * Returns the number of children for the element.
     *
     * @return the count.
     * @webref
     * @see processing.xml.XMLElement#getChild(int)
     * @see processing.xml.XMLElement#getChildren(String)
     */
    public int getChildCount() {
        return this.children.size();
    }


    /**
     * Returns a vector containing all the child elements.
     *
     * @return the vector.
     */
//    public Vector getChildren() {
//        return this.children;
//    }


    /**
     * Put the names of all children into an array. Same as looping through 
     * each child and calling getName() on each XMLElement.
     */
    public String[] listChildren() {
      int childCount = getChildCount();
      String[] outgoing = new String[childCount];
      for (int i = 0; i < childCount; i++) {
        outgoing[i] = getChild(i).getName();
      }
      return outgoing;
    }
    
    
    /**
     * Returns an array containing all the child elements.
     */
    public XMLElement[] getChildren() {
        int childCount = getChildCount();
        XMLElement[] kids = new XMLElement[childCount];
        children.copyInto(kids);
        return kids;
    }


    /**
     * Quick accessor for an element at a particular index.
     * @author processing.org
     * @param index the element
     */
    public XMLElement getChild(int index) {
        return (XMLElement) children.elementAt(index);
    }


    /**
     * Returns the child XMLElement as specified by the <b>index</b> parameter. The value of the <b>index</b> parameter must be less than the total number of children to avoid going out of the array storing the child elements.
     * When the <b>path</b> parameter is specified, then it will return all children that match that path. The path is a series of elements and sub-elements, separated by slashes.
     * 
     * @return the element
     * @author processing.org
     * 
     * @webref
     * @see processing.xml.XMLElement#getChildCount()
     * @see processing.xml.XMLElement#getChildren(String)
     * @brief Get a child by its name or path.
     * @param path path to a particular element
     */
    public XMLElement getChild(String path) {
        if (path.indexOf('/') != -1) {
            return getChildRecursive(PApplet.split(path, '/'), 0);
        }
        int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            XMLElement kid = getChild(i);
            String kidName = kid.getName();
            if (kidName != null && kidName.equals(path)) {
                return kid;
            }
        }
        return null;
    }


    /**
     * Internal helper function for getChild(String).
     * @param items result of splitting the query on slashes
     * @param offset where in the items[] array we're currently looking
     * @return matching element or null if no match
     * @author processing.org
     */
    protected XMLElement getChildRecursive(String[] items, int offset) {
        // if it's a number, do an index instead
        if (Character.isDigit(items[offset].charAt(0))) {
            XMLElement kid = getChild(Integer.parseInt(items[offset]));
            if (offset == items.length-1) {
                return kid;
            } else {
                return kid.getChildRecursive(items, offset+1);
            }
        }
        int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            XMLElement kid = getChild(i);
            String kidName = kid.getName();
            if (kidName != null && kidName.equals(items[offset])) {
                if (offset == items.length-1) {
                    return kid;
                } else {
                    return kid.getChildRecursive(items, offset+1);
                }
            }
        }
        return null;
    }


    /**
     * Returns the child at a specific index.
     *
     * @param index the index of the child
     *
     * @return the non-null child
     *
     * @throws java.lang.ArrayIndexOutOfBoundsException
     *          if the index is out of bounds.
     */
    public XMLElement getChildAtIndex(int index)
    throws ArrayIndexOutOfBoundsException {
        return (XMLElement) this.children.elementAt(index);
    }


    /**
     * Searches a child element.
     *
     * @param name the full name of the child to search for.
     *
     * @return the child element, or null if no such child was found.
     */
//    public XMLElement getFirstChildNamed(String name) {
//        Enumeration enum = this.children.elements();
//        while (enum.hasMoreElements()) {
//            XMLElement child = (XMLElement) enum.nextElement();
//            String childName = child.getFullName();
//            if ((childName != null) && childName.equals(name)) {
//                return child;
//            }
//        }
//        return null;
//    }


    /**
     * Searches a child element.
     *
     * @param name      the name of the child to search for.
     * @param namespace the namespace, which may be null.
     *
     * @return the child element, or null if no such child was found.
     */
//    public XMLElement getFirstChildNamed(String name,
//                                          String namespace) {
//        Enumeration enum = this.children.elements();
//        while (enum.hasMoreElements()) {
//            XMLElement child = (XMLElement) enum.nextElement();
//            String str = child.getName();
//            boolean found = (str != null) && (str.equals(name));
//            str = child.getNamespace();
//            if (str == null) {
//                found &= (name == null);
//            } else {
//                found &= str.equals(namespace);
//            }
//            if (found) {
//                return child;
//            }
//        }
//        return null;
//    }


    /**
     * Returns all of the children as an XMLElement array.
     * When the <b>path</b> parameter is specified, then it will return all children that match that path.
     * The path is a series of elements and sub-elements, separated by slashes.
     * 
     * @param path element name or path/to/element
     * @return array of child elements that match
     * @author processing.org
     * 
     * @webref
     * @brief Returns all of the children as an XMLElement array.
     * @see processing.xml.XMLElement#getChildCount()
     * @see processing.xml.XMLElement#getChild(int)
     */
    public XMLElement[] getChildren(String path) {
        if (path.indexOf('/') != -1) {
        	return getChildrenRecursive(PApplet.split(path, '/'), 0);
        }
        // if it's a number, do an index instead
        // (returns a single element array, since this will be a single match
        if (Character.isDigit(path.charAt(0))) {
        	return new XMLElement[] { getChild(Integer.parseInt(path)) };
        }
        int childCount = getChildCount();
        XMLElement[] matches = new XMLElement[childCount];
        int matchCount = 0;
        for (int i = 0; i < childCount; i++) {
        	XMLElement kid = getChild(i);
        	String kidName = kid.getName();
        	if (kidName != null && kidName.equals(path)) {
        		matches[matchCount++] = kid;
        	}
        }
        return (XMLElement[]) PApplet.subset(matches, 0, matchCount);
    }


    protected XMLElement[] getChildrenRecursive(String[] items, int offset) {
    	if (offset == items.length-1) {
    		return getChildren(items[offset]);
    	}
    	XMLElement[] matches = getChildren(items[offset]);
    	XMLElement[] outgoing = new XMLElement[0];
    	for (int i = 0; i < matches.length; i++) {
    		XMLElement[] kidMatches = matches[i].getChildrenRecursive(items, offset+1);
    		outgoing = (XMLElement[]) PApplet.concat(outgoing, kidMatches);
    	}
    	return outgoing;
    }


    /**
     * Returns a vector of all child elements named <I>name</I>.
     *
     * @param name the full name of the children to search for.
     *
     * @return the non-null vector of child elements.
     */
//    public Vector getChildrenNamed(String name) {
//        Vector result = new Vector(this.children.size());
//        Enumeration enum = this.children.elements();
//        while (enum.hasMoreElements()) {
//            XMLElement child = (XMLElement) enum.nextElement();
//            String childName = child.getFullName();
//            if ((childName != null) && childName.equals(name)) {
//                result.addElement(child);
//            }
//        }
//        return result;
//    }


    /**
     * Returns a vector of all child elements named <I>name</I>.
     *
     * @param name      the name of the children to search for.
     * @param namespace the namespace, which may be null.
     *
     * @return the non-null vector of child elements.
     */
//    public Vector getChildrenNamed(String name,
//                                   String namespace) {
//        Vector result = new Vector(this.children.size());
//        Enumeration enum = this.children.elements();
//        while (enum.hasMoreElements()) {
//            XMLElement child = (XMLElement) enum.nextElement();
//            String str = child.getName();
//            boolean found = (str != null) && (str.equals(name));
//            str = child.getNamespace();
//            if (str == null) {
//                found &= (name == null);
//            } else {
//                found &= str.equals(namespace);
//            }
//
//            if (found) {
//                result.addElement(child);
//            }
//        }
//        return result;
//    }


    /**
     * Searches an attribute.
     *
     * @param fullName the non-null full name of the attribute.
     *
     * @return the attribute, or null if the attribute does not exist.
     */
    private XMLAttribute findAttribute(String fullName) {
        Enumeration<XMLAttribute> en = this.attributes.elements();
        while (en.hasMoreElements()) {
            XMLAttribute attr = (XMLAttribute) en.nextElement();
            if (attr.getFullName().equals(fullName)) {
                return attr;
            }
        }
        return null;
    }


    /**
     * Searches an attribute.
     *
     * @param name the non-null short name of the attribute.
     * @param namespace the name space, which may be null.
     *
     * @return the attribute, or null if the attribute does not exist.
     */
    private XMLAttribute findAttribute(String name,
                                       String namespace) {
        Enumeration<XMLAttribute> en = this.attributes.elements();
        while (en.hasMoreElements()) {
            XMLAttribute attr = (XMLAttribute) en.nextElement();
            boolean found = attr.getName().equals(name);
            if (namespace == null) {
                found &= (attr.getNamespace() == null);
            } else {
                found &= namespace.equals(attr.getNamespace());
            }

            if (found) {
                return attr;
            }
        }
        return null;
    }


    /**
     * Returns the number of attributes.
     */
    public int getAttributeCount() {
        return this.attributes.size();
    }


    /**
     * Returns the value of an attribute.
     *
     * @param name the non-null name of the attribute.
     *
     * @return the value, or null if the attribute does not exist.
     */
    public String getAttribute(String name) {
        return this.getAttribute(name, null);
    }


    /**
     * Returns the value of an attribute.
     *
     * @param name the non-null full name of the attribute.
     * @param defaultValue the default value of the attribute.
     *
     * @return the value, or defaultValue if the attribute does not exist.
     */
    public String getAttribute(String name,
                               String defaultValue) {
        XMLAttribute attr = this.findAttribute(name);
        if (attr == null) {
            return defaultValue;
        } else {
            return attr.getValue();
        }
    }


    /**
     * Returns the value of an attribute.
     *
     * @param name the non-null name of the attribute.
     * @param namespace the namespace URI, which may be null.
     * @param defaultValue the default value of the attribute.
     *
     * @return the value, or defaultValue if the attribute does not exist.
     */
    public String getAttribute(String name,
                               String namespace,
                               String defaultValue) {
        XMLAttribute attr = this.findAttribute(name, namespace);
        if (attr == null) {
            return defaultValue;
        } else {
            return attr.getValue();
        }
    }

    
    public String getStringAttribute(String name) {
        return getAttribute(name);
    }

    /**
     * Returns a String attribute of the element.
     * If the <b>default</b> parameter is used and the attribute doesn't exist, the <b>default</b> value is returned.
     * When using the version of the method without the <b>default</b> parameter, if the attribute doesn't exist, the value 0 is returned.
     * 
     * @webref
     * @param name the name of the attribute
     * @param default Value value returned if the attribute is not found
     * 
     * @brief Returns a String attribute of the element.
     */
    public String getStringAttribute(String name, String defaultValue) {
        return getAttribute(name, defaultValue);
    }


    public String getStringAttribute(String name,
                                     String namespace,
                                     String defaultValue) {
        return getAttribute(name, namespace, defaultValue);
    }

    /**
     * Returns an integer attribute of the element.
     */
    public int getIntAttribute(String name) {
        return getIntAttribute(name, 0);
    }


    /**
     * Returns an integer attribute of the element.
     * If the <b>default</b> parameter is used and the attribute doesn't exist, the <b>default</b> value is returned.
     * When using the version of the method without the <b>default</b> parameter, if the attribute doesn't exist, the value 0 is returned.
     *
     * @param name the name of the attribute
     * @param defaultValue value returned if the attribute is not found
     *
     * @webref
     * @brief Returns an integer attribute of the element.
     * @return the value, or defaultValue if the attribute does not exist.
     */
    public int getIntAttribute(String name,
                               int defaultValue) {
        String value = this.getAttribute(name, Integer.toString(defaultValue));
        return Integer.parseInt(value);
    }


    /**
     * Returns the value of an attribute.
     *
     * @param name the non-null name of the attribute.
     * @param namespace the namespace URI, which may be null.
     * @param defaultValue the default value of the attribute.
     *
     * @return the value, or defaultValue if the attribute does not exist.
     */
    public int getIntAttribute(String name,
                               String namespace,
                               int defaultValue) {
        String value = this.getAttribute(name, namespace,
                                         Integer.toString(defaultValue));
        return Integer.parseInt(value);
    }


    public float getFloatAttribute(String name) {
        return getFloatAttribute(name, 0);
    }


    /**
     * Returns a float attribute of the element.
     * If the <b>default</b> parameter is used and the attribute doesn't exist, the <b>default</b> value is returned.
     * When using the version of the method without the <b>default</b> parameter, if the attribute doesn't exist, the value 0 is returned.
     *
     * @param name the name of the attribute
     * @param defaultValue value returned if the attribute is not found
     *
     * @return the value, or defaultValue if the attribute does not exist.
     * 
     * @webref
     * @brief Returns a float attribute of the element.
     */
    public float getFloatAttribute(String name,
                                   float defaultValue) {
        String value = this.getAttribute(name, Float.toString(defaultValue));
        return Float.parseFloat(value);
    }


    /**
     * Returns the value of an attribute.
     *
     * @param name the non-null name of the attribute.
     * @param namespace the namespace URI, which may be null.
     * @param defaultValue the default value of the attribute.
     *
     * @return the value, or defaultValue if the attribute does not exist.
     * @nowebref
     */
    public float getFloatAttribute(String name,
                                   String namespace,
                                   float defaultValue) {
        String value = this.getAttribute(name, namespace,
                                         Float.toString(defaultValue));
        return Float.parseFloat(value);
    }


    public double getDoubleAttribute(String name) {
        return getDoubleAttribute(name, 0);
    }


    /**
     * Returns the value of an attribute.
     *
     * @param name the non-null full name of the attribute.
     * @param defaultValue the default value of the attribute.
     *
     * @return the value, or defaultValue if the attribute does not exist.
     */
    public double getDoubleAttribute(String name,
                                     double defaultValue) {
        String value = this.getAttribute(name, Double.toString(defaultValue));
        return Double.parseDouble(value);
    }


    /**
     * Returns the value of an attribute.
     *
     * @param name the non-null name of the attribute.
     * @param namespace the namespace URI, which may be null.
     * @param defaultValue the default value of the attribute.
     *
     * @return the value, or defaultValue if the attribute does not exist.
     */
    public double getDoubleAttribute(String name,
                                     String namespace,
                                     double defaultValue) {
        String value = this.getAttribute(name, namespace,
                                         Double.toString(defaultValue));
        return Double.parseDouble(value);
    }


    /**
     * Returns the type of an attribute.
     *
     * @param name the non-null full name of the attribute.
     *
     * @return the type, or null if the attribute does not exist.
     */
    public String getAttributeType(String name) {
        XMLAttribute attr = this.findAttribute(name);
        if (attr == null) {
            return null;
        } else {
            return attr.getType();
        }
    }


    /**
     * Returns the namespace of an attribute.
     *
     * @param name the non-null full name of the attribute.
     *
     * @return the namespace, or null if there is none associated.
     */
    public String getAttributeNamespace(String name) {
        XMLAttribute attr = this.findAttribute(name);
        if (attr == null) {
            return null;
        } else {
            return attr.getNamespace();
        }
    }


    /**
     * Returns the type of an attribute.
     *
     * @param name the non-null name of the attribute.
     * @param namespace the namespace URI, which may be null.
     *
     * @return the type, or null if the attribute does not exist.
     */
    public String getAttributeType(String name,
                                   String namespace) {
        XMLAttribute attr = this.findAttribute(name, namespace);
        if (attr == null) {
            return null;
        } else {
            return attr.getType();
        }
    }


    /**
     * Sets an attribute.
     *
     * @param name the non-null full name of the attribute.
     * @param value the non-null value of the attribute.
     */
    public void setAttribute(String name,
                             String value) {
        XMLAttribute attr = this.findAttribute(name);
        if (attr == null) {
            attr = new XMLAttribute(name, name, null, value, "CDATA");
            this.attributes.addElement(attr);
        } else {
            attr.setValue(value);
        }
    }


    /**
     * Sets an attribute.
     *
     * @param fullName the non-null full name of the attribute.
     * @param namespace the namespace URI of the attribute, which may be null.
     * @param value the non-null value of the attribute.
     */
    public void setAttribute(String fullName,
                             String namespace,
                             String value) {
        int index = fullName.indexOf(':');
        String vorname = fullName.substring(index + 1);
        XMLAttribute attr = this.findAttribute(vorname, namespace);
        if (attr == null) {
            attr = new XMLAttribute(fullName, vorname, namespace, value, "CDATA");
            this.attributes.addElement(attr);
        } else {
            attr.setValue(value);
        }
    }


    /**
     * Removes an attribute.
     *
     * @param name the non-null name of the attribute.
     */
    public void removeAttribute(String name) {
        for (int i = 0; i < this.attributes.size(); i++) {
            XMLAttribute attr = (XMLAttribute) this.attributes.elementAt(i);
            if (attr.getFullName().equals(name)) {
                this.attributes.removeElementAt(i);
                return;
            }
        }
    }


    /**
     * Removes an attribute.
     *
     * @param name the non-null name of the attribute.
     * @param namespace the namespace URI of the attribute, which may be null.
     */
    public void removeAttribute(String name,
                                String namespace) {
        for (int i = 0; i < this.attributes.size(); i++) {
            XMLAttribute attr = (XMLAttribute) this.attributes.elementAt(i);
            boolean found = attr.getName().equals(name);
            if (namespace == null) {
                found &= (attr.getNamespace() == null);
            } else {
                found &= attr.getNamespace().equals(namespace);
            }

            if (found) {
                this.attributes.removeElementAt(i);
                return;
            }
        }
    }


    /**
     * Returns an enumeration of all attribute names.
     *
     * @return the non-null enumeration.
     */
    public Enumeration<String> enumerateAttributeNames() {
        Vector<String> result = new Vector<String>();
        Enumeration<XMLAttribute> en = this.attributes.elements();
        while (en.hasMoreElements()) {
            XMLAttribute attr = (XMLAttribute) en.nextElement();
            result.addElement(attr.getFullName());
        }
        return result.elements();
    }


    /**
     * Returns whether an attribute exists.
     *
     * @return true if the attribute exists.
     */
    public boolean hasAttribute(String name) {
        return this.findAttribute(name) != null;
    }


    /**
     * Returns whether an attribute exists.
     *
     * @return true if the attribute exists.
     */
    public boolean hasAttribute(String name,
                                String namespace) {
        return this.findAttribute(name, namespace) != null;
    }


    /**
     * Returns all attributes as a Properties object.
     *
     * @return the non-null set.
     */
    public Properties getAttributes() {
        Properties result = new Properties();
        Enumeration<XMLAttribute> en = this.attributes.elements();
        while (en.hasMoreElements()) {
            XMLAttribute attr = (XMLAttribute) en.nextElement();
            result.put(attr.getFullName(), attr.getValue());
        }
        return result;
    }


    /**
     * Returns all attributes in a specific namespace as a Properties object.
     *
     * @param namespace the namespace URI of the attributes, which may be null.
     *
     * @return the non-null set.
     */
    public Properties getAttributesInNamespace(String namespace) {
        Properties result = new Properties();
        Enumeration<XMLAttribute> en = this.attributes.elements();
        while (en.hasMoreElements()) {
            XMLAttribute attr = (XMLAttribute) en.nextElement();
            if (namespace == null) {
                if (attr.getNamespace() == null) {
                    result.put(attr.getName(), attr.getValue());
                }
            } else {
                if (namespace.equals(attr.getNamespace())) {
                    result.put(attr.getName(), attr.getValue());
                }
            }
        }
        return result;
    }


    /**
     * Returns the system ID of the data where the element started.
     *
     * @return the system ID, or null if unknown.
     *
     * @see #getLineNr
     */
    public String getSystemID() {
        return this.systemID;
    }


    /**
     * Returns the line number in the data where the element started.
     *
     * @return the line number, or NO_LINE if unknown.
     *
     * @see #NO_LINE
     * @see #getSystemID
     */
    public int getLineNr() {
        return this.lineNr;
    }


    /**
     * Returns the content of an element. If there is no such content, <b>null</b> is returned.
     * =advanced
     * Return the #PCDATA content of the element. If the element has a
     * combination of #PCDATA content and child elements, the #PCDATA
     * sections can be retrieved as unnamed child objects. In this case,
     * this method returns null.
     *
     * @webref 
     * @brief  	Returns the content of an element
     * @return the content.
     */
    public String getContent() {
        return this.content;
    }


    /**
     * Sets the #PCDATA content. It is an error to call this method with a
     * non-null value if there are child objects.
     *
     * @param content the (possibly null) content.
     */
    public void setContent(String content) {
        this.content = content;
    }


    /**
     * Returns true if the element equals another element.
     *
     * @param rawElement the element to compare to
     */
    public boolean equals(Object rawElement) {
        try {
            return this.equalsXMLElement((XMLElement) rawElement);
        } catch (ClassCastException e) {
            return false;
        }
    }


    /**
     * Returns true if the element equals another element.
     *
     * @param rawElement the element to compare to
     */
    public boolean equalsXMLElement(XMLElement rawElement) {
        if (! this.name.equals(rawElement.getLocalName())) {
            return false;
        }
        if (this.attributes.size() != rawElement.getAttributeCount()) {
            return false;
        }
        Enumeration<XMLAttribute> en = this.attributes.elements();
        while (en.hasMoreElements()) {
            XMLAttribute attr = (XMLAttribute) en.nextElement();
            if (! rawElement.hasAttribute(attr.getName(), attr.getNamespace())) {
                return false;
            }
            String value = rawElement.getAttribute(attr.getName(),
                                            attr.getNamespace(),
                                            null);
            if (! attr.getValue().equals(value)) {
                return false;
            }
            String type = rawElement.getAttributeType(attr.getName(),
                                               attr.getNamespace());
            if (! attr.getType().equals(type)) {
                return false;
            }
        }
        if (this.children.size() != rawElement.getChildCount()) {
            return false;
        }
        for (int i = 0; i < this.children.size(); i++) {
            XMLElement child1 = this.getChildAtIndex(i);
            XMLElement child2 = rawElement.getChildAtIndex(i);

            if (! child1.equalsXMLElement(child2)) {
                return false;
            }
        }
        return true;
    }


    public String toString() {
      return toString(true);
    }
    
    
    public String toString(boolean pretty) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        OutputStreamWriter osw = new OutputStreamWriter(baos);
        XMLWriter writer = new XMLWriter(osw);
        try {
          if (pretty) {
            writer.write(this, true, 2, true);
          } else {
            writer.write(this, false, 0, true);
          }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return baos.toString();
    }
}
