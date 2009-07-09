/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

	 ====================================================================
	 Part of these mappings came from SDL. If they changed, they will
	 have to be changed here also, for portability those must be available
	 for wii to look out (without using SDL on wii at all).
	 ====================================================================

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _KEYSYMS_H_
#define _KEYSYMS_H_

typedef enum 
{
	/* The keyboard syms have been cleverly chosen to map to ASCII */
	K_KBD_UNKNOWN		= 0,
	K_KBD_FIRST		= 0,
	K_KBD_BACKSPACE		= 8,
	K_KBD_TAB		= 9,
	K_KBD_CLEAR		= 12,
	K_KBD_RETURN		= 13,
	K_KBD_PAUSE		= 19,
	K_KBD_ESCAPE		= 27,
	K_KBD_SPACE		= 32,
	K_KBD_EXCLAIM		= 33,
	K_KBD_QUOTEDBL		= 34,
	K_KBD_HASH		= 35,
	K_KBD_DOLLAR		= 36,
	K_KBD_AMPERSAND		= 38,
	K_KBD_QUOTE		= 39,
	K_KBD_LEFTPAREN		= 40,
	K_KBD_RIGHTPAREN		= 41,
	K_KBD_ASTERISK		= 42,
	K_KBD_PLUS		= 43,
	K_KBD_COMMA		= 44,
	K_KBD_MINUS		= 45,
	K_KBD_PERIOD		= 46,
	K_KBD_SLASH		= 47,
	K_KBD_0			= 48,
	K_KBD_1			= 49,
	K_KBD_2			= 50,
	K_KBD_3			= 51,
	K_KBD_4			= 52,
	K_KBD_5			= 53,
	K_KBD_6			= 54,
	K_KBD_7			= 55,
	K_KBD_8			= 56,
	K_KBD_9			= 57,
	K_KBD_COLON		= 58,
	K_KBD_SEMICOLON		= 59,
	K_KBD_LESS		= 60,
	K_KBD_EQUALS		= 61,
	K_KBD_GREATER		= 62,
	K_KBD_QUESTION		= 63,
	K_KBD_AT			= 64,
	/* 
	   Skip uppercase letters
	 */
	K_KBD_LEFTBRACKET	= 91,
	K_KBD_BACKSLASH		= 92,
	K_KBD_RIGHTBRACKET	= 93,
	K_KBD_CARET		= 94,
	K_KBD_UNDERSCORE		= 95,
	K_KBD_BACKQUOTE		= 96,
	K_KBD_a			= 97,
	K_KBD_b			= 98,
	K_KBD_c			= 99,
	K_KBD_d			= 100,
	K_KBD_e			= 101,
	K_KBD_f			= 102,
	K_KBD_g			= 103,
	K_KBD_h			= 104,
	K_KBD_i			= 105,
	K_KBD_j			= 106,
	K_KBD_k			= 107,
	K_KBD_l			= 108,
	K_KBD_m			= 109,
	K_KBD_n			= 110,
	K_KBD_o			= 111,
	K_KBD_p			= 112,
	K_KBD_q			= 113,
	K_KBD_r			= 114,
	K_KBD_s			= 115,
	K_KBD_t			= 116,
	K_KBD_u			= 117,
	K_KBD_v			= 118,
	K_KBD_w			= 119,
	K_KBD_x			= 120,
	K_KBD_y			= 121,
	K_KBD_z			= 122,
	K_KBD_DELETE		= 127,
	/* End of ASCII mapped keysyms */

	/* International keyboard syms */
	K_KBD_WORLD_0		= 160,		/* 0xA0 */
	K_KBD_WORLD_1		= 161,
	K_KBD_WORLD_2		= 162,
	K_KBD_WORLD_3		= 163,
	K_KBD_WORLD_4		= 164,
	K_KBD_WORLD_5		= 165,
	K_KBD_WORLD_6		= 166,
	K_KBD_WORLD_7		= 167,
	K_KBD_WORLD_8		= 168,
	K_KBD_WORLD_9		= 169,
	K_KBD_WORLD_10		= 170,
	K_KBD_WORLD_11		= 171,
	K_KBD_WORLD_12		= 172,
	K_KBD_WORLD_13		= 173,
	K_KBD_WORLD_14		= 174,
	K_KBD_WORLD_15		= 175,
	K_KBD_WORLD_16		= 176,
	K_KBD_WORLD_17		= 177,
	K_KBD_WORLD_18		= 178,
	K_KBD_WORLD_19		= 179,
	K_KBD_WORLD_20		= 180,
	K_KBD_WORLD_21		= 181,
	K_KBD_WORLD_22		= 182,
	K_KBD_WORLD_23		= 183,
	K_KBD_WORLD_24		= 184,
	K_KBD_WORLD_25		= 185,
	K_KBD_WORLD_26		= 186,
	K_KBD_WORLD_27		= 187,
	K_KBD_WORLD_28		= 188,
	K_KBD_WORLD_29		= 189,
	K_KBD_WORLD_30		= 190,
	K_KBD_WORLD_31		= 191,
	K_KBD_WORLD_32		= 192,
	K_KBD_WORLD_33		= 193,
	K_KBD_WORLD_34		= 194,
	K_KBD_WORLD_35		= 195,
	K_KBD_WORLD_36		= 196,
	K_KBD_WORLD_37		= 197,
	K_KBD_WORLD_38		= 198,
	K_KBD_WORLD_39		= 199,
	K_KBD_WORLD_40		= 200,
	K_KBD_WORLD_41		= 201,
	K_KBD_WORLD_42		= 202,
	K_KBD_WORLD_43		= 203,
	K_KBD_WORLD_44		= 204,
	K_KBD_WORLD_45		= 205,
	K_KBD_WORLD_46		= 206,
	K_KBD_WORLD_47		= 207,
	K_KBD_WORLD_48		= 208,
	K_KBD_WORLD_49		= 209,
	K_KBD_WORLD_50		= 210,
	K_KBD_WORLD_51		= 211,
	K_KBD_WORLD_52		= 212,
	K_KBD_WORLD_53		= 213,
	K_KBD_WORLD_54		= 214,
	K_KBD_WORLD_55		= 215,
	K_KBD_WORLD_56		= 216,
	K_KBD_WORLD_57		= 217,
	K_KBD_WORLD_58		= 218,
	K_KBD_WORLD_59		= 219,
	K_KBD_WORLD_60		= 220,
	K_KBD_WORLD_61		= 221,
	K_KBD_WORLD_62		= 222,
	K_KBD_WORLD_63		= 223,
	K_KBD_WORLD_64		= 224,
	K_KBD_WORLD_65		= 225,
	K_KBD_WORLD_66		= 226,
	K_KBD_WORLD_67		= 227,
	K_KBD_WORLD_68		= 228,
	K_KBD_WORLD_69		= 229,
	K_KBD_WORLD_70		= 230,
	K_KBD_WORLD_71		= 231,
	K_KBD_WORLD_72		= 232,
	K_KBD_WORLD_73		= 233,
	K_KBD_WORLD_74		= 234,
	K_KBD_WORLD_75		= 235,
	K_KBD_WORLD_76		= 236,
	K_KBD_WORLD_77		= 237,
	K_KBD_WORLD_78		= 238,
	K_KBD_WORLD_79		= 239,
	K_KBD_WORLD_80		= 240,
	K_KBD_WORLD_81		= 241,
	K_KBD_WORLD_82		= 242,
	K_KBD_WORLD_83		= 243,
	K_KBD_WORLD_84		= 244,
	K_KBD_WORLD_85		= 245,
	K_KBD_WORLD_86		= 246,
	K_KBD_WORLD_87		= 247,
	K_KBD_WORLD_88		= 248,
	K_KBD_WORLD_89		= 249,
	K_KBD_WORLD_90		= 250,
	K_KBD_WORLD_91		= 251,
	K_KBD_WORLD_92		= 252,
	K_KBD_WORLD_93		= 253,
	K_KBD_WORLD_94		= 254,
	K_KBD_WORLD_95		= 255,		/* 0xFF */

	/* Numeric keypad */
	K_KBD_KP0		= 256,
	K_KBD_KP1		= 257,
	K_KBD_KP2		= 258,
	K_KBD_KP3		= 259,
	K_KBD_KP4		= 260,
	K_KBD_KP5		= 261,
	K_KBD_KP6		= 262,
	K_KBD_KP7		= 263,
	K_KBD_KP8		= 264,
	K_KBD_KP9		= 265,
	K_KBD_KP_PERIOD		= 266,
	K_KBD_KP_DIVIDE		= 267,
	K_KBD_KP_MULTIPLY	= 268,
	K_KBD_KP_MINUS		= 269,
	K_KBD_KP_PLUS		= 270,
	K_KBD_KP_ENTER		= 271,
	K_KBD_KP_EQUALS		= 272,

	/* Arrows + Home/End pad */
	K_KBD_UP			= 273,
	K_KBD_DOWN		= 274,
	K_KBD_RIGHT		= 275,
	K_KBD_LEFT		= 276,
	K_KBD_INSERT		= 277,
	K_KBD_HOME		= 278,
	K_KBD_END		= 279,
	K_KBD_PAGEUP		= 280,
	K_KBD_PAGEDOWN		= 281,

	/* Function keys */
	K_KBD_F1			= 282,
	K_KBD_F2			= 283,
	K_KBD_F3			= 284,
	K_KBD_F4			= 285,
	K_KBD_F5			= 286,
	K_KBD_F6			= 287,
	K_KBD_F7			= 288,
	K_KBD_F8			= 289,
	K_KBD_F9			= 290,
	K_KBD_F10		= 291,
	K_KBD_F11		= 292,
	K_KBD_F12		= 293,
	K_KBD_F13		= 294,
	K_KBD_F14		= 295,
	K_KBD_F15		= 296,

	/* Key state modifier keys */
	K_KBD_NUMLOCK		= 300,
	K_KBD_CAPSLOCK		= 301,
	K_KBD_SCROLLOCK		= 302,
	K_KBD_RSHIFT		= 303,
	K_KBD_LSHIFT		= 304,
	K_KBD_RCTRL		= 305,
	K_KBD_LCTRL		= 306,
	K_KBD_RALT		= 307,
	K_KBD_LALT		= 308,
	K_KBD_RMETA		= 309,
	K_KBD_LMETA		= 310,
	K_KBD_LSUPER		= 311,		/* Left "Windows" key */
	K_KBD_RSUPER		= 312,		/* Right "Windows" key */
	K_KBD_MODE		= 313,		/* "Alt Gr" key */
	K_KBD_COMPOSE		= 314,		/* Multi-key compose key */

	/* Miscellaneous function keys */
	K_KBD_HELP		= 315,
	K_KBD_PRINT		= 316,
	K_KBD_SYSREQ		= 317,
	K_KBD_BREAK		= 318,
	K_KBD_MENU		= 319,
	K_KBD_POWER		= 320,		/* Power Macintosh power key */
	K_KBD_EURO		= 321,		/* Some european keyboards */
	K_KBD_UNDO		= 322,		/* Atari keyboard has Undo */

	/* Add any other keys here */

	K_KBD_LAST
} K_CopyK_KBDey;

#define WIIMOTE_BUTTON_2							0x0001
#define WIIMOTE_BUTTON_1							0x0002
#define WIIMOTE_BUTTON_B							0x0004
#define WIIMOTE_BUTTON_A							0x0008
#define WIIMOTE_BUTTON_MINUS						0x0010
#define WIIMOTE_BUTTON_HOME						0x0080
#define WIIMOTE_BUTTON_LEFT						0x0100
#define WIIMOTE_BUTTON_RIGHT						0x0200
#define WIIMOTE_BUTTON_DOWN						0x0400
#define WIIMOTE_BUTTON_UP							0x0800
#define WIIMOTE_BUTTON_PLUS						0x1000

#endif

