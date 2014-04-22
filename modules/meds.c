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
#include <drivers/buzzer.h>

int med_alert = 0;
int prn = 0;
int prn_effects = 0;
uint8_t prn_cooldown = CONFIG_MOD_MEDS_PRN_COOLDOWN;
uint8_t am_hour = CONFIG_MOD_MEDS_AM;
uint8_t pm_hour = CONFIG_MOD_MEDS_PM;
uint8_t am_minute = CONFIG_MOD_MEDS_AM_MIN;
uint8_t pm_minute = CONFIG_MOD_MEDS_PM_MIN;
uint8_t bed_hour = CONFIG_MOD_MEDS_BED;
uint8_t bed_minute = CONFIG_MOD_MEDS_BED_MIN;
static uint8_t tmp_hh, tmp_mm, prn_hh, prn_mm, prn_effect_hh, prn_effect_mm;

void med_check();
void alarmCheck(uint8_t hour, uint8_t minute);


static void num_press()
{
	//Do Something	
	display_clear(0, 2);
	display_chars(0, LCD_SEG_L2_4_0, "DONE", SEG_ON);

	//Clear Notice, Update Display
    display_symbol(0, LCD_ICON_ALARM, SEG_OFF);
	med_alert = 0;
}

static void prn_press()
{
	prn = 1;
	prn_effects = 1;
	display_clear(0, 2);
	display_chars(0, LCD_SEG_L2_4_0, "PRN", SEG_ON);
	display_symbol(0, LCD_ICON_HEART, SEG_ON);
	prn_hh = (rtca_time.hour + prn_cooldown) % 24;
	prn_mm = rtca_time.min;
    prn_effect_hh = rtca_time.hour;
    prn_effect_mm = rtca_time.min + 12;
}

static void meds_activate()
{
	display_chars(0, LCD_SEG_L2_4_0, "MEDS", SEG_ON);
}

static void meds_deactivate()
{
	display_clear(0, 2);
}

void mod_meds_init(void)
{
	sys_messagebus_register(med_check, SYS_MSG_RTC_MINUTE);	
	menu_add_entry("MEDS", &prn_press, NULL, &num_press, NULL, NULL, NULL,
						&meds_activate,
						&meds_deactivate);
}

void med_check()
{
	tmp_hh = rtca_time.hour;
	tmp_mm = rtca_time.min;
	
	if(tmp_hh == prn_hh && tmp_mm == prn_mm)
	{
	    prn = 0;
	    display_symbol(0, LCD_ICON_HEART, SEG_OFF);
	}
	
	if(prn == 1)
	{
	    	    display_symbol(0, LCD_ICON_HEART, SEG_ON);
	    	    if (prn_effects == 1)
	    	    {
	    	        display_symbol(0, LCD_ICON_HEART, BLINK_ON);
	    	    } else {
	    	        display_symbol(0, LCD_ICON_HEART, BLINK_OFF);
	    	    }
	} else {
	    display_symbol(0, LCD_ICON_HEART, BLINK_OFF);
	    display_symbol(0, LCD_ICON_HEART, SEG_OFF);
	}
	
	
	if(tmp_hh == prn_effect_hh && tmp_mm == prn_effect_mm)
	{
	    prn_effects = 0;
	}
	
/*	if(tmp_hh == am_hour && tmp_mm == pm_minute)
		{
			med_alert = 1;
		}

	if(tmp_hh == pm_hour && tmp_mm == pm_minute)
		{
			med_alert = 1;
		}

	if(tmp_hh == bed_hour && tmp_mm == bed_minute)
		{
			med_alert = 1;
		}
*/

    alarmCheck(am_hour, am_minute);
    alarmCheck(pm_hour, pm_minute);
    alarmCheck(bed_hour, bed_minute);

	if(med_alert == 1)
		{
			display_symbol(0, LCD_ICON_ALARM, SEG_ON);
			if((tmp_mm % 10) == 0)
			{
			    	/* Play "welcome" chord: A major */
	                note welcome[4] = {0x1901, 0x1904, 0x1908, 0x000F};
	                buzzer_play(welcome);

			}
		} else {
			display_symbol(0, LCD_ICON_ALARM, SEG_OFF);
		}
}


void alarmCheck(uint8_t hour, uint8_t minute)
{
    	if(tmp_hh == hour && tmp_mm == minute)
		{
			med_alert = 1;
		}
}