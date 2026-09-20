/*
 *  Copyright (C) 2010-2026 Fabio Cavallo (aka FHorse)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _JSTICK_DB_H_
#define _JSTICK_DB_H_

#include "common.h"
#include "jstick_ids.h"
#include "os_jstick.h"

enum _js_db_misc {
	JS_DB_ICON_DESC_ELEMENTS = 21,
	JS_DB_NO_VENDOR_ID = 0xFFFFFFFF,
	JS_DB_NO_PRODUCT_ID = JS_DB_NO_VENDOR_ID
};

#define JS_BTN_DEF_BIT 0x20000
#define JS_BTN_DEF(a) a | JS_BTN_DEF_BIT
#define JS_IS_BTN_DEF(a) a & JS_BTN_DEF_BIT
#define JS_ABS_DEF_BIT(b) ((b & 0x01) << 16)
#define JS_ABS_DEF(a, b) a | JS_ABS_DEF_BIT(b)
#define JS_BTNABS_UNDEF(a) a & 0xFFFF

typedef struct _js_db_device_icon_desc {
	DBWORD offset;
	const uTCHAR *icon;
	const uTCHAR *desc;
} _js_db_device_icon_desc;
typedef struct _js_db_device {
	enum _js_gamepad_type type;
	BYTE is_default;
	DBWORD vendor_id;
	DBWORD product_id;
	DBWORD std_pad_default[MAX_STD_PAD_BUTTONS];
	_js_db_device_icon_desc btn[JS_DB_ICON_DESC_ELEMENTS];
	_js_db_device_icon_desc axs[JS_DB_ICON_DESC_ELEMENTS];
} _js_db_device;

static const _js_db_device js_db_devices[] = {
	// Default generico e Xbox360
	{
		JS_SC_MS_XBOX_360_GAMEPAD,
		TRUE,
		JS_DB_NO_VENDOR_ID,
		JS_DB_NO_PRODUCT_ID,
		{
			/* BUT_A  */ JS_BTN_DEF(BTN_B),
			/* BUT_B  */ JS_BTN_DEF(BTN_A),
			/* SELECT */ JS_BTN_DEF(BTN_SELECT),
			/* START  */ JS_BTN_DEF(BTN_START),
			/* UP     */ JS_ABS_DEF(ABS_Y, 0),
			/* DOWN   */ JS_ABS_DEF(ABS_Y, 1),
			/* LEFT   */ JS_ABS_DEF(ABS_X, 0),
			/* RIGHT  */ JS_ABS_DEF(ABS_X, 1),
			/* TRB_A  */ JS_BTN_DEF(BTN_Y),
			/* TRB_B  */ JS_BTN_DEF(BTN_X)
		},
		{
			{ BTN_A,          uL(":/icon/icons/gamepad_xbox_360_a.svgz"),             uL("A")         },
			{ BTN_B,          uL(":/icon/icons/gamepad_xbox_360_b.svgz"),             uL("B")         },
			{ BTN_X,          uL(":/icon/icons/gamepad_xbox_360_x.svgz"),             uL("X")         },
			{ BTN_Y,          uL(":/icon/icons/gamepad_xbox_360_y.svgz"),             uL("Y")         },
			{ BTN_TL,         uL(":/icon/icons/gamepad_xbox_lb.svgz"),                uL("LB")        },
			{ BTN_TR,         uL(":/icon/icons/gamepad_xbox_rb.svgz"),                uL("RB")        },
			{ BTN_SELECT,     uL(":/icon/icons/gamepad_xbox_360_back.svgz"),          uL("BACK")      },
			{ BTN_START,      uL(":/icon/icons/gamepad_xbox_360_start.svgz"),         uL("START")     },
			{ BTN_MODE,       uL(":/icon/icons/gamepad_xbox_360_home.svgz"),          uL("HOME")      },
			{ BTN_THUMBL,     uL(":/icon/icons/gamepad_xbox_left_stick_click.svgz"),  uL("LS CLICK")  },
			{ BTN_THUMBR,     uL(":/icon/icons/gamepad_xbox_right_stick_click.svgz"), uL("RS CLICK")  },
			{ BTN_DPAD_LEFT,  uL(":/icon/icons/gamepad_xbox_360_dpad_left.svgz"),     uL("DPAD LEFT") },
			{ BTN_DPAD_RIGHT, uL(":/icon/icons/gamepad_xbox_360_dpad_right.svgz"),    uL("DPAD RIGHT")},
			{ BTN_DPAD_UP,    uL(":/icon/icons/gamepad_xbox_360_dpad_up.svgz"),       uL("DPAD UP")   },
			{ BTN_DPAD_DOWN,  uL(":/icon/icons/gamepad_xbox_360_dpad_down.svgz"),     uL("DPAD DOWN") }
		},
		{
			{ ABS_X,          uL(":/icon/icons/gamepad_xbox_left_stick_left.svgz"),   uL("LS LEFT")   },
			{ ABS_X,          uL(":/icon/icons/gamepad_xbox_left_stick_right.svgz"),  uL("LS RIGHT")  },
			{ ABS_Y,          uL(":/icon/icons/gamepad_xbox_left_stick_up.svgz"),     uL("LS UP")     },
			{ ABS_Y,          uL(":/icon/icons/gamepad_xbox_left_stick_down.svgz"),   uL("LS DOWN")   },
			{ ABS_Z,          uL(":/icon/icons/gamepad_xbox_lt.svgz"),                uL("LT")        },
			{ ABS_Z,          uL(":/icon/icons/gamepad_xbox_lt.svgz"),                uL("LT")        },
			{ ABS_RX,         uL(":/icon/icons/gamepad_xbox_right_stick_left.svgz"),  uL("RS LEFT")   },
			{ ABS_RX,         uL(":/icon/icons/gamepad_xbox_right_stick_right.svgz"), uL("RS RIGHT")  },
			{ ABS_RY,         uL(":/icon/icons/gamepad_xbox_right_stick_up.svgz"),    uL("RS UP")     },
			{ ABS_RY,         uL(":/icon/icons/gamepad_xbox_right_stick_down.svgz"),  uL("RS DOWN")   },
			{ ABS_RZ,         uL(":/icon/icons/gamepad_xbox_rt.svgz"),                uL("RT")        },
			{ ABS_RZ,         uL(":/icon/icons/gamepad_xbox_rt.svgz"),                uL("RT")        },
			{ ABS_HAT0X,      uL(":/icon/icons/gamepad_xbox_360_dpad_left.svgz"),     uL("DPAD LEFT") },
			{ ABS_HAT0X,      uL(":/icon/icons/gamepad_xbox_360_dpad_right.svgz"),    uL("DPAD RIGHT")},
			{ ABS_HAT0Y,      uL(":/icon/icons/gamepad_xbox_360_dpad_up.svgz"),       uL("DPAD UP")   },
			{ ABS_HAT0Y,      uL(":/icon/icons/gamepad_xbox_360_dpad_down.svgz"),     uL("DPAD DOWN") }
		}
	},
	// Xbox One
	{
		JS_SC_MS_XBOX_ONE_GAMEPAD,
		TRUE,
		JS_DB_NO_VENDOR_ID,
		JS_DB_NO_PRODUCT_ID,
		{
			/* BUT_A  */ JS_BTN_DEF(BTN_B),
			/* BUT_B  */ JS_BTN_DEF(BTN_A),
			/* SELECT */ JS_BTN_DEF(BTN_SELECT),
			/* START  */ JS_BTN_DEF(BTN_START),
			/* UP     */ JS_ABS_DEF(ABS_Y, 0),
			/* DOWN   */ JS_ABS_DEF(ABS_Y, 1),
			/* LEFT   */ JS_ABS_DEF(ABS_X, 0),
			/* RIGHT  */ JS_ABS_DEF(ABS_X, 1),
			/* TRB_A  */ JS_BTN_DEF(BTN_Y),
			/* TRB_B  */ JS_BTN_DEF(BTN_X)
		},
		{
			{ BTN_A,          uL(":/icon/icons/gamepad_xbox_one_a.svgz"),               uL("A")         },
			{ BTN_B,          uL(":/icon/icons/gamepad_xbox_one_b.svgz"),               uL("B")         },
			{ BTN_X,          uL(":/icon/icons/gamepad_xbox_one_x.svgz"),               uL("X")         },
			{ BTN_Y,          uL(":/icon/icons/gamepad_xbox_one_y.svgz"),               uL("Y")         },
			{ BTN_TL,         uL(":/icon/icons/gamepad_xbox_lb.svgz"),                  uL("LB")        },
			{ BTN_TR,         uL(":/icon/icons/gamepad_xbox_rb.svgz"),                  uL("RB")        },
			{ BTN_SELECT,     uL(":/icon/icons/gamepad_xbox_one_view.svgz"),            uL("VIEW")      },
			{ BTN_START,      uL(":/icon/icons/gamepad_xbox_one_menu.svgz"),            uL("MODE")      },
			{ BTN_MODE,       uL(":/icon/icons/gamepad_xbox_360_home.svgz"),            uL("HOME")      },
			{ BTN_THUMBL,     uL(":/icon/icons/gamepad_xbox_left_stick_click.svgz"),    uL("LS CLICK")  },
			{ BTN_THUMBR,     uL(":/icon/icons/gamepad_xbox_right_stick_click.svgz"),   uL("RS CLICK")  },
			{ BTN_DPAD_LEFT,  uL(":/icon/icons/gamepad_xbox_one_dpad_left.svgz"),       uL("DPAD LEFT") },
			{ BTN_DPAD_RIGHT, uL(":/icon/icons/gamepad_xbox_one_dpad_right.svgz"),      uL("DPAD RIGHT")},
			{ BTN_DPAD_UP,    uL(":/icon/icons/gamepad_xbox_one_dpad_up.svgz"),         uL("DPAD UP")   },
			{ BTN_DPAD_DOWN,  uL(":/icon/icons/gamepad_xbox_one_dpad_down.svgz"),       uL("DPAD DOWN") }
		},
		{
			{ ABS_X,          uL(":/icon/icons/gamepad_xbox_left_stick_left.svgz"),     uL("LS LEFT")   },
			{ ABS_X,          uL(":/icon/icons/gamepad_xbox_left_stick_right.svgz"),    uL("LS RIGHT")  },
			{ ABS_Y,          uL(":/icon/icons/gamepad_xbox_left_stick_up.svgz"),       uL("LS UP")     },
			{ ABS_Y,          uL(":/icon/icons/gamepad_xbox_left_stick_down.svgz"),     uL("LS DOWN")   },
			{ ABS_Z,          uL(":/icon/icons/gamepad_xbox_lt.svgz"),                  uL("LT")        },
			{ ABS_Z,          uL(":/icon/icons/gamepad_xbox_lt.svgz"),                  uL("LT")        },
			{ ABS_RX,         uL(":/icon/icons/gamepad_xbox_right_stick_left.svgz"),    uL("RS LEFT")   },
			{ ABS_RX,         uL(":/icon/icons/gamepad_xbox_right_stick_right.svgz"),   uL("RS RIGHT")  },
			{ ABS_RY,         uL(":/icon/icons/gamepad_xbox_right_stick_up.svgz"),      uL("RS UP")     },
			{ ABS_RY,         uL(":/icon/icons/gamepad_xbox_right_stick_down.svgz"),    uL("RS DOWN")   },
			{ ABS_RZ,         uL(":/icon/icons/gamepad_xbox_rt.svgz"),                  uL("RT")        },
			{ ABS_RZ,         uL(":/icon/icons/gamepad_xbox_rt.svgz"),                  uL("RT")        },
			{ ABS_HAT0X,      uL(":/icon/icons/gamepad_xbox_one_dpad_left.svgz"),       uL("DPAD LEFT") },
			{ ABS_HAT0X,      uL(":/icon/icons/gamepad_xbox_one_dpad_right.svgz"),      uL("DPAD RIGHT")},
			{ ABS_HAT0Y,      uL(":/icon/icons/gamepad_xbox_one_dpad_up.svgz"),         uL("DPAD UP")   },
			{ ABS_HAT0Y,      uL(":/icon/icons/gamepad_xbox_one_dpad_down.svgz"),       uL("DPAD DOWN") }
		}
	},
	// Playstation 3
	{
		JS_SC_SONY_PS3_GAMEPAD,
		TRUE,
		JS_DB_NO_VENDOR_ID,
		JS_DB_NO_PRODUCT_ID,
		{
			/* BUT_A  */ JS_BTN_DEF(BTN_B),
			/* BUT_B  */ JS_BTN_DEF(BTN_X),
			/* SELECT */ JS_BTN_DEF(BTN_MODE),
			/* START  */ JS_BTN_DEF(BTN_THUMBL),
			/* UP     */ JS_ABS_DEF(ABS_Y, 0),
			/* DOWN   */ JS_ABS_DEF(ABS_Y, 1),
			/* LEFT   */ JS_ABS_DEF(ABS_X, 0),
			/* RIGHT  */ JS_ABS_DEF(ABS_X, 1),
			/* TRB_A  */ JS_BTN_DEF(BTN_A),
			/* TRB_B  */ JS_BTN_DEF(BTN_Y)
		},
		{
			{ BTN_X,          uL(":/icon/icons/gamepad_playstation_x.svgz"),            uL("CROSS")     },
			{ BTN_B,          uL(":/icon/icons/gamepad_playstation_c.svgz"),            uL("CIRCLE")    },
			{ BTN_Y,          uL(":/icon/icons/gamepad_playstation_s.svgz"),            uL("SQUARE")    },
			{ BTN_A,          uL(":/icon/icons/gamepad_playstation_t.svgz"),            uL("TRIANGLE")  },
			{ BTN_SELECT,     uL(":/icon/icons/gamepad_playstation_3_l1.svgz"),         uL("L1")        },
			{ BTN_START,      uL(":/icon/icons/gamepad_playstation_3_r1.svgz"),         uL("R1")        },
			{ BTN_TL,         uL(":/icon/icons/gamepad_playstation_3_l2.svgz"),         uL("L2")        },
			{ BTN_TR,         uL(":/icon/icons/gamepad_playstation_3_r2.svgz"),         uL("R2")        },
			{ BTN_MODE,       uL(":/icon/icons/gamepad_playstation_3_select.svgz"),     uL("SELECT")    },
			{ BTN_THUMBL,     uL(":/icon/icons/gamepad_playstation_3_start.svgz"),      uL("START")     },
			{ BTN_DPAD_DOWN,  uL(":/icon/icons/gamepad_playstation_home.svgz"),         uL("HOME")      },
			{ BTN_THUMBR,     uL(":/icon/icons/gamepad_xbox_left_stick_click.svgz"),    uL("LS CLICK")  },
			{ BTN_DPAD_UP,    uL(":/icon/icons/gamepad_xbox_right_stick_click.svgz"),   uL("RS CLICK")  }
		},
		{
			{ ABS_X,          uL(":/icon/icons/gamepad_xbox_left_stick_left.svgz"),     uL("LS LEFT")   },
			{ ABS_X,          uL(":/icon/icons/gamepad_xbox_left_stick_right.svgz"),    uL("LS RIGHT")  },
			{ ABS_Y,          uL(":/icon/icons/gamepad_xbox_left_stick_up.svgz"),       uL("LS UP")     },
			{ ABS_Y,          uL(":/icon/icons/gamepad_xbox_left_stick_down.svgz"),     uL("LS DOWN")   },
			{ ABS_RX,         uL(":/icon/icons/gamepad_playstation_3_l2.svgz"),         uL("L2")        },
			{ ABS_RX,         uL(":/icon/icons/gamepad_playstation_3_l2.svgz"),         uL("L2")        },
			{ ABS_Z,          uL(":/icon/icons/gamepad_xbox_right_stick_left.svgz"),    uL("RS LEFT")   },
			{ ABS_Z,          uL(":/icon/icons/gamepad_xbox_right_stick_right.svgz"),   uL("RS RIGHT")  },
			{ ABS_RZ,         uL(":/icon/icons/gamepad_xbox_right_stick_up.svgz"),      uL("RS UP")     },
			{ ABS_RZ,         uL(":/icon/icons/gamepad_xbox_right_stick_down.svgz"),    uL("RS DOWN")   },
			{ ABS_RY,         uL(":/icon/icons/gamepad_playstation_3_r2.svgz"),         uL("R2")        },
			{ ABS_RY,         uL(":/icon/icons/gamepad_playstation_3_r2.svgz"),         uL("R2")        },
			{ ABS_HAT0X,      uL(":/icon/icons/gamepad_playstation_3_dpad_left.svgz"),  uL("DPAD LEFT") },
			{ ABS_HAT0X,      uL(":/icon/icons/gamepad_playstation_3_dpad_right.svgz"), uL("DPAD RIGHT")},
			{ ABS_HAT0Y,      uL(":/icon/icons/gamepad_playstation_3_dpad_up.svgz"),    uL("DPAD UP")   },
			{ ABS_HAT0Y,      uL(":/icon/icons/gamepad_playstation_3_dpad_down.svgz"),  uL("DPAD DOWN") }
		}
	},
	// Playstation 4
	{
		JS_SC_SONY_PS4_GAMEPAD,
		TRUE,
		JS_DB_NO_VENDOR_ID,
		JS_DB_NO_PRODUCT_ID,
		{
			/* BUT_A  */ JS_BTN_DEF(BTN_X),
			/* BUT_B  */ JS_BTN_DEF(BTN_B),
			/* SELECT */ JS_BTN_DEF(BTN_MODE),
			/* START  */ JS_BTN_DEF(BTN_THUMBL),
			/* UP     */ JS_ABS_DEF(ABS_Y, 0),
			/* DOWN   */ JS_ABS_DEF(ABS_Y, 1),
			/* LEFT   */ JS_ABS_DEF(ABS_X, 0),
			/* RIGHT  */ JS_ABS_DEF(ABS_X, 1),
			/* TRB_A  */ JS_BTN_DEF(BTN_Y),
			/* TRB_B  */ JS_BTN_DEF(BTN_A)
		},
		{
			{ BTN_B,          uL(":/icon/icons/gamepad_playstation_x.svgz"),            uL("CROSS")     },
			{ BTN_X,          uL(":/icon/icons/gamepad_playstation_c.svgz"),            uL("CIRCLE")    },
			{ BTN_A,          uL(":/icon/icons/gamepad_playstation_s.svgz"),            uL("SQUARE")    },
			{ BTN_Y,          uL(":/icon/icons/gamepad_playstation_t.svgz"),            uL("TRIANGLE")  },
			{ BTN_TL,         uL(":/icon/icons/gamepad_playstation_4_l1.svgz"),         uL("L1")        },
			{ BTN_TR,         uL(":/icon/icons/gamepad_playstation_4_r1.svgz"),         uL("R1")        },
			{ BTN_SELECT,     uL(":/icon/icons/gamepad_playstation_4_l2.svgz"),         uL("L2")        },
			{ BTN_START,      uL(":/icon/icons/gamepad_playstation_4_r2.svgz"),         uL("R2")        },
			{ BTN_MODE,       uL(":/icon/icons/gamepad_playstation_4_share.svgz"),      uL("SHARE")     },
			{ BTN_THUMBL,     uL(":/icon/icons/gamepad_playstation_4_options.svgz"),    uL("OPTIONS")   },
			{ BTN_DPAD_DOWN,  uL(":/icon/icons/gamepad_playstation_home.svgz"),         uL("HOME")      },
			{ BTN_THUMBR,     uL(":/icon/icons/gamepad_xbox_left_stick_click.svgz"),    uL("LS CLICK")  },
			{ BTN_DPAD_UP,    uL(":/icon/icons/gamepad_xbox_right_stick_click.svgz"),   uL("RS CLICK")  },
			{ BTN_DPAD_LEFT,  uL(":/icon/icons/gamepad_playstation_4_tpad_click.svgz"), uL("TPAD CLICK")}
		},
		{
			{ ABS_X,          uL(":/icon/icons/gamepad_xbox_left_stick_left.svgz"),     uL("LS LEFT")   },
			{ ABS_X,          uL(":/icon/icons/gamepad_xbox_left_stick_right.svgz"),    uL("LS RIGHT")  },
			{ ABS_Y,          uL(":/icon/icons/gamepad_xbox_left_stick_up.svgz"),       uL("LS UP")     },
			{ ABS_Y,          uL(":/icon/icons/gamepad_xbox_left_stick_down.svgz"),     uL("LS DOWN")   },
			{ ABS_RX,         uL(":/icon/icons/gamepad_playstation_4_l2.svgz"),         uL("L2")        },
			{ ABS_RX,         uL(":/icon/icons/gamepad_playstation_4_l2.svgz"),         uL("L2")        },
			{ ABS_Z,          uL(":/icon/icons/gamepad_xbox_right_stick_left.svgz"),    uL("RS LEFT")   },
			{ ABS_Z,          uL(":/icon/icons/gamepad_xbox_right_stick_right.svgz"),   uL("RS RIGHT")  },
			{ ABS_RZ,         uL(":/icon/icons/gamepad_xbox_right_stick_up.svgz"),      uL("RS UP")     },
			{ ABS_RZ,         uL(":/icon/icons/gamepad_xbox_right_stick_down.svgz"),    uL("RS DOWN")   },
			{ ABS_RY,         uL(":/icon/icons/gamepad_playstation_4_r2.svgz"),         uL("R2")        },
			{ ABS_RY,         uL(":/icon/icons/gamepad_playstation_4_r2.svgz"),         uL("R2")        },
			{ ABS_HAT0X,      uL(":/icon/icons/gamepad_playstation_4_dpad_left.svgz"),  uL("DPAD LEFT") },
			{ ABS_HAT0X,      uL(":/icon/icons/gamepad_playstation_4_dpad_right.svgz"), uL("DPAD RIGHT")},
			{ ABS_HAT0Y,      uL(":/icon/icons/gamepad_playstation_4_dpad_up.svgz"),    uL("DPAD UP")   },
			{ ABS_HAT0Y,      uL(":/icon/icons/gamepad_playstation_4_dpad_down.svgz"),  uL("DPAD DOWN") }
		}
	},
	// Hama Game USB Joystick
	{
		JS_SC_UNKNOWN,
		FALSE,
		0xF766,
		0x0001,
		{
			/* BUT_A  */ JS_BTN_DEF(BTN_X),
			/* BUT_B  */ JS_BTN_DEF(BTN_B),
			/* SELECT */ JS_BTN_DEF(BTN_MODE),
			/* START  */ JS_BTN_DEF(BTN_THUMBL),
			/* UP     */ JS_ABS_DEF(ABS_HAT0Y, 0),
			/* DOWN   */ JS_ABS_DEF(ABS_HAT0Y, 1),
			/* LEFT   */ JS_ABS_DEF(ABS_HAT0X, 0),
			/* RIGHT  */ JS_ABS_DEF(ABS_HAT0X, 1),
			/* TRB_A  */ JS_BTN_DEF(BTN_Y),
			/* TRB_B  */ JS_BTN_DEF(BTN_A)
		},
		{},
		{}
	},
	// Retro-Bit NES adapter
	{
		JS_SC_UNKNOWN,
		FALSE,
		0x1292,
		0x4643,
		{
			/* BUT_A  */ JS_BTN_DEF(BTN_A),
			/* BUT_B  */ JS_BTN_DEF(BTN_B),
			/* SELECT */ JS_BTN_DEF(BTN_X),
			/* START  */ JS_BTN_DEF(BTN_Y),
			/* UP     */ JS_ABS_DEF(ABS_HAT0Y, 0),
			/* DOWN   */ JS_ABS_DEF(ABS_HAT0Y, 1),
			/* LEFT   */ JS_ABS_DEF(ABS_HAT0X, 0),
			/* RIGHT  */ JS_ABS_DEF(ABS_HAT0X, 1),
			/* TRB_A  */ 0x000,
			/* TRB_B  */ 0x000
		},
		{},
		{}
	},
	// SNES adapter
	{
		JS_SC_UNKNOWN,
		FALSE,
		0x0E8F,
		0x3013,
		{
			/* BUT_A  */ JS_BTN_DEF(BTN_B),
			/* BUT_B  */ JS_BTN_DEF(BTN_X),
			/* SELECT */ JS_BTN_DEF(BTN_MODE),
			/* START  */ JS_BTN_DEF(BTN_THUMBL),
			/* UP     */ JS_ABS_DEF(ABS_HAT0Y, 0),
			/* DOWN   */ JS_ABS_DEF(ABS_HAT0Y, 1),
			/* LEFT   */ JS_ABS_DEF(ABS_HAT0X, 0),
			/* RIGHT  */ JS_ABS_DEF(ABS_HAT0X, 1),
			/* TRB_A  */ JS_BTN_DEF(BTN_A),
			/* TRB_B  */ JS_BTN_DEF(BTN_Y)
		},
		{},
		{}
	},
	// Mamba Retro SNES adapter
	{
			JS_SC_UNKNOWN,
			FALSE,
			0x081F,
			0xE401,
			{
				/* BUT_A  */ JS_BTN_DEF(BTN_B),
				/* BUT_B  */ JS_BTN_DEF(BTN_X),
				/* SELECT */ JS_BTN_DEF(BTN_MODE),
				/* START  */ JS_BTN_DEF(BTN_THUMBL),
				/* UP     */ JS_ABS_DEF(ABS_HAT0Y, 0),
				/* DOWN   */ JS_ABS_DEF(ABS_HAT0Y, 1),
				/* LEFT   */ JS_ABS_DEF(ABS_HAT0X, 0),
				/* RIGHT  */ JS_ABS_DEF(ABS_HAT0X, 1),
				/* TRB_A  */ JS_BTN_DEF(BTN_A),
				/* TRB_B  */ JS_BTN_DEF(BTN_Y)
			},
			{},
			{}
		}
};

#endif /* _JSTICK_DB_H_ */
