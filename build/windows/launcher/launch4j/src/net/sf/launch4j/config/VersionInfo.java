/*
	Launch4j (http://launch4j.sourceforge.net/)
	Cross-platform Java application wrapper for creating Windows native executables.

	Copyright (c) 2004, 2007 Grzegorz Kowal

	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	    * Redistributions of source code must retain the above copyright notice,
	      this list of conditions and the following disclaimer.
	    * Redistributions in binary form must reproduce the above copyright notice,
	      this list of conditions and the following disclaimer in the documentation
	      and/or other materials provided with the distribution.
	    * Neither the name of the Launch4j nor the names of its contributors
	      may be used to endorse or promote products derived from this software without
	      specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
	PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * Created on May 21, 2005
 */
package net.sf.launch4j.config;

import net.sf.launch4j.binding.IValidatable;
import net.sf.launch4j.binding.Validator;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class VersionInfo implements IValidatable {
	public static final String VERSION_PATTERN = "(\\d+\\.){3}\\d+";

	private String fileVersion;
	private String txtFileVersion;
	private String fileDescription;
	private String copyright;
	private String productVersion;
	private String txtProductVersion;
	private String productName;
	private String companyName;
	private String internalName;
	private String originalFilename;

	public void checkInvariants() {
		Validator.checkString(fileVersion, 20, VERSION_PATTERN,
				"versionInfo.fileVersion",
				Messages.getString("VersionInfo.file.version"));
		Validator.checkString(txtFileVersion, 50, "versionInfo.txtFileVersion",
				Messages.getString("VersionInfo.txt.file.version"));
		Validator.checkString(fileDescription, 150, "versionInfo.fileDescription",
				Messages.getString("VersionInfo.file.description"));
		Validator.checkString(copyright, 150, "versionInfo.copyright",
				Messages.getString("VersionInfo.copyright"));
		Validator.checkString(productVersion, 20, VERSION_PATTERN,
				"versionInfo.productVersion",
				Messages.getString("VersionInfo.product.version"));
		Validator.checkString(txtProductVersion, 50, "versionInfo.txtProductVersion",
				Messages.getString("VersionInfo.txt.product.version"));
		Validator.checkString(productName, 150, "versionInfo.productName",
				Messages.getString("VersionInfo.product.name"));
		Validator.checkOptString(companyName, 150, "versionInfo.companyName",
				Messages.getString("VersionInfo.company.name"));
		Validator.checkString(internalName, 50, 	"versionInfo.internalName",
				Messages.getString("VersionInfo.internal.name"));
		Validator.checkTrue(!internalName.endsWith(".exe"), "versionInfo.internalName",
				Messages.getString("VersionInfo.internal.name.not.exe"));
		Validator.checkString(originalFilename, 50, "versionInfo.originalFilename",
				Messages.getString("VersionInfo.original.filename"));
		Validator.checkTrue(originalFilename.endsWith(".exe"),
				"versionInfo.originalFilename",
				Messages.getString("VersionInfo.original.filename.exe"));
	}

	public String getCompanyName() {
		return companyName;
	}

	public void setCompanyName(String companyName) {
		this.companyName = companyName;
	}

	public String getCopyright() {
		return copyright;
	}

	public void setCopyright(String copyright) {
		this.copyright = copyright;
	}

	public String getFileDescription() {
		return fileDescription;
	}

	public void setFileDescription(String fileDescription) {
		this.fileDescription = fileDescription;
	}

	public String getFileVersion() {
		return fileVersion;
	}

	public void setFileVersion(String fileVersion) {
		this.fileVersion = fileVersion;
	}

	public String getInternalName() {
		return internalName;
	}

	public void setInternalName(String internalName) {
		this.internalName = internalName;
	}

	public String getOriginalFilename() {
		return originalFilename;
	}

	public void setOriginalFilename(String originalFilename) {
		this.originalFilename = originalFilename;
	}

	public String getProductName() {
		return productName;
	}

	public void setProductName(String productName) {
		this.productName = productName;
	}

	public String getProductVersion() {
		return productVersion;
	}

	public void setProductVersion(String productVersion) {
		this.productVersion = productVersion;
	}

	public String getTxtFileVersion() {
		return txtFileVersion;
	}

	public void setTxtFileVersion(String txtFileVersion) {
		this.txtFileVersion = txtFileVersion;
	}

	public String getTxtProductVersion() {
		return txtProductVersion;
	}

	public void setTxtProductVersion(String txtProductVersion) {
		this.txtProductVersion = txtProductVersion;
	}
}
