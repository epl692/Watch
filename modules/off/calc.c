/*
    modules/meds.c: meds module for openchronos-ng

    Copyright (C) 2012 Eric Lynema <epl692@sdf.org>
 
	            http://www.openchronos-ng.sourceforge.net

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


#include <openchronos.h>

#include <drivers/display.h>
#include <drivers/rtca.h>

int med_alert = 0;
uint8_t am_hour = CONFIG_MOD_MEDS_AM;
uint8_t pm_hour = CONFIG_MOD_MEDS_PM;
uint8_t am_minute = CONFIG_MOD_MEDS_AM_MIN;
uint8_t pm_minute = CONFIG_MOD_MEDS_PM_MIN;
static uint8_t tmp_hh, tmp_mm;

void med_check();

static void num_press()
{
	//Do Something	
	display_clear(0, 2);
	display_chars(0, LCD_SEG_L2_4_0, "DONE", SEG_ON);

	//Clear Notice, Update Display
        display_symbol(0, LCD_ICON_ALARM, SEG_OFF);
	med_alert = 0;
}


static void meds_activate()
{
	display_chars(0, LCD_SEG_L2_4_0, "MEDS", SEG_ON);
	//display_symbol(0, LCD_ICON_ALARM, SEG_ON);
	//refresh_screen();
}

static void meds_deactivate()
{
	display_clear(0, 2);
}

void mod_meds_init(void)
{
	sys_messagebus_register(med_check, SYS_MSG_RTC_MINUTE);	
	menu_add_entry("MEDS", NULL, NULL, &num_press, NULL, NULL, NULL,
						&meds_activate,
						&meds_deactivate);
}

void med_check()
{
	tmp_hh = rtca_time.hour;
	tmp_mm = rtca_time.min;
	if(tmp_hh == am_hour && tmp_mm == pm_minute)
		{
			med_alert = 1;
		}

	if(tmp_hh == pm_hour && tmp_mm == pm_minute)
		{
			med_alert = 1;
		}

	if(med_alert == 1)
		{
			display_symbol(0, LCD_ICON_ALARM, SEG_ON);
		} else {
			display_symbol(0, LCD_ICON_ALARM, SEG_OFF);
		}
}
