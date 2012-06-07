#ifndef _DESCRIPTOR_PARSER_
#define _DESCRIPTOR_PARSER_

typedef void (*PARSE)( uint8_t* buffer, uint8_t pkt_size );

/* Common Messages */
const char descr_len[] = "Descriptor Length:\t";
const char descr_type[] = "Descriptor type:\t";
const char class_str[] = "Class:\t\t\t";
const char subclass_str[] = "Subclass:\t\t";
const char protocol_str[] = "Protocol:\t\t";
const char maxpktsize_str[] = "Max.packet size:\t";
const char unk_msg[] = " Unknown";
const char reserved_msg[] = "Reserved";
const char rcode_error_msg[] = "\r\nRequest error. Return code: ";

/* Endpoint attributes */
const char control_tr[] = "Control";
const char iso_tr[] = "Isochronous";
const char bulk_tr[] = "Bulk";
const char int_tr[] = "Interrupt";

const char* transfer_types[] =
{
	control_tr,
	iso_tr,
	bulk_tr,
	int_tr
};

const char nosync_type[] = "No Synchronization";
const char async_type[] = "Asynchronous";
const char adaptive_type[] = "Adaptive";
const char sync_type[] = "Synchronous";

const char* sync_types[] =
{
	nosync_type,
	async_type,
	adaptive_type,
	sync_type
};

const char data_usage[] = "Data";
const char feedback_usage[] = "Feedback";
const char implicit_usage[] = "Implicit Feedback Data";
const char reserved_usage[] = "Reserved";

const char* usage_types[] =
{
	data_usage,
	feedback_usage,
	implicit_usage,
	reserved_usage
};

/* HID Country Codes */
const char notsupported_cc[] = "Not Supported";
const char arabic_cc[] = "Arabic";
const char belgian_cc[] = "Belgian";
const char canadianbi_cc[] = "Canadian-Bilingual";
const char canadianfr_cc[] = "Canadian-French";
const char czech_cc[] = "Czech Republic";
const char danish_cc[] = "Danish";
const char finnish_cc[] = "Finnish";
const char french_cc[] = "French";
const char german_cc[] = "German";
const char greek_cc[] = "Greek";
const char hebrew_cc[] = "Hebrew";
const char hungary_cc[] = "Hungary";
const char intl_cc[] = "International (ISO)";
const char italian_cc[] = "Italian";
const char japan_cc[] = "Japan (Katakana)";
const char korean_cc[] = "Korean";
const char latam_cc[] = "Latin American";
const char dutch_cc[] = "Netherlands/Dutch";
const char norwegian_cc[] = "Norwegian";
const char persian_cc[] = "Persian (Farsi)";
const char poland_cc[] = "Poland";
const char portuguese_cc[] = "Portuguese";
const char russia_cc[] = "Russia";
const char slovakia_cc[] = "Slovakia";
const char spanish_cc[] = "Spanish";
const char swedish_cc[] = "Swedish";
const char swiss_fr_cc[] = "Swiss/French";
const char swiss_ger_cc[] = "Swiss/German";
const char swiss_cc[] = "Switzerland";
const char taiwan_cc[] = "Taiwan";
const char turkish_q_cc[] = "Turkish-Q";
const char uk_cc[] = "UK";
const char us_cc[] = "US";
const char yugo_cc[] = "Yugoslavia";
const char turkish_f_cc[] = "Turkish-F";

const char* HID_Country_Codes[] =
{
	notsupported_cc,
	arabic_cc,
	belgian_cc,
	canadianbi_cc,
	canadianfr_cc,
	czech_cc,
	danish_cc,
	finnish_cc,
	french_cc,
	german_cc,
	greek_cc,
	hebrew_cc,
	hungary_cc,
	intl_cc,
	italian_cc,
	japan_cc,
	korean_cc,
	latam_cc,
	dutch_cc,
	norwegian_cc,
	persian_cc,
	poland_cc,
	portuguese_cc,
	russia_cc,
	slovakia_cc,
	spanish_cc,
	swedish_cc,
	swiss_fr_cc,
	swiss_ger_cc,
	swiss_cc,
	taiwan_cc,
	turkish_q_cc,
	uk_cc,
	us_cc,
	yugo_cc,
	turkish_f_cc
};

/* HID report descriptor parser string definitions */
/* Item type strings */
const char btype_main[] = "Main";
const char btype_global[] = "Global";
const char btype_local[] = "Local";
const char btype_reserved[] = "Reserved";

