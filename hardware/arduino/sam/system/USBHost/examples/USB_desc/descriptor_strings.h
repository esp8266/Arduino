#ifndef DESCRIPTOR_STRINGS_H_INCLUDED
#define DESCRIPTOR_STRINGS_H_INCLUDED

// Buffer size
#define BUFSIZE 256

/* Print strings in Program Memory */
const char Gen_Error_str[] = "\r\nRequest error. Error code:\t";
const char Dev_Header_str[] = "\r\nDevice descriptor: ";
const char Dev_Length_str[] = "\r\nDescriptor Length:\t";
const char Dev_Type_str[] = "\r\nDescriptor type:\t";
const char Dev_Version_str[] = "\r\nUSB version:\t\t";
const char Dev_Class_str[] = "\r\nDevice class:\t\t";
const char Dev_Subclass_str[] = "\r\nDevice Subclass:\t";
const char Dev_Protocol_str[] = "\r\nDevice Protocol:\t";
const char Dev_Pktsize_str[] = "\r\nMax.packet size:\t";
const char Dev_Vendor_str[] = "\r\nVendor  ID:\t\t";
const char Dev_Product_str[] = "\r\nProduct ID:\t\t";
const char Dev_Revision_str[] = "\r\nRevision ID:\t\t";
const char Dev_Mfg_str[] = "\r\nMfg.string index:\t";
const char Dev_Prod_str[] = "\r\nProd.string index:\t";
const char Dev_Serial_str[] = "\r\nSerial number index:\t";
const char Dev_Nconf_str[] = "\r\nNumber of conf.:\t";
const char Conf_Trunc_str[] = "Total length truncated to 256 bytes";
const char Conf_Header_str[] = "\r\nConfiguration descriptor:";
const char Conf_Totlen_str[] = "\r\nTotal length:\t\t";
const char Conf_Nint_str[] = "\r\nNum.intf:\t\t";
const char Conf_Value_str[] = "\r\nConf.value:\t\t";
const char Conf_String_str[] = "\r\nConf.string:\t\t";
const char Conf_Attr_str[] = "\r\nAttr.:\t\t\t";
const char Conf_Pwr_str[] = "\r\nMax.pwr:\t\t";
const char Int_Header_str[] = "\r\n\r\nInterface descriptor:";
const char Int_Number_str[] = "\r\nIntf.number:\t\t";
const char Int_Alt_str[] = "\r\nAlt.:\t\t\t";
const char Int_Endpoints_str[] = "\r\nEndpoints:\t\t";
const char Int_Class_str[] = "\r\nIntf. Class:\t\t";
const char Int_Subclass_str[] = "\r\nIntf. Subclass:\t\t";
const char Int_Protocol_str[] = "\r\nIntf. Protocol:\t\t";
const char Int_String_str[] = "\r\nIntf.string:\t\t";
const char End_Header_str[] = "\r\n\r\nEndpoint descriptor:";
const char End_Address_str[] = "\r\nEndpoint address:\t";
const char End_Attr_str[] = "\r\nAttr.:\t\t\t";
const char End_Pktsize_str[] = "\r\nMax.pkt size:\t\t";
const char End_Interval_str[] = "\r\nPolling interval:\t";
const char Unk_Header_str[] = "\r\n\r\nUnknown descriptor:";
const char Unk_Length_str[] = "\r\nLength:\t\t\t";
const char Unk_Type_str[] = "\r\nType:\t\t\t";
const char Unk_Contents_str[] = "\r\nContents:\t\t";

#endif /* DESCRIPTOR_STRINGS_H_INCLUDED */
