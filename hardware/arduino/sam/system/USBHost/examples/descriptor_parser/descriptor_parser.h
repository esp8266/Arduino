#ifndef _DESCRIPTOR_PARSER_
#define _DESCRIPTOR_PARSER_

/* PGMSPACE */
#include <inttypes.h>
#include <avr/pgmspace.h>

typedef void (*PARSE)( uint8_t bytes );

/* Common Messages */

const char descr_len [] PROGMEM = "\r\nDescriptor Length:\t";
const char descr_type [] PROGMEM = "\r\nDescriptor type:\t";
const char class_str [] PROGMEM = "\r\nClass:\t\t\t";
const char subclass_str [] PROGMEM = "\r\nSubclass:\t\t";
const char protocol_str [] PROGMEM = "\r\nProtocol:\t\t";
const char maxpktsize_str [] PROGMEM = "\r\nMax.packet size:\t";
const char unk_msg [] PROGMEM = " Unknown";
const char reserved_msg [] PROGMEM = "Reserved";
const char rcode_error_msg [] PROGMEM = "\r\nRequest error. Reurn code: ";

/* Endpoint attributes */

const char control_tr [] PROGMEM = "Control";
const char iso_tr [] PROGMEM = "Isochronous";
const char bulk_tr [] PROGMEM = "Bulk";
const char int_tr [] PROGMEM = "Interrupt";

const char* transfer_types [] PROGMEM =
{
  control_tr,
  iso_tr,
  bulk_tr,
  int_tr
};

const char nosync_type [] PROGMEM = "No Synchronization";
const char async_type [] PROGMEM = "Asynchronous";
const char adaptive_type [] PROGMEM = "Adaptive";
const char sync_type [] PROGMEM = "Synchronous";

const char* sync_types [] PROGMEM =
{
  nosync_type,
  async_type,
  adaptive_type,
  sync_type
};

const char data_usage [] PROGMEM = "Data";
const char feedback_usage [] PROGMEM = "Feedback";
const char implicit_usage [] PROGMEM = "Implicit Feedback Data";
const char reserved_usage [] PROGMEM = "Reserved";

const char* usage_types [] PROGMEM =
{
  data_usage,
  feedback_usage,
  implicit_usage,
  reserved_usage
};

/* HID Country Codes */

const char notsupported_cc [] PROGMEM = "Not Supported";
const char arabic_cc [] PROGMEM = "Arabic";
const char belgian_cc [] PROGMEM = "Belgian";
const char canadianbi_cc [] PROGMEM = "Canadian-Bilingual";
const char canadianfr_cc [] PROGMEM = "Canadian-French";
const char czech_cc [] PROGMEM = "Czech Republic";
const char danish_cc [] PROGMEM = "Danish";
const char finnish_cc [] PROGMEM = "Finnish";
const char french_cc [] PROGMEM = "French";
const char german_cc [] PROGMEM = "German";
const char greek_cc [] PROGMEM = "Greek";
const char hebrew_cc [] PROGMEM = "Hebrew";
const char hungary_cc [] PROGMEM = "Hungary";
const char intl_cc [] PROGMEM = "International (ISO)";
const char italian_cc [] PROGMEM = "Italian";
const char japan_cc [] PROGMEM = "Japan (Katakana)";
const char korean_cc [] PROGMEM = "Korean";
const char latam_cc [] PROGMEM = "Latin American";
const char dutch_cc [] PROGMEM = "Netherlands/Dutch";
const char norwegian_cc [] PROGMEM = "Norwegian";
const char persian_cc [] PROGMEM = "Persian (Farsi)";
const char poland_cc [] PROGMEM = "Poland";
const char portuguese_cc [] PROGMEM = "Portuguese";
const char russia_cc [] PROGMEM = "Russia";
const char slovakia_cc [] PROGMEM = "Slovakia";
const char spanish_cc [] PROGMEM = "Spanish";
const char swedish_cc [] PROGMEM = "Swedish";
const char swiss_fr_cc [] PROGMEM = "Swiss/French";
const char swiss_ger_cc [] PROGMEM = "Swiss/German";
const char swiss_cc [] PROGMEM = "Switzerland";
const char taiwan_cc [] PROGMEM = "Taiwan";
const char turkish_q_cc [] PROGMEM = "Turkish-Q";
const char uk_cc [] PROGMEM = "UK";
const char us_cc [] PROGMEM = "US";
const char yugo_cc [] PROGMEM = "Yugoslavia";
const char turkish_f_cc [] PROGMEM = "Turkish-F";