/* Item types strings array. Array index corresponds to bType */
const char* btypes[] =
{
	btype_main,
	btype_global,
	btype_local,
	btype_reserved
};

/* Main Item Tag Strings */
const char main_tag_input[] = "Input ";
const char main_tag_output[] = "Output ";
const char main_tag_collection[] = "Collection ";
const char main_tag_feature[] = "Feature ";
const char main_tag_endcoll[] = "End Collection\r\n";

/* Main Item Tags Strings Array */
const char* maintags[] =
{
	main_tag_input,
	main_tag_output,
	main_tag_collection,
	main_tag_feature,
	main_tag_endcoll
};

/* Global Item Tag Strings */
const char global_tag_usagepage[] = "Usage Page ";
const char global_tag_logmin[] = "Logical Minimum ";
const char global_tag_logmax[] = "Logical Maximum ";
const char global_tag_physmin[] = "Physical Minimum ";
const char global_tag_physmax[] = "Physical Maximum ";
const char global_tag_unitexp[] = "Unit Exponent ";
const char global_tag_unit[] = "Unit ";
const char global_tag_repsize[] = "Report Size ";
const char global_tag_repid[] = "Report ID ";
const char global_tag_repcount[] = "Report Count ";
const char global_tag_push[] = "Push";
const char global_tag_pop[] = "Pop";

/* Global Item Tag Strings Array */
const char* globaltags[] =
{
	global_tag_usagepage,
	global_tag_logmin,
	global_tag_logmax,
	global_tag_physmin,
	global_tag_physmax,
	global_tag_unitexp,
	global_tag_unit,
	global_tag_repsize,
	global_tag_repid,
	global_tag_repcount,
	global_tag_push,
	global_tag_pop
};

/* Local Item Tag Strings */
const char local_tag_usage[] = "Usage ";
const char local_tag_usagemin[] = "Usage Minimum ";
const char local_tag_usagemax[] = "Usage Maximum ";
const char local_tag_desidx[] = "Designator Index ";
const char local_tag_desmin[] = "Designator Minimum ";
const char local_tag_desmax[] = "Designator Maximum ";
const char local_tag_stridx[] = "String Index ";
const char local_tag_strmin[] = "String Minimum ";
const char local_tag_strmax[] = "String Maximum ";
const char local_tag_delimiter[] = "Delimiter ";

/* Local Item Tag Strings Array */
const char* localtags[] =
{
	local_tag_usage,
	local_tag_usagemin,
	local_tag_usagemax,
	local_tag_desidx,
	local_tag_desmin,
	local_tag_desmax,
	local_tag_stridx,
	local_tag_strmin,
	local_tag_strmax,
	local_tag_delimiter
};

/* Collection Types Strings */
const char coll_phy[] = "Physical (group of axes)";
const char coll_app[] = "Application (mouse, keyboard)";
const char coll_log[] = "Logical (interrelated data)";
const char coll_rep[] = "Report";
const char coll_arr[] = "Named Array";
const char coll_usw[] = "Usage Switch";
const char coll_umod[] = "Usage Modifier";

/* Collection Types Strings Array */
const char* collections[] =
{
	coll_phy,
	coll_app,
	coll_log,
	coll_rep,
	coll_arr,
	coll_usw,
	coll_umod
};

/* Usage Pages Strings */
const char up_undef[] = "Undefined";
const char up_gendesk[] = "Generic Desktop Controls";
const char up_sim[] = "Simulation Controls";
const char up_vr[] = "VR Controls";
const char up_sport[] = "Sport Controls";
const char up_game[] = "Game Controls";
const char up_gendev[] = "Generic Device Controls";
const char up_kbd[] = "Keyboard/Keypad";
const char up_led[] = "LEDs";
const char up_button[] = "Button";
const char up_ord[] = "Ordinal";
const char up_tele[] = "Telephony";
const char up_cons[] = "Consumer";
const char up_dig[] = "Digitizer";
//const char up_res[] = "Reserved";
const char up_pid[] = "PID Page";
const char up_uni[] = "Unicode";

/* Usage Pages Strings Array  */
const char * usage_pages[] =
{
	up_undef,
	up_gendesk,
	up_sim,
	up_vr,
	up_sport,
	up_game,
	up_gendev,
	up_kbd,
	up_led,
	up_button,
	up_ord,
	up_tele,
	up_cons,
	up_dig,
	reserved_msg,
	up_pid,
	up_uni
};

#endif //_DESCRIPTOR_PARSER_