const char* HID_Country_Codes [] PROGMEM =
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
const char btype_main [] PROGMEM = "Main";
const char btype_global [] PROGMEM = "Global";
const char btype_local [] PROGMEM = "Local";
const char btype_reserved [] PROGMEM = "Reserved";
/* Item types strings array. Array index corresponds to bType */
const char* btypes [] PROGMEM =
{
  btype_main,
  btype_global,
  btype_local,
  btype_reserved
};
/* Main Item Tag Strings */
const char main_tag_input [] PROGMEM = "Input\t\t";
const char main_tag_output [] PROGMEM = "Output\t\t";
const char main_tag_collection [] PROGMEM = "Collection\t\t";
const char main_tag_feature [] PROGMEM = "Feature\t\t";
const char main_tag_endcoll [] PROGMEM = "End Collection\t";
/* Main Item Tags Strings Array */
const char* maintags [] PROGMEM =
{
  main_tag_input,
  main_tag_output,
  main_tag_collection,
  main_tag_feature,
  main_tag_endcoll
};
/* Global Item Tag Strings */
const char global_tag_usagepage [] PROGMEM = "Usage Page\t\t";
const char global_tag_logmin [] PROGMEM = "Logical Minimum\t";
const char global_tag_logmax [] PROGMEM = "Logical Maximum\t";
const char global_tag_physmin [] PROGMEM = "Physical Minimum\t";
const char global_tag_physmax [] PROGMEM = "Physical Maximum\t";
const char global_tag_unitexp [] PROGMEM = "Unit Exponent\t";
const char global_tag_unit [] PROGMEM = "Unit\t\t";
const char global_tag_repsize [] PROGMEM = "Report Size\t";
const char global_tag_repid [] PROGMEM = "Report ID\t\t";
const char global_tag_repcount [] PROGMEM = "Report Count\t";
const char global_tag_push [] PROGMEM = "Push\t\t";
const char global_tag_pop [] PROGMEM = "Pop\t\t";
/* Global Item Tag Strings Array */
const char* globaltags [] PROGMEM =
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
const char local_tag_usage [] PROGMEM = "Usage\t\t";
const char local_tag_usagemin [] PROGMEM = "Usage Minimum\t";
const char local_tag_usagemax [] PROGMEM = "Usage Maximum\t";
const char local_tag_desidx [] PROGMEM = "Designator Index\t";
const char local_tag_desmin [] PROGMEM = "Designator Minimum\t";
const char local_tag_desmax [] PROGMEM = "Designator Maximum\t";
const char local_tag_stridx [] PROGMEM = "String Index\t";
const char local_tag_strmin [] PROGMEM = "String Minimum\t";
const char local_tag_strmax [] PROGMEM = "String Maximum\t";
const char local_tag_delimiter [] PROGMEM = "Delimiter\t";
/* Local Item Tag Strings Array */
const char* localtags [] PROGMEM =
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
const char coll_phy [] PROGMEM = "Physical (group of axes)";
const char coll_app [] PROGMEM = "Application (mouse, keyboard)";
const char coll_log [] PROGMEM = "Logical (interrelated data)";
const char coll_rep [] PROGMEM = "Report";
const char coll_arr [] PROGMEM = "Named Array";
const char coll_usw [] PROGMEM = "Usage Switch";
const char coll_umod [] PROGMEM = "Usage Modifier";
/* Collection Types Strings Array */
const char* collections [] PROGMEM =
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
const char up_undef [] PROGMEM = "Undefined";
const char up_gendesk [] PROGMEM = "Generic Desktop Controls";
const char up_sim [] PROGMEM = "Simulation Controls";
const char up_vr [] PROGMEM = "VR Controls";
const char up_sport [] PROGMEM = "Sport Controls";
const char up_game [] PROGMEM = "Game Controls";
const char up_gendev [] PROGMEM = "Generic Device Controls";
const char up_kbd [] PROGMEM = "Keyboard/Keypad";
const char up_led [] PROGMEM = "LEDs";
const char up_button [] PROGMEM = "Button";
const char up_ord [] PROGMEM = "Ordinal";
const char up_tele [] PROGMEM = "Telephony";
const char up_cons [] PROGMEM = "Consumer";
const char up_dig [] PROGMEM = "Digitizer";
//const char up_res [] PROGMEM = "Reserved";
const char up_pid [] PROGMEM = "PID Page";
const char up_uni [] PROGMEM = "Unicode";
/* Usage Pages Strings Array  */
const char * usage_pages [] PROGMEM =
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
